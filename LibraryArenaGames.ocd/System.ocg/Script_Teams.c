
/**
	Gets the number of players in a team.

	@par team The team index.
	@return The number of players that are in the team.

	@author Unknown
 */
global func GetTeamPlayerCount(int team)
{
	for (var i = GetPlayerCount() - 1, j; i > -1; --i)
		if (GetPlayerTeam(GetPlayerByIndex(i)) == team)
			j++;
	return j;
}


/**
	Gets the n-th player in a team.

	@par team The team index.
	@par player The team member, starting from 0, of that team.
	@return int The engine player index of the team member.

	@author Unknown
 */
global func GetTeamPlayer(int team, int player_index)
{	
	for (var i = 0; i < GetPlayerCount(); ++i)
	{
		var index = GetPlayerByIndex(i);
		if (GetPlayerTeam(index) == team && player_index-- <= 0)
		{
			return index;
		}
	}
	return -1;
}


/**
	Gets the teams. In a game with manually assigned
	teams there will be one team for each player
	initially, then once the players are assigned
	you have a few teams and then the empty teams...

	@return array An array with the numbers of the
	              non-empty teams.
 */
global func GetActiveTeams()
{
	var active_teams = [];
	for (var i; i < GetPlayerCount(); ++i)
	{
		var player = GetPlayerByIndex(i);
		var team = GetPlayerTeam(player);
		PushBack(active_teams, team);
	}
	RemoveDuplicates(active_teams);
	SortArray(active_teams);
	return active_teams;
}
