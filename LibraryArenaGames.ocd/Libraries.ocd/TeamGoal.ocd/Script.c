/**
 Library for a goal that is winnable by teams.
 A faction in {@link Library_ConfigurableRule#index} corresponds to a team.
 @author Marky
 @version 0.1.0
 */

#include Library_ConfigurableGoal

public func IsTeamGoal()
{
	return true;
}

public func GetDescription(int plr)
{
	var flags = GetWinScore() - score_list_points[GetPlayerTeam(plr)];
	if (IsFulfilled())
		return "$MsgGoalFulfilled$";
	else
	{
		var msg = "$MsgGoalCaptureTheFlag$";
		if (flags == 1)
			return Format("%s %s", msg, "$MsgGoalUnfulfilled1$");
		else
			return Format("%s %s", msg, Format("$MsgGoalUnfulfilledX$", flags));
	}
}


public func GetShortDescription(int plr)
{
	var team = GetPlayerTeam(plr);
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

public func Activate(int byplr)
{
	var msg = GetDescription(byplr);
	
	MessageWindow(msg, byplr);
	return;
}

/*
private func EliminateOthers(int win_team)
{
	for (var i = 0; i < GetPlayerCount(); i++)
	{
		var plr = GetPlayerByIndex(i);
		var team = GetPlayerTeam(plr);
		if (team != win_team)
			EliminatePlayer(plr);	
	}
	return;
}
*/

// Returns the number of players in a specific team.
private func GetPlayerInTeamCount(int team)
{
	var cnt = 0;
	for (var i = 0; i < GetPlayerCount(); i++)
		if (GetPlayerTeam(GetPlayerByIndex(i)) == team)
			cnt++;
	return cnt;
}

// 2 teams per default
public func GetFactionCount()
{
	return 2;
}

private func GetFactionColor(int team)
{
	return GetTeamColor(team);
}

local Name = "$Name$";
