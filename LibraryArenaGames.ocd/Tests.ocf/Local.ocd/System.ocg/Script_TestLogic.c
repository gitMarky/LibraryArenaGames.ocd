
/* --- Test Control --- */

// Aborts the current test and launches the specified test instead.
global func LaunchTest(int nr)
{
	// Get the control test.
	var test = CurrentTest();
	if (test)
	{
		// Finish the currently running test.
		Call(Format("~Test%d_OnFinished", test.testnr));
		// Start the requested test by just setting the test number and setting 
		// test.launched to false, effect will handle the rest.
	}
	else
	{
		// Create a new control effect and launch the test.
		test = Scenario->CreateEffect(IntTestControl, 100, 2);
		test.player = GetPlayerByIndex(0, C4PT_User);
		test.global_result = true;
		test.count_total = 0;
		test.count_failed = 0;
		test.count_skipped = 0;
	}

	test.testnr = nr;
	test.launched = false;
}

// Calling this function skips the current test, does not work if last test has been ran already.
global func SkipTest()
{
	// Get the control test.
	var test = CurrentTest();
	if (test)
	{
		// Finish the previous test.
		Call(Format("~Test%d_OnFinished", test.testnr));
		// Start the next test by just increasing the test number and setting 
		// test.launched to false, effect will handle the rest.
		test.testnr++;
		test.launched = false;
		test.count_skipped++;
	}
}


/* --- Test Effect --- */

static const IntTestControl = new Effect
{
	Start = func (int temporary)
	{
		if (!temporary)
		{
			// Set default interval.
			this.Interval = 1;
		}
		return FX_OK;
	},

	Timer = func ()
	{
		// Launch new test if needed.
		if (!this.launched)
		{
			// Log test start.
			Log("=====================================");
			Log("Test %d started:", this.testnr);
			// Start the test if available, otherwise finish test sequence.
			if (!Call(Format("~Test%d_OnStart", this.testnr), this.player))
			{
				Log("Test %d not available, the previous test was the last test.", this.testnr);
				Log("=====================================");
				Log("All tests have been completed!");
				Log("* %d tests total", this.count_total);
				Log("%d tests failed", this.count_failed);
				Log("%d tests skipped", this.count_skipped);
				Log("=====================================");
				if (this.count_skipped == 0 && this.count_failed == 0 && this.count_total > 0)
				{
					Log("All tests passed!");
				}
				else
				{
					Log("At least one test failed or was skipped!");
				}
				return FX_Execute_Kill;
			}
			this.launched = true;
			this.count_total++;
		}
		
		// waiting
		if (this.wait > 0)
		{
			this.wait -= 1;
			return FX_OK;
		}
		
		// Check whether the current test has been finished.
		if (Call(Format("Test%d_Execute", this.testnr)))
		{
			this.launched = false;
			
			if (this.current_result)
			{
				Log(">> Test %d passed.", this.testnr);
			}
			else
			{
				Log(">> Test %d failed.", this.testnr);
				this.count_failed++;
			}
			
			// Update global result
			this.global_result &= this.current_result;
			this.current_result = false;
	
			// Call the test on finished function.
			Call(Format("~Test%d_OnFinished", this.testnr));
			// Log result and increase test number.
			Log("Test %d successfully completed.", this.testnr);
			this.testnr++;
		}
		return FX_OK;
	}
};


global func doTest(description, returned, expected)
{
	var test = (returned == expected);
	
	var predicate = "[Fail]";
	if (test) predicate = "[Pass]";
	
	Log(Format("%s %s", predicate, description), returned, expected);
	return test;
}


global func CurrentTest()
{
	return GetEffect("IntTestControl", Scenario);
}


global func PassTest()
{
	CurrentTest().current_result = true;
	return true;
}


global func FailTest()
{
	CurrentTest().current_result = false;
	return true;
}


global func Wait(int amount)
{
	CurrentTest().wait = Max(0, amount);
	return false;
}
