/**
	Player manager
	
	Helps managing player statistics. Internal values are saved by player ID

	@author Marky
 */

#include Library_Singleton

/* --- Properties --- */
 
local Name = "$Name$";
local Description = "$Description$";

local Singleton_GetterCreatesInstance = true;

local player_elimination;
local player_stats;

/* --- Engine callbacks --- */

func Construction (object creator)
{
	_inherited(creator, ...);
	
	player_elimination = [];
	player_stats = [];
}

func OnGameOver()
{
	for (var index = 0; index < GetLength(player_elimination); ++index)
	{
		if (player_elimination[index])
		{
			var player = GetPlayerByID(index);
			if (player != NO_OWNER)
			{
				EliminatePlayer(player);
			}
		}
	}
}

/* --- Interface --- */

/**
	Marks a player for elimination in the {@link Scenario#OnGameOver} call.
	
	@par player The player number. Is saved by player ID internally.
	@par eliminate Default is {@code true}. Pass {@code false} if you want to
	               revoke the setting.
	               
	@return object The instance itself for further function calls.
 */
public func MarkForElimination(int player, bool eliminate)
{
	player_elimination[GetPlayerID(player)] = eliminate ?? true;
	return this;
}


/**
	Defines a statistic for a player.
	
	@par player The player number. Is saved by player ID internally.
	@par stat The statistics name.
	               
	@return object The instance itself for further function calls.
 */
public func SetPlayerStat(int player, string stat, value)
{
	AssertNotNil(stat);
	
	var index = GetPlayerID(player);
	if (!player_stats[index])
	{
		player_stats[index] = {};
	}
	
	if (player_stats[index][stat])
	{
		if (value == nil)
		{
			player_stats[index][stat] = nil;
		}
		else if (player_stats[index][stat].Type == GetType(value))
		{
			player_stats[index][stat].Value = value;
		}
		else
		{
			FatalError(Format("Type mismatch: Trying to set %v, expected %v", GetType(value), player_stats[index][stat].Type));
		}
	}
	else
	{
		player_stats[index][stat] = { Type = GetType(value), Value = value};
	}
	return this;
}


/**
	Reads a statistic for a player.
	
	@par player The player number. Is saved by player ID internally.
	               
	@return The value.
 */
public func GetPlayerStat(int player, string stat)
{
	AssertNotNil(stat);
	
	var index = GetPlayerID(player);
	if (!player_stats[index])
	{
		player_stats[index] = {};
	}
	
	if (player_stats[index][stat])
	{
		return player_stats[index][stat].Value;
	}
	else
	{
		return nil;
	}
}
