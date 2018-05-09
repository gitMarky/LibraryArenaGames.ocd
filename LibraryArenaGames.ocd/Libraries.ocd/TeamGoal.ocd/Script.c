/**
 Library for a goal that is winnable by teams.
 A faction in {@link Library_ConfigurableRule#index} corresponds to a team.
 @author Marky
  */

#include Library_ConfigurableGoal

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Properties

public func IsTeamGoal()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Goal description texts

public func GetDescription(int player)
{
	var faction = GetPlayerTeam(player);
	return GetGoalDescription(faction);
}


public func GetShortDescription(int player)
{
	var team = GetPlayerTeam(player);
	var score_message = "";

	// start with own team
	if (team && team > 0)
		score_message = GetScoreMessage(team);

	var other_team;
	for (var i = 0; i < GetTeamCount(); ++i)
	{
		other_team = GetTeamByIndex(i);
		if (other_team > 0 && other_team != team)
			score_message = Format("%s : %s", score_message, GetScoreMessage(other_team));
	}
	
	return score_message;
}

// Returns the number of players in a specific team.
private func GetPlayerInTeamCount(int team)
{
	var amount = 0;
	for (var player = 0; player < GetPlayerCount(); player++)
		if (GetPlayerTeam(GetPlayerByIndex(player)) == team)
			amount++;
	return amount;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Overloaded from configurable goal

public func GetFactionCount()
{
	return GetTeamCount();
}

public func GetFactionByIndex(int index)
{
	return GetTeamByIndex(index);
}

public func GetFactionByPlayer(int player)
{
	return GetPlayerTeam(player);
}

public func GetFactionColor(int team)
{
	return GetTeamColor(team);
}

public func GetFactionName(int team)
{
	return GetTeamName(team);
}

public func DoWinRound(int faction)
{
	DoRoundScore(faction, 1);
	_inherited(faction);
}
