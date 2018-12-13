/**
	Faction manager

	Helps managing factions, such as players or teams.

	@author Marky
 */

#include Library_Singleton

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";

local Singleton_GetterCreatesInstance = true;

local factions;
local type;

/* --- Engine callbacks --- */

func Construction (object creator)
{
	_inherited(creator, ...);

	factions = [];
}

func InitializePlayer (int player, int x, int y, object base, int team, id extra_data)
{
	// Called after when a new player joins the game. 
}

func RemovePlayer (int player, int team)
{
	// Called when a player is removed from the game.
}

func OnTeamSwitch (int player, int new_team, int old_team)
{
	// Called after the team of a player has been changed.
}


/* --- Interface --- */

/**
	Gets the amount of factions that are currently active.

	@note requires that the type was set with {@link #SetType} initially.

	@return int the number of factions.
 */
func GetFactionCount()
{
	AssertHasType();
	return type->Count();
}

/**
	Gets a faction by its identifier.

	@note requires that the type was set with {@link #SetType} initially.

	@return proplist the faction representation, as a proplist.
	                 If the identifier is invalid, this will nonetheless 
	                 return a proplist.
 */
func GetFaction(int faction_id)
{
	AssertHasType();
	if (factions[faction_id])
	{
		return factions[faction_id];
	}
	else
	{
		var faction = new type { FactionID = faction_id };
		factions[faction_id] = faction;
		return faction;
	}
}


/**
	Gets a faction by its index.

	@note requires that the type was set with {@link #SetType} initially.

	@par index the index, must be inside [0; {@link #GetFactionCount}].

	@return proplist the faction representation, as a proplist.
	                 If the index is invalid, this will nonetheless 
	                 return a proplist.
 */
func GetFactionByIndex(int index)
{
	AssertHasType();
	var last_index = GetFactionCount() - 1;
	if (0 > index || index > last_index)
	{
		FatalError(Format("Faction index must be inside [0;%d], got %d", last_index, index));
	}
	return GetFaction(type->GetIdentifierByIndex(index));
}


/**
	Gets a faction by player number.

	@note requires that the type was set with {@link #SetType} initially.

	@par player the player number, must point to a valid player.

	@return proplist the faction representation, as a proplist.
	                 If the player number is invalid, this will nonetheless 
	                 return a proplist.
 */
func GetFactionByPlayer(int player)
{
	AssertHasType();
	return GetFaction(type->GetIdentifierByPlayer(player));
}


/* --- Internal --- */

func AssertHasType()
{
	if (type == nil)
	{
		FatalError(Format("Faction type was not set. Make sure to call %i->SetType(...) in the scenario", GetID()));
	}
}


/**
	Definition call: Sets the faction type.

	@par prototype Defines the faction type for this game, e.g. {@link Arena_Faction_Team}.
	               This type will be returned as a proplist by
	               <ul>
	               <li>{@link #GetFaction}</li>
	               <li>{@link #GetFactionByIndex}</li>
	               <li>{@link #GetFactionByPlayer}</li>
	               </ul> 

	               You can pass any type here, as long as it is compatible with
	               the faction interface (which is not officially defined, duh :/).
 */
func SetType(id prototype)
{
	AssertDefinitionContext();

	var instance = GetInstance();
	if (instance.type)
	{
		FatalError(Format("Faction type was already assigned to %i. You are not allowed to redefine it to %i.", instance.type, prototype));
	}
	else
	{
		instance.type = prototype;
	}
	return instance;
}
