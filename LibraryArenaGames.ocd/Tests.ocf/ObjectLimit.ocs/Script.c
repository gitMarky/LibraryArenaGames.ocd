
func InitializePlayer(int player)
{
	// Set zoom and move player to the middle of the scenario.
	SetPlayerZoomByViewRange(player, LandscapeWidth(), nil, PLRZOOM_Direct);
	SetFoW(false, player);
	GetCrew(player)->SetPosition(120, 190);
	GetCrew(player)->MakeInvincible();

	if (GetPlayerCount() < 2)
	{
		CreateScriptPlayer("Second player", RGB(0, 0, 255), nil, CSPF_NoEliminationCheck);
	}

	// Add test control effect.
	if (player == 0)
	{
		LaunchTest(1);
	}
	return true;
}

/* --- Tests --- */



/* --- Basics --- */

global func Test1_OnStart(int player){ return true;}
global func Test1_OnFinished(){ return; }
global func Test1_Execute()
{
	Log("Test default values");

	doTest("Library default value 'ObjectLimitPlayer' is %d, should be %d", Library_ObjectLimit.ObjectLimitPlayer, -1);
	doTest("Library default value 'ObjectLimitGlobal' is %d, should be %d", Library_ObjectLimit.ObjectLimitGlobal, -1);
	doTest("Library default value 'ObjectLimitReplace' is %v, should be %v", Library_ObjectLimit.ObjectLimitReplace, true);

	return Evaluate();
}


/* --- Player limit --- */

global func Test2_OnStart(int player){ return true;}
global func Test2_OnFinished(){ return; }
global func Test2_Execute()
{
	Log("Test player limits, replace old objects");

	// Create 3 objects, all should be there
	var instance1_noowner_object1 = CreateObject(Test_Limit_Player3_Replace, 0, 0, NO_OWNER);
	var instance2_player0_object1 = CreateObject(Test_Limit_Player3_Replace, 0, 0, 0);
	var instance3_player0_object2 = CreateObject(Test_Limit_Player3_Replace, 0, 0, 0);

	doTest("Step 1: Instance 1, no owner, object 1 is created. Is %v, should be %v", nil != instance1_noowner_object1, true);
	doTest("Step 1: Instance 2, player 0, object 1 is created. Is %v, should be %v", nil != instance2_player0_object1, true);
	doTest("Step 1: Instance 3, player 0, object 2 is created. Is %v, should be %v", nil != instance3_player0_object2, true);

	// Create a fourth object for second player, all should be there
	var instance4_player1_object1 = CreateObject(Test_Limit_Player3_Replace, 0, 0, 1);

	doTest("Step 2: Instance 1, no owner, object 1 exists. Is %v, should be %v", nil != instance1_noowner_object1, true);
	doTest("Step 2: Instance 2, player 0, object 1 exists. Is %v, should be %v", nil != instance2_player0_object1, true);
	doTest("Step 2: Instance 3, player 0, object 2 exists. Is %v, should be %v", nil != instance3_player0_object2, true);
	doTest("Step 2: Instance 4, player 1, object 1 is created. Is %v, should be %v", nil != instance4_player1_object1, true);

	// Create a third object for first player, all should be there
	var instance5_player0_object3 = CreateObject(Test_Limit_Player3_Replace, 0, 0, 0);

	doTest("Step 3: Instance 1, no owner, object 1 exists. Is %v, should be %v", nil != instance1_noowner_object1, true);
	doTest("Step 3: Instance 2, player 0, object 1 exists. Is %v, should be %v", nil != instance2_player0_object1, true);
	doTest("Step 3: Instance 3, player 0, object 2 exists. Is %v, should be %v", nil != instance3_player0_object2, true);
	doTest("Step 3: Instance 4, player 1, object 1 exists. Is %v, should be %v", nil != instance4_player1_object1, true);
	doTest("Step 3: Instance 5, player 0, object 3 is created. Is %v, should be %v", nil != instance5_player0_object3, true);

	// Create a fourth object for first player, first of his objects should vanish
	var instance6_player0_object4 = CreateObject(Test_Limit_Player3_Replace, 0, 0, 0);

	doTest("Step 4: Instance 1, no owner, object 1 exists. Is %v, should be %v", nil != instance1_noowner_object1, true);
	doTest("Step 4: Instance 2, player 0, object 1 removed. Is %v, should be %v", nil == instance2_player0_object1, true);
	doTest("Step 4: Instance 3, player 0, object 2 exists. Is %v, should be %v", nil != instance3_player0_object2, true);
	doTest("Step 4: Instance 4, player 1, object 1 exists. Is %v, should be %v", nil != instance4_player1_object1, true);
	doTest("Step 4: Instance 5, player 0, object 3 exists. Is %v, should be %v", nil != instance5_player0_object3, true);
	doTest("Step 4: Instance 6, player 0, object 4 exists. Is %v, should be %v", nil != instance6_player0_object4, true);

	RemoveAll(Find_ID(Test_Limit_Player3_Replace));
	return Evaluate();
}

