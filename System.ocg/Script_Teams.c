
// Schaut, wie viele Spieler in einem Team sind
global func GetTeamPlayerCount(int team)
{
	for(var i = GetPlayerCount() - 1, j; i > -1; i--)
		if(GetPlayerTeam(GetPlayerByIndex(i)) == team)
			j++;
	return j;
}

// Liefert nacheinander die einzelnen Spieler eines Teams (Parameter 1 - x)
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