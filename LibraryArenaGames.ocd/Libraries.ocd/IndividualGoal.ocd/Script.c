/**
 Library for a goal that is winnable by individual players.
 A faction in {@link Library_ConfigurableRule#index} corresponds to a player.
 @author Marky
 @version 0.1.0
 */

#include Library_ConfigurableGoal

local Name = "$Name$";

protected func Initialize()
{
	_inherited(...);
	
	var player = 0;
	for (var i = 0; i < GetPlayerCount(); i++)
	{
		var current = GetPlayerByIndex(i);
		
		if (current > player)
		{
			player = current;
		}
	}
	
	EnsureArraySize(player);
}

protected func InitializePlayer(int player)
{
	_inherited(player, ...);
	
	EnsureArraySize(player);
}

private func EnsureArraySize(int player)
{
	for (var i = 0; i <= player && GetLength(score_list_points) <= player; i++)
	{
		PushBack(score_list_points, 0);
		PushBack(score_list_rounds, 0);
	}
}

private func GetFactionColor(int player)
{
	return GetPlayerColor(player);
}
