/**
 This is a rule object that enables the game to run in rounds. That means after a goal is fulfilled, instead
 of finishing the scenario, the game starts merely a new round.
 {@section Script Architecture}
 There are phases and game calls for rounds:@br
 1.) A game call is issued: {@c GameCallEx("OnRoundReset", round_number)}.
     Objects should create a default state for a new round in this call, for example
     players should be prevented from acting by putting them in spawn points.
     Use {@link Environment_RoundManager#RegisterRoundStartBlocker} here if you want to prevent the round from
     starting immediately.@br
 2.) Configuration Phase. This is completed once all round start blockers have removed themselves
     with {@link Environment_RoundManager#RemoveRoundStartBlocker}.@br
 3.) A game call is issued: {@c GameCallEx("OnRoundStart", round_number)}.
     This is the point where interactive objects should be enabled, for example spawn points
     can spawn the players at this point.@br
     Use {@link Environment_RoundManager#RegisterRoundEndBlocker} here if you want to prevent the round from
     ending immediately.@br
 4.) Game phase. This is completed once all round end blockers have removed themselves
     with {@link Environment_RoundManager#RemoveRoundEndBlocker}.@br
 5.) A game call is issued: {@c GameCallEx("OnRoundEnd", round_number)}. Remove undesired objects now.@br
 6.) Repeat from 1.)@br
 @br
 Note that a single {@link global#RoundManager RoundManager()} object will do one thing: Rush through the rounds quickly.
 It actually needs rules or goals that register as blockers for the phases.
 {@section Constants}
 The object offers new constants:
 <table>
 	<tr><th>Name</th>                     <th>Value</th> <th>Description</th></tr>
 	<tr><td>ROUND_Number_Default</td> <td>-1</td> <td>Number of rounds that the game will run. The default value is an infinite number of rounds. Overload for a finite number of rounds.</td></tr>
 </table>
 
 
 @title Rounds
 @id index
 @author Marky
  */
 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// definitions

local Name = "$Name$";
local Description = "$Description$";

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// global functions


/**
 Use this to access the round manager object from other objects.
 @return object A {@link Environment_RoundManager#index round manager} object,
         or {@c nil} if it does not exist.
  */
