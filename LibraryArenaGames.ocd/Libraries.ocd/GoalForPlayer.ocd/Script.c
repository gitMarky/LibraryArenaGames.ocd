/**
	Library for a goal that is winnable by individual players.
	
	A faction in {@link Library_Goal_Configurable#index} corresponds to a player.
	
	@author Marky
 */

#include Library_Goal_Configurable


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
	return GetFactionByPlayer(GetPlayerByIndex(index));
}

func GetFactionByPlayer(int player)
{
	return GetPlayerID(player);
}

func GetFactionColor(int player)
{
	return GetPlayerColor(player);
}

func GetFactionName(int player)
{
	return GetPlayerName(player);
}

func DoWinRound(array factions)
{
	for (var faction in factions)
	{
		DoRoundScore(faction, 1);
	}
	_inherited(factions);
}
