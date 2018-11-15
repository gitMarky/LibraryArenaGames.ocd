/**
	Plugin for goal script.
	
	The goal lasts until a specific timeout only.
	Can interface with a clock object.

	@author Marky
*/

local goal_time_limit = 10 * 60; // Time limit, in seconds
local goal_time_clock = nil;     // Clock object

/* --- Engine callbacks --- */


func Initialize()
{
	// Start countdown immediately, or when the round starts
	if (RoundManager() == nil)
	{
		StartCountdown();
	}
	
	return _inherited(...);
}

/* --- Interface --- */

/**
	Sets the time limit.
	
	@par minutes This many minutes.
	@par seconds This many seconds.
	
	@return The goal itself, for further function calls.
 */
public func SetTimeLimit(int minutes, int seconds)
{
	goal_time_limit = minutes * 60 + seconds;
	return this;
}

/**
	Gets the time limit
	
	@return int The time limit, in seconds.
 */
public func GetTimeLimit()
{
	return goal_time_limit;
}

/**
	Sets a clock that can display the countdown.
	Otherwise, the countdown runs internally only.
	
	@par clock The clock. This can be an obect or a proplist.
	           It must provide a function "SetTime", where
	           the first parameter is the remaining time
	           in seconds.
 */
public func SetClock(proplist clock)
{
	goal_time_clock = clock;
	if (clock)
	{
		if (clock.SetTime == nil || GetType(clock.SetTime) != C4V_Function)
		{
			FatalError("The given clock does not have a function SetTime(int to_seconds)");
		}
	}
}

/**
	Is called at the end of the countdown.
 */
public func TimeExpired()
{
	// Leading faction wins
	this->DoWinRound(this->GetLeadingFaction());
}


/* --- Internals --- */

func OnRoundStart(int round)
{
	StartCountdown();
	_inherited(round, ...);
}


func OnRoundEnd(int round)
{
	RemoveCountdown(); // Just to be sure
}


func OnRoundReset(int round)
{
	RemoveCountdown(); // Just to be sure
}


func StartCountdown()
{
	if (!GetEffect("TimeLimitCountdown", this))
	{
		CreateEffect(TimeLimitCountdown, 1, FRAME_Factor_Second);
	}
}


func RemoveCountdown()
{
	RemoveEffect("TimeLimitCountdown", this);
}


local TimeLimitCountdown = new Effect
{
	Start = func (int temporary)
	{
		if (temporary)
		{
			return;
		}
		
		this.time_remaining = this.Target->GetTimeLimit();
		this->UpdateClock();
		return FX_OK;
	},

	Timer = func ()
	{
		// Remove if goal is gone
		if (!this.Target)
		{
			return FX_Execute_Kill;
		}

		this.time_remaining = Max(0, this.time_remaining - 1);
		this->UpdateClock();
		
		if (this.time_remaining > 0)
		{
			return FX_OK;
		}
		else
		{
			this.Target->TimeExpired();
			return FX_Execute_Kill;
		}
	},
	
	UpdateClock = func ()
	{
		if (this.Target.goal_time_clock)
		{
			this.Target.goal_time_clock->SetTime(this.time_remaining);
		}
	},
};
