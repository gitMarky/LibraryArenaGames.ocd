/**
	Library for a goal that is winnable by teams.

	A faction in {@link Library_Goal_Configurable#index} corresponds to a team.
	
	@author Marky
 */

#include Library_Goal_Configurable
#include Library_Faction_Team

/* --- Properties --- */

public func IsTeamGoal()
{
	return true;
}

/* --- Goal description texts --- */

public func GetDescription(int player)
{
	var faction = GetPlayerTeam(player);
	return GetGoalDescription(faction);
}


public func GetShortDescription(int player)
{
	var team = GetPlayerTeam(player);
	var score_message = "";

	// Start with own team
	if (team && team > 0)
	{
		score_message = GetFactionScoreMessage(team);
	}

	var other_team;
	for (var i = 0; i < GetTeamCount(); ++i)
	{
		other_team = GetTeamByIndex(i);
		if (other_team > 0 && other_team != team && GetPlayerInTeamCount(other_team) > 0)
		{
			score_message = Format("%s : %s", score_message, GetFactionScoreMessage(other_team));
		}
	}
	
	return score_message;
}


/* --- Overloaded from configurable goal --- */

func DoWinRound(array factions)
{
	for (var faction in factions)
	{
		DoRoundScore(faction, 1);
	}
	_inherited(factions);
}
