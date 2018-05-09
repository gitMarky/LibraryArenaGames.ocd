/**
 This is a rule object that enables the game to run in turns.
 {@section Script Architecture}
 There are phases and game calls for turns:@br
 1.) A game call is issued: {@c GameCallEx("OnTurnReset", turn_number)}.
     Objects should create a default state for a new turn in this call, for example
     players should be prevented from acting by putting them in spawn points.
     Use {@link Environment_TurnManager#RegisterTurnStartBlocker} here if you want to prevent the turn from
     starting immediately.@br
 2.) Configuration Phase. This is completed once all turn start blockers have removed themselves
     with {@link Environment_TurnManager#RemoveTurnStartBlocker}.@br
 3.) A game call is issued: {@c GameCallEx("OnTurnStart", turn_number)}.
     This is the point where interactive objects should be enabled, for example spawn points
     can spawn the players at this point.@br
     Use {@link Environment_TurnManager#RegisterTurnEndBlocker} here if you want to prevent the turn from
     ending immediately.@br
 4.) Game phase. This is completed once all turn end blockers have removed themselves
     with {@link Environment_TurnManager#RemoveTurnEndBlocker}.@br
 5.) A game call is issued: {@c GameCallEx("OnTurnEnd", turn_number)}. Remove undesired objects now.@br
 6.) Repeat from 1.)@br
 @br
 Note that a single {@link global#TurnManager TurnManager()} object will do one thing: Rush through the turns quickly.
 It actually needs rules or goals that register as blockers for the phases.
 {@section Constants}
 The object offers new constants:
 <table>
 	<tr><th>Name</th>                     <th>Value</th> <th>Description</th></tr>
 	<tr><td>TURN_Number_Default</td> <td>-1</td> <td>Number of turns that the game will run. The default value is an infinite number of turns. Overload for a finite number of turns.</td></tr>
 </table>
 
 
 @title Turns
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
 Use this to access the turn manager object from other objects.
 @return object A {@link Environment_TurnManager#index turn manager} object,
         or {@c nil} if it does not exist.
  */