global func Test3_OnStart(int player){ return true;}
global func Test3_OnFinished(){ return; }
global func Test3_Execute()
{
	Log("Test player limits, prevent new objects");

	// Create 3 objects, all should be there
	var instance1_noowner_object1 = CreateObject(Test_Limit_Player3_Prevent, 0, 0, NO_OWNER);
	var instance2_player0_object1 = CreateObject(Test_Limit_Player3_Prevent, 0, 0, 0);
	var instance3_player0_object2 = CreateObject(Test_Limit_Player3_Prevent, 0, 0, 0);

	doTest("Step 1: Instance 1, no owner, object 1 is created. Is %v, should be %v", nil != instance1_noowner_object1, true);
	doTest("Step 1: Instance 2, player 0, object 1 is created. Is %v, should be %v", nil != instance2_player0_object1, true);
	doTest("Step 1: Instance 3, player 0, object 2 is created. Is %v, should be %v", nil != instance3_player0_object2, true);

	// Create a fourth object for second player, all should be there
	var instance4_player1_object1 = CreateObject(Test_Limit_Player3_Prevent, 0, 0, 1);

	doTest("Step 2: Instance 1, no owner, object 1 exists. Is %v, should be %v", nil != instance1_noowner_object1, true);
	doTest("Step 2: Instance 2, player 0, object 1 exists. Is %v, should be %v", nil != instance2_player0_object1, true);
	doTest("Step 2: Instance 3, player 0, object 2 exists. Is %v, should be %v", nil != instance3_player0_object2, true);
	doTest("Step 2: Instance 4, player 1, object 1 is created. Is %v, should be %v", nil != instance4_player1_object1, true);

	// Create a third object for first player, all should be there
	var instance5_player0_object3 = CreateObject(Test_Limit_Player3_Prevent, 0, 0, 0);

	doTest("Step 3: Instance 1, no owner, object 1 exists. Is %v, should be %v", nil != instance1_noowner_object1, true);
	doTest("Step 3: Instance 2, player 0, object 1 exists. Is %v, should be %v", nil != instance2_player0_object1, true);
	doTest("Step 3: Instance 3, player 0, object 2 exists. Is %v, should be %v", nil != instance3_player0_object2, true);
	doTest("Step 3: Instance 4, player 1, object 1 exists. Is %v, should be %v", nil != instance4_player1_object1, true);
	doTest("Step 3: Instance 5, player 0, object 3 is created. Is %v, should be %v", nil != instance5_player0_object3, true);

	// Create a fourth object for first player, first of his objects should vanish
	var instance6_player0_object4 = CreateObject(Test_Limit_Player3_Prevent, 0, 0, 0);

	doTest("Step 4: Instance 1, no owner, object 1 exists. Is %v, should be %v", nil != instance1_noowner_object1, true);
	doTest("Step 4: Instance 2, player 0, object 1 exists. Is %v, should be %v", nil != instance2_player0_object1, true);
	doTest("Step 4: Instance 3, player 0, object 2 exists. Is %v, should be %v", nil != instance3_player0_object2, true);
	doTest("Step 4: Instance 4, player 1, object 1 exists. Is %v, should be %v", nil != instance4_player1_object1, true);
	doTest("Step 4: Instance 5, player 0, object 3 exists. Is %v, should be %v", nil != instance5_player0_object3, true);
	doTest("Step 4: Instance 6, player 0, object 4 removed. Is %v, should be %v", nil == instance6_player0_object4, true);

	RemoveAll(Find_ID(Test_Limit_Player3_Prevent));
	return Evaluate();
}

