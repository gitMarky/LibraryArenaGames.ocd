/**
	Dummy object. You can create this if you want to test the round system
	without having proper conditions for round end and start yet.
 */

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";

/* --- Interface --- */

public func TriggerRoundStart()
{
	DebugLog("[%d] +60 Removing myself from the blocker list", FrameCounter());
	RoundManager()->RemoveRoundStartBlocker(this);
}


public func TriggerRoundEnd()
{
	DebugLog("[%d] + 360 RoundHelper: stop the round", FrameCounter());
	RoundManager()->RemoveRoundEndBlocker(this);
}


/* --- Callbacks from round manager --- */


func OnRoundReset(int counter)
{
	DebugLog("[%d] RoundHelper preparing for round %d", FrameCounter(), counter);
	RoundManager()->RegisterRoundStartBlocker(this);
	
	ScheduleCall(this, "TriggerRoundStart", 60, 0);
}


func OnRoundStart(int counter)
{
	DebugLog("[%d] RoundHelper: Round %d starts", FrameCounter(), counter);
	RoundManager()->RegisterRoundEndBlocker(this);
	ScheduleCall(this, "TriggerRoundEnd", 36*10, 0);
}


func OnRoundEnd(int counter)
{
	DebugLog("[%d] RoundHelper: Round %d ends", FrameCounter(), counter);
}
