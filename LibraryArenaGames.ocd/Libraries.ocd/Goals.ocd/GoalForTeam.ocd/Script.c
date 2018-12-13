/**
	Library for a goal that is winnable by teams.

	A faction in {@link Library_Goal_Configurable#index} corresponds to a team.

	@author Marky
 */

#include Library_Goal_Configurable

/* --- Properties --- */

public func IsTeamGoal()
{
	return true;
}

/* --- Goal description texts --- */

public func GetDescription(int player)
{
	return GetGoalDescription(GetFactionByPlayer(player));
}


public func GetShortDescription(int player)
{
	var team = GetFactionByPlayer(player);
	var score_message = "";

	// Start with own team
	if (team)
	{
		score_message = GetFactionScoreMessage(GetFactionByPlayer(player));
	}

	var other_team;
	for (var i = 0; i < GetFactionCount(); ++i)
	{
		other_team = GetFactionByIndex(i);
		if (other_team->GetID() != team->GetID() && GetFactionSize(other_team) > 0)
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
		DoRoundScore(GetFaction(faction), 1);
	}
	_inherited(factions);
}