global func Test4_OnStart(int player){ return true;}
global func Test4_OnFinished(){ return; }
global func Test4_Execute()
{
	Log("Test player limits, no owner should be able to have unlimited objects");

	// Create 4 objects, all should be there
	var instance1_noowner_object1 = CreateObject(Test_Limit_Player3_Replace, 0, 0, NO_OWNER);
	var instance2_noowner_object2 = CreateObject(Test_Limit_Player3_Replace, 0, 0, NO_OWNER);
	var instance3_noowner_object3 = CreateObject(Test_Limit_Player3_Replace, 0, 0, NO_OWNER);
	var instance4_noowner_object4 = CreateObject(Test_Limit_Player3_Replace, 0, 0, NO_OWNER);

	var instance5_noowner_object1 = CreateObject(Test_Limit_Player3_Prevent, 0, 0, NO_OWNER);
	var instance6_noowner_object2 = CreateObject(Test_Limit_Player3_Prevent, 0, 0, NO_OWNER);
	var instance7_noowner_object3 = CreateObject(Test_Limit_Player3_Prevent, 0, 0, NO_OWNER);
	var instance8_noowner_object4 = CreateObject(Test_Limit_Player3_Prevent, 0, 0, NO_OWNER);

	doTest("Instance 1, no owner, object 1 is created. Is %v, should be %v", nil != instance1_noowner_object1, true);
	doTest("Instance 2, no owner, object 2 is created. Is %v, should be %v", nil != instance2_noowner_object2, true);
	doTest("Instance 3, no owner, object 3 is created. Is %v, should be %v", nil != instance3_noowner_object3, true);
	doTest("Instance 4, no owner, object 4 is created. Is %v, should be %v", nil != instance4_noowner_object4, true);

	doTest("Instance 5, no owner, object 1 is created. Is %v, should be %v", nil != instance5_noowner_object1, true);
	doTest("Instance 6, no owner, object 2 is created. Is %v, should be %v", nil != instance6_noowner_object2, true);
	doTest("Instance 7, no owner, object 3 is created. Is %v, should be %v", nil != instance7_noowner_object3, true);
	doTest("Instance 8, no owner, object 4 is created. Is %v, should be %v", nil != instance8_noowner_object4, true);

	RemoveAll(Find_ID(Test_Limit_Player3_Prevent), Find_ID(Test_Limit_Player3_Replace));
	return Evaluate();
}


/* --- Global limit --- */

global func Test5_OnStart(int player){ return true;}
global func Test5_OnFinished(){ return; }
global func Test5_Execute()
{
	Log("Test global limits, replace old objects");

	// Create 3 objects, all should be there
	var instance1_noowner_object1 = CreateObject(Test_Limit_Global3_Replace, 0, 0, NO_OWNER);
	var instance2_player0_object1 = CreateObject(Test_Limit_Global3_Replace, 0, 0, 0);
	var instance3_player0_object2 = CreateObject(Test_Limit_Global3_Replace, 0, 0, 0);

	doTest("Step 1: Instance 1, no owner, object 1 is created. Is %v, should be %v", nil != instance1_noowner_object1, true);
	doTest("Step 1: Instance 2, player 0, object 1 is created. Is %v, should be %v", nil != instance2_player0_object1, true);
	doTest("Step 1: Instance 3, player 0, object 2 is created. Is %v, should be %v", nil != instance3_player0_object2, true);

	// Create a fourth object for second player
	var instance4_player1_object1 = CreateObject(Test_Limit_Global3_Replace, 0, 0, 1);

	doTest("Step 2: Instance 1, no owner, object 1 removed. Is %v, should be %v", nil == instance1_noowner_object1, true);
	doTest("Step 2: Instance 2, player 0, object 1 exists. Is %v, should be %v", nil != instance2_player0_object1, true);
	doTest("Step 2: Instance 3, player 0, object 2 exists. Is %v, should be %v", nil != instance3_player0_object2, true);
	doTest("Step 2: Instance 4, player 1, object 1 exists. Is %v, should be %v", nil != instance4_player1_object1, true);

	// Create a third object for first player
	var instance5_player0_object3 = CreateObject(Test_Limit_Global3_Replace, 0, 0, 0);

	doTest("Step 3: Instance 1, no owner, object 1 removed. Is %v, should be %v", nil == instance1_noowner_object1, true);
	doTest("Step 3: Instance 2, player 0, object 1 removed. Is %v, should be %v", nil == instance2_player0_object1, true);
	doTest("Step 3: Instance 3, player 0, object 2 exists. Is %v, should be %v", nil != instance3_player0_object2, true);
	doTest("Step 3: Instance 4, player 1, object 1 exists. Is %v, should be %v", nil != instance4_player1_object1, true);
	doTest("Step 3: Instance 5, player 0, object 3 exists. Is %v, should be %v", nil != instance5_player0_object3, true);

	RemoveAll(Find_ID(Test_Limit_Global3_Replace));
	return Evaluate();
}

