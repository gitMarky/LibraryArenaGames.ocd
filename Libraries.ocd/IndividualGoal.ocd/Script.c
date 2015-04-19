/**
 Library for a goal that is winnable by individual players.
 A faction in {@link Library_ConfigurableRule} corresponds to a player.
 @author Marky
 @version 0.1.0
 */

#include Library_ConfigurableGoal

local Name = "$Name$";

protected func Initialize()
{
	_inherited();
	
	var plr = 0;
	for (var i = 0; i < GetPlayerCount(); i++)
	{
		var current = GetPlayerByIndex(i);
		
		if (current > plr)
		{
			plr = current;
		}
	}
	
	EnsureArraySize(plr);
}

protected func InitializePlayer(int plr)
{
	_inherited(...);
	
	EnsureArraySize(plr);
}

private func EnsureArraySize(int plr)
{
	for (var i = 0; i <= plr && GetLength(score_list_points) <= plr; i++)
	{
		PushBack(score_list_points, 0);
		PushBack(score_list_rounds, 0);
	}
}