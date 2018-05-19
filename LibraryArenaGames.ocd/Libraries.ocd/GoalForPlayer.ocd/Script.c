/**
 Library for a goal that is winnable by individual players.
 A faction in {@link Library_ConfigurableRule#index} corresponds to a player.
 @author Marky
  */

#include Library_Goal_Configurable

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Engine callbacks

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Goal description texts

public func GetDescription(int player)
{
	return GetGoalDescription(player);
}

public func GetShortDescription(int player)
{
	var score = GetFactionScore(player);
	var target = GetWinScore();

	return Format("%d / %d", score, target);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Overloaded from configurable goal

public func GetFactionCount()
{
	return GetPlayerCount();
}

public func GetFactionByIndex(int index)
{
	return GetPlayerByIndex(index);
}

public func GetFactionByPlayer(int player)
{
	return player;
}

public func GetFactionColor(int player)
{
	return GetPlayerColor(player);
}

public func GetFactionName(int player)
{
	return GetPlayerName(player);
}

public func DoWinRound(int faction)
{
	DoRoundScore(faction, 1);
	_inherited(faction);
}
