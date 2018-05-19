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

/* --- Engine callbacks --- */

func Construction (object creator)
{
	_inherited(creator, ...);
	
	player_elimination = [];
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
 */
public func MarkForElimination(int player, bool eliminate)
{
	player_elimination[GetPlayerID(player)] = eliminate ?? true;
	return this;
}