global func Test6_OnStart(int player){ return true;}
global func Test6_OnFinished(){ return; }
global func Test6_Execute()
{
	Log("Test global limits, prevent new objects");

	// Create 3 objects, all should be there
	var instance1_noowner_object1 = CreateObject(Test_Limit_Global3_Prevent, 0, 0, NO_OWNER);
	var instance2_player0_object1 = CreateObject(Test_Limit_Global3_Prevent, 0, 0, 0);
	var instance3_player0_object2 = CreateObject(Test_Limit_Global3_Prevent, 0, 0, 0);

	doTest("Step 1: Instance 1, no owner, object 1 is created. Is %v, should be %v", nil != instance1_noowner_object1, true);
	doTest("Step 1: Instance 2, player 0, object 1 is created. Is %v, should be %v", nil != instance2_player0_object1, true);
	doTest("Step 1: Instance 3, player 0, object 2 is created. Is %v, should be %v", nil != instance3_player0_object2, true);

	// Create a fourth object for second player
	var instance4_player1_object1 = CreateObject(Test_Limit_Global3_Prevent, 0, 0, 1);

	doTest("Step 2: Instance 1, no owner, object 1 exists. Is %v, should be %v", nil != instance1_noowner_object1, true);
	doTest("Step 2: Instance 2, player 0, object 1 exists. Is %v, should be %v", nil != instance2_player0_object1, true);
	doTest("Step 2: Instance 3, player 0, object 2 exists. Is %v, should be %v", nil != instance3_player0_object2, true);
	doTest("Step 2: Instance 4, player 1, object 1 removed. Is %v, should be %v", nil == instance4_player1_object1, true);

	// Create a third object for first player
	var instance5_player0_object3 = CreateObject(Test_Limit_Global3_Prevent, 0, 0, 0);

	doTest("Step 3: Instance 1, no owner, object 1 exists. Is %v, should be %v", nil != instance1_noowner_object1, true);
	doTest("Step 3: Instance 2, player 0, object 1 exists. Is %v, should be %v", nil != instance2_player0_object1, true);
	doTest("Step 3: Instance 3, player 0, object 2 exists. Is %v, should be %v", nil != instance3_player0_object2, true);
	doTest("Step 3: Instance 4, player 1, object 1 removed. Is %v, should be %v", nil == instance4_player1_object1, true);
	doTest("Step 3: Instance 5, player 0, object 3 removed. Is %v, should be %v", nil == instance5_player0_object3, true);

	RemoveAll(Find_ID(Test_Limit_Global3_Prevent));
	return Evaluate();
}

global func Test7_OnStart(int player){ return true;}
global func Test7_OnFinished(){ return; }
global func Test7_Execute()
{
	Log("Test global limits, counts for no owner, too");

	// Create objects
	var instance1_noowner_object1 = CreateObject(Test_Limit_Global3_Replace, 0, 0, NO_OWNER);
	var instance2_noowner_object2 = CreateObject(Test_Limit_Global3_Replace, 0, 0, NO_OWNER);
	var instance3_noowner_object3 = CreateObject(Test_Limit_Global3_Replace, 0, 0, NO_OWNER);
	var instance4_noowner_object4 = CreateObject(Test_Limit_Global3_Replace, 0, 0, NO_OWNER);

	var instance5_noowner_object1 = CreateObject(Test_Limit_Global3_Prevent, 0, 0, NO_OWNER);
	var instance6_noowner_object2 = CreateObject(Test_Limit_Global3_Prevent, 0, 0, NO_OWNER);
	var instance7_noowner_object3 = CreateObject(Test_Limit_Global3_Prevent, 0, 0, NO_OWNER);
	var instance8_noowner_object4 = CreateObject(Test_Limit_Global3_Prevent, 0, 0, NO_OWNER);

	doTest("Instance 1, no owner, object 1 is removed. Is %v, should be %v", nil == instance1_noowner_object1, true);
	doTest("Instance 2, no owner, object 2 is created. Is %v, should be %v", nil != instance2_noowner_object2, true);
	doTest("Instance 3, no owner, object 3 is created. Is %v, should be %v", nil != instance3_noowner_object3, true);
	doTest("Instance 4, no owner, object 4 is created. Is %v, should be %v", nil != instance4_noowner_object4, true);

	doTest("Instance 5, no owner, object 1 is created. Is %v, should be %v", nil != instance5_noowner_object1, true);
	doTest("Instance 6, no owner, object 2 is created. Is %v, should be %v", nil != instance6_noowner_object2, true);
	doTest("Instance 7, no owner, object 3 is created. Is %v, should be %v", nil != instance7_noowner_object3, true);
	doTest("Instance 8, no owner, object 4 is removed. Is %v, should be %v", nil == instance8_noowner_object4, true);

	RemoveAll(Find_ID(Test_Limit_Global3_Prevent), Find_ID(Test_Limit_Global3_Replace));
	return Evaluate();
}