global func TurnManager()
{
	var manager = FindObject(Find_ID(Environment_TurnManager));
	
	return manager;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// finished functions

static const TURN_Number_Default = -1;

static const TURN_Callback_OnTurnReset = "OnTurnReset";
static const TURN_Callback_OnTurnStart = "OnTurnStart";
static const TURN_Callback_OnTurnEnd = "OnTurnEnd";

// locals: internal information

local turn_counter;	      // int: the number of the current turn. Starts at 1.
local turn_has_started;   // bool: true - the turn has started, enables countdown
                          //       false - the turn is inactive, for example when a chooser is running etc.
local turn_end_blocker;   // array: the turn cannot be finished until all of these objects have been removed
local turn_start_blocker; // dito

// locals: configuration

local max_turns;		// int:  the game automatically stops after this many turns

//---------- engine calls ----------//

protected func Initialize()
{
	turn_counter = 0; // on purpose
	turn_has_started = false;
	
	max_turns = TURN_Number_Default;
	
	turn_end_blocker = CreateArray();
	turn_start_blocker = CreateArray();
	
	ScheduleCall(this, this.NextTurn, 5, 0);
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
 Recognizes an object, so that the turn does not start until
 this object tells the turn manager that it is ready.
 @note The object has to tell the turn manager that it is ready
       by calling {@link Environment_TurnManager#RemoveTurnStartBlocker}.
 @par blocker The object.
 @related {@link Environment_TurnManager#RegisterTurnEndBlocker}
  */
public func RegisterTurnStartBlocker(object blocker)
{
	CheckBlocker(blocker);

	if (!IsValueInArray(turn_start_blocker, blocker))
	{
		PushBack(turn_start_blocker, blocker);
	}
}

/**
 Tells the turn manager, that the object does not prevent the
 turn from starting anymore. The turn starts if no object blocks
 the turn manager.
 @par blocker The object.
 @related {@link Environment_TurnManager#RegisterTurnStartBlocker}, {@link Environment_TurnManager#RemoveTurnEndBlocker}.
  */
public func RemoveTurnStartBlocker(object blocker)
{
	CheckBlocker(blocker);

	RemoveArrayValue(turn_start_blocker, blocker, false);
	
	PrepareTurnStart();
}

/**
 Recognizes an object, so that the turn does not end until
 this object tells the turn manager that it is ready.
 @note The object has to tell the turn manager that it is ready
       by calling {@link Environment_TurnManager#RemoveTurnEndBlocker}.
 @par blocker The object.
 @related {@link Environment_TurnManager#RegisterTurnStartBlocker}
  */
public func RegisterTurnEndBlocker(object blocker)
{
	CheckBlocker(blocker);

	if (!IsValueInArray(turn_end_blocker, blocker))
	{
		PushBack(turn_end_blocker, blocker);
	}
}

/**
 Tells the turn manager, that the object does not prevent the
 turn from ending anymore. The turn ends if no object blocks
 the turn manager.
 @par blocker The object.
 @related {@link Environment_TurnManager#RegisterTurnEndBlocker}, {@link Environment_TurnManager#RemoveTurnStartBlocker}.
  */
public func RemoveTurnEndBlocker(object blocker)
{
	CheckBlocker(blocker);

	RemoveArrayValue(turn_end_blocker, blocker, false);
	
	PrepareTurnEnd();
}

/**
 Use this to check if a turn in the game is currently active.
 @return {@c true} if the turn has started, including the game call "OnTurnStart",@br
         {@c false} if the turn has ended, including the game call "OnTurnend"
  */
public func IsTurnActive()
{
	return turn_has_started;
}

//---------- rection to game calls ----------//

/**
 GameCall when the turn is reset. 
  */
protected func OnTurnReset(int turn_number)
{
	ScheduleCall(this, this.PrepareTurnStart, 5, 0);
}


//---------- internal calls ----------//

/**
 Checks if the object actually exists and throws an error if not.
 @par blocker The blocker object.
 @ignore
  */
private func CheckBlocker(object blocker)
{
	if (blocker == nil)
	{
		FatalError(Format("Must specify an existing object! Parameter is %v", blocker));
	}
}

/**
 Prepares a new turn.
 Issues the game call {@c OnTurnEnd(int turn_number)}, increases the turn counter and calls {@c OnTurnReset(int turn_number)}.
  */
private func NextTurn()
{
	// increase turn number
	turn_counter++;

	GameCallEx(TURN_Callback_OnTurnReset, turn_counter);
}

/**
 Gets called every time a turn start blocker is removed.
  */
private func PrepareTurnStart()
{
	if (GetLength(turn_start_blocker) == 0)
	{
		DoTurnStart();
	}
}

/**
 Gets called every time a turn end blocker is removed.
  */
private func PrepareTurnEnd()
{
	if (GetLength(turn_end_blocker) == 0)
	{
		DoTurnEnd();
		NextTurn();
	}
}

/**
 Issues the game call {@c GameCallEx("OnTurnEnd", turn_number)}.
  */
private func DoTurnEnd()
{
	// reset the activity status
	if (turn_has_started)
	{
		turn_has_started = false;
		GameCallEx(TURN_Callback_OnTurnEnd, turn_counter);
	}
}

/**
 Issues the game call {@c GameCallEx("OnTurnStart", turn_number)}.
  */
private func DoTurnStart()
{
	if (!turn_has_started)
	{
		turn_has_started = true;
		GameCallEx(TURN_Callback_OnTurnStart, turn_counter);
		
		ScheduleCall(this, this.PrepareTurnEnd, 5, 0);
	}
}

/**
 Gives the current turn number.
 @return int The number of the current turn. It starts counting at 1 and increases before {@c GameCallEx("OnTurnReset"")} is called.
  */
public func GetTurnNumber()
{
	return turn_counter;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// non-functional and temporary stuff
