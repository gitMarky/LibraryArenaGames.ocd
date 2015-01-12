/**
 This is a rule object that enables the game to run in rounds. That means after a goal is fulfilled, instead
 of finishing the scenario, the game starts merely a new round.
 {@section Script Architecture}
 @title Rounds
 @id index
 @author Marky
 @version 0.1.0
 */
 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// definitions

local Name = "$Name$";
local Description = "$Description$";

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// global functions

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// finished functions

static const ROUND_Number_Default = -1;

// locals: internal information

local round_counter;	 // int: the number of the current round. Starts at 1.
local round_has_started; // bool: true - the round has started, enables countdown
                         //       false - the round is inactive, for example when a chooser is running etc.
local round_end_blocker; // array: the round cannot be finished until all of these objects have been removed
local round_start_blocker; // dito

// locals: configuration

local cycle_chooser;	// bool: every player can configure a round once, in order?
local chooser_id;		// def:  a chooser object that handles the configuration of the round
local chooser_player;	// int:  this player configures the round
local max_rounds;		// int:  the game automatically stops after this many rounds


protected func Construction()
{
	round_counter = 0; // on purpose
	round_has_started = false;
	
	cycle_chooser = false;
	chooser_id = nil;
	chooser_player = 0;
	max_rounds = ROUND_Number_Default;
	
	round_end_blocker = CreateArray();
	round_start_blocker = CreateArray();
	
	ScheduleCall(this, "NextRound", 5, 0);
}

/**
 Displays a message window with the description of the object.
 @par player_index The player who selected the object in the rules menu.
 @return bool {@c true}.
 */
public func Activate(int player_index)
{
	MessageWindow(GetProperty("Description"), player_index);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// non-functional and temporary stuff

global func RoundManager()
{
	var manager = FindObject(Find_ID(Rule_RoundManager));
	
	if (manager == nil)
	{
		FatalError(Format("A script relies on an existing %i object, but no such object exists", Rule_RoundManager));
	}
	else
	{
		return manager;
	}
}

public func RegisterRoundEndBlocker(object blocker)
{
	CheckBlocker(blocker);

	if (!IsValueInArray(round_end_blocker, blocker))
	{
		PushBack(round_end_blocker, blocker);
	}
}

public func RegisterRoundStartBlocker(object blocker)
{
	CheckBlocker(blocker);

	if (!IsValueInArray(round_start_blocker, blocker))
	{
		PushBack(round_start_blocker, blocker);
	}
}

/**
 Ends the current round and prepares a new round.
 Issues the game call {@c OnRoundEnd(int round_number)}, increases the round counter and calls {@c OnRoundReset(int round_number)}.
 @version 0.1.0
 */
protected func NextRound()
{
	// increase round number
	round_counter++;
	
	GameCallEx("OnRoundReset", round_counter);
}

protected func OnRoundEnd()
{
	// TODO: wait until no object with "BlockRoundEnd" returns true
	// then reset  the round
}

/**
 GameCall when the round is reset. 
 @version 0.1.0
 */
protected func OnRoundReset(int round_number)
{
/*
	if (chooser_id == nil)
	{
		// nothing to do yet, just start the new round
		GameCallEx("OnRoundStart", round_counter);
	}
	else
	{
		// has a chooser? Configure the settings!
		if (cycle_chooser)
		{
			chooser_player = (round_counter - 1) % GetPlayerCount(C4PT_User);
		}
		
		// TODO
		
		// nothing to do yet, just start the new round
		GameCallEx("OnRoundStart", round_counter);
	}
*/
	if (chooser_id != nil)
	{
		// has a chooser? Configure the settings!
		if (cycle_chooser)
		{
			chooser_player = (round_counter - 1) % GetPlayerCount(C4PT_User);
		}
	}
	
	ScheduleCall(this, "PrepareRoundStart", 5, 0);
}

public func PrepareRoundStart()
{
	if (GetLength(round_start_blocker) == 0)
	{
		DoRoundStart();
	}
}

public func PrepareRoundEnd()
{
	if (GetLength(round_end_blocker) == 0)
	{
		DoRoundEnd();
		NextRound();
	}
}

private func DoRoundEnd()
{
	// reset the activity status
	round_has_started = false;
	GameCallEx("OnRoundEnd", round_counter);
}

private func DoRoundStart()
{
	round_has_started = true;
	GameCallEx("OnRoundStart", round_counter);
	
	ScheduleCall(this, "PrepareRoundEnd", 5, 0);
}



public func RemoveRoundEndBlocker(object blocker)
{
	CheckBlocker(blocker);

	RemoveArrayValue(round_end_blocker, blocker, false);
	
	PrepareRoundEnd();
}

public func RemoveRoundStartBlocker(object blocker)
{
	CheckBlocker(blocker);

	RemoveArrayValue(round_start_blocker, blocker, false);
	
	PrepareRoundStart();
}

private func CheckBlocker(object blocker)
{
	if (blocker == nil)
	{
		FatalError(Format("Must specify an existing object! Parameter is %v", blocker));
	}
}