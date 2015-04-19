
/**
 Gets the number of players in a team.
 @par team The team index.
 @return The number of players that are in the team.
 @author Unknown
 @version 0.1.0
 */
global func GetTeamPlayerCount(int team)
{
	for(var i = GetPlayerCount() - 1, j; i > -1; i--)
		if(GetPlayerTeam(GetPlayerByIndex(i)) == team)
			j++;
	return j;
}

/**
 @par team The team index.
 @par player The team member, starting from 0, of that team.
 @return int The engine player index of the team member.
 @author Unknown
 @version 0.1.0
 */
global func GetTeamPlayer(int team, int player)
{	
	for (var i = 0; i < GetPlayerCount(); i++)
	{
		var index = GetPlayerByIndex(i);
		if (GetPlayerTeam(index) == team && player-- <= 0)
		{
			return index;
		}
	}
	return -1;
}