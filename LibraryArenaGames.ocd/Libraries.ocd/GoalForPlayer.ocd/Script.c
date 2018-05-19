/**
	Library for a goal that is winnable by individual players.
	
	A faction in {@link Library_Goal_Configurable#index} corresponds to a player.
	
	@author Marky
 */

#include Library_Goal_Configurable

/* --- Engine callbacks --- */

func Initialize()
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

func InitializePlayer(int player)
{
	_inherited(player, ...);
	
	EnsureArraySize(player);
}

/* --- Goal description texts --- */

func GetDescription(int player)
{
	return GetGoalDescription(player);
}

func GetShortDescription(int player)
{
	var score = GetFactionScore(player);
	var target = GetWinScore();

	return Format("%d / %d", score, target);
}

/* --- Overloaded from configurable goal --- */

func GetFactionCount()
{
	return GetPlayerCount();
}

func GetFactionByIndex(int index)
{
	return GetPlayerByIndex(index);
}

func GetFactionByPlayer(int player)
{
	return player;
}

func GetFactionColor(int player)
{
	return GetPlayerColor(player);
}

func GetFactionName(int player)
{
	return GetPlayerName(player);
}

func DoWinRound(int faction)
{
	DoRoundScore(faction, 1);
	_inherited(faction);
}