global func RoundManager()
{
	var manager = FindObject(Find_ID(Environment_RoundManager));
	
	return manager;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// finished functions

static const ROUND_Number_Default = -1;

static const ROUND_Callback_OnRoundReset = "OnRoundReset";
static const ROUND_Callback_OnRoundStart = "OnRoundStart";
static const ROUND_Callback_OnRoundEnd = "OnRoundEnd";


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

//---------- engine calls ----------//

func Initialize()
{
	round_counter = 0; // on purpose
	round_has_started = false;
	
	cycle_chooser = false;
	chooser_id = nil;
	chooser_player = 0;
	max_rounds = ROUND_Number_Default;
	
	round_end_blocker = CreateArray();
	round_start_blocker = CreateArray();
	
	ScheduleCall(this, this.NextRound, 5, 0);
}

/**
 Displays a message window with the description of the object.
 @par player_index The player who selected the object in the rules menu.
 @return bool {@c true}.
  */
public func Activate(int player_index)
{
	MessageWindow(this.Description, player_index);
	return true;
}

//---------- interaction with other objects ----------//

/**
 Recognizes an object, so that the round does not start until
 this object tells the round manager that it is ready.
 @note The object has to tell the round manager that it is ready
       by calling {@link Environment_RoundManager#RemoveRoundStartBlocker}.
 @par blocker The object.
 @related {@link Environment_RoundManager#RegisterRoundEndBlocker}
  */
public func RegisterRoundStartBlocker(object blocker)
{
	CheckBlocker(blocker);

	if (!IsValueInArray(round_start_blocker, blocker))
	{
		PushBack(round_start_blocker, blocker);
	}
}

/**
 Tells the round manager, that the object does not prevent the
 round from starting anymore. The round starts if no object blocks
 the round manager.
 @par blocker The object.
 @related {@link Environment_RoundManager#RegisterRoundStartBlocker}, {@link Environment_RoundManager#RemoveRoundEndBlocker}.
  */
public func RemoveRoundStartBlocker(object blocker)
{
	CheckBlocker(blocker);

	RemoveArrayValue(round_start_blocker, blocker, false);
	
	PrepareRoundStart();
}

/**
 Recognizes an object, so that the round does not end until
 this object tells the round manager that it is ready.
 @note The object has to tell the round manager that it is ready
       by calling {@link Environment_RoundManager#RemoveRoundEndBlocker}.
 @par blocker The object.
 @related {@link Environment_RoundManager#RegisterRoundStartBlocker}
  */
public func RegisterRoundEndBlocker(object blocker)
{
	CheckBlocker(blocker);

	if (!IsValueInArray(round_end_blocker, blocker))
	{
		PushBack(round_end_blocker, blocker);
	}
}

/**
 Tells the round manager, that the object does not prevent the
 round from ending anymore. The round ends if no object blocks
 the round manager.
 @par blocker The object.
 @related {@link Environment_RoundManager#RegisterRoundEndBlocker}, {@link Environment_RoundManager#RemoveRoundStartBlocker}.
  */
public func RemoveRoundEndBlocker(object blocker)
{
	CheckBlocker(blocker);

	RemoveArrayValue(round_end_blocker, blocker, false);
	
	PrepareRoundEnd();
}

/**
 Use this to check if a round in the game is currently active.
 @return {@c true} if the round has started, including the game call "OnRoundStart",@br
         {@c false} if the round has ended, including the game call "OnRoundend"
  */
public func IsRoundActive()
{
	return round_has_started;
}

//---------- rection to game calls ----------//

/**
 GameCall when the round is reset. 
  */
func OnRoundReset(int round_number)
{
	if (chooser_id != nil)
	{
		// has a chooser? Configure the settings!
		if (cycle_chooser)
		{
			chooser_player = (round_counter - 1) % GetPlayerCount(C4PT_User);
		}
	}
	
	ScheduleCall(this, this.PrepareRoundStart, 5, 0);
}


//---------- internal calls ----------//

/**
 Checks if the object actually exists and throws an error if not.
 @par blocker The blocker object.
 @ignore
  */
func CheckBlocker(object blocker)
{
	if (blocker == nil)
	{
		FatalError(Format("Must specify an existing object! Parameter is %v", blocker));
	}
}

/**
 Prepares a new round.
 Issues the game call {@c OnRoundEnd(int round_number)}, increases the round counter and calls {@c OnRoundReset(int round_number)}.
  */
func NextRound()
{
	// increase round number
	round_counter++;
	
	GameCallEx(ROUND_Callback_OnRoundReset, round_counter);
}

/**
 Gets called every time a round start blocker is removed.
  */
func PrepareRoundStart()
{
	if (GetLength(round_start_blocker) == 0)
	{
		DoRoundStart();
	}
}

/**
 Gets called every time a round end blocker is removed.
  */
func PrepareRoundEnd()
{
	if (GetLength(round_end_blocker) == 0)
	{
		DoRoundEnd();
		NextRound();
	}
}

/**
 Issues the game call {@c GameCallEx("OnRoundEnd", round_number)}.
  */
func DoRoundEnd()
{
	if (round_has_started)
	{
		// reset the activity status
		round_has_started = false;
		GameCallEx(ROUND_Callback_OnRoundEnd, round_counter);
	}
}

/**
 Issues the game call {@c GameCallEx("OnRoundStart", round_number)}.
  */
func DoRoundStart()
{
	if (!round_has_started)
	{
		round_has_started = true;
		GameCallEx(ROUND_Callback_OnRoundStart, round_counter);
		
		ScheduleCall(this, this.PrepareRoundEnd, 5, 0);
	}
}

/**
 Gives the current round number.
 @return int The number of the current round. It starts counting at 1 and increases before {@c GameCallEx("OnRoundReset")} is called.
  */
public func GetRoundNumber()
{
	return round_counter;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// non-functional and temporary stuff
