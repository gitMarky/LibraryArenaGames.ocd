/**
	Faction representation: A faction corresponds to a team.

	@author Marky
 */


/* --- Global logic --- */

/**
	Definition call: Counts the number of factions.

	@return int the amount of teams. Returns only the
	            amount of active teams if the teams
	            are auto-generated. 
 */
func Count()
{
	AssertDefinitionContext();

	if (GetTeamConfig(TEAM_AutoGenerateTeams))
	{
		return GetLength(GetActiveTeams());
	}
	else
	{
		return GetTeamCount();
	}
}


/**
	Definition call: Gets a faction identifier by its index.

	@par index the index.

	@return int the team number, by index. Only the active
	            teams are evaluated if teams are auto-generated. 
 */
func GetIdentifierByIndex(int index)
{
	AssertDefinitionContext();

	if (GetTeamConfig(TEAM_AutoGenerateTeams))
	{
		return GetActiveTeams()[index];
	}
	else
	{
		return GetTeamByIndex(index);
	}
}


/**
	Definition call: Gets a faction identifier by player number.

	@par player the player number, must point to a valid player.

	@return int the team number, by player number.
 */
func GetIdentifierByPlayer(int player)
{
	AssertDefinitionContext();

	return GetPlayerTeam(player);
}

/* --- Individual logic --- */

local FactionID;

/**
	Gets the color of the faction.

	@return int the team color. Auto-generated teams with only one player
	            return the player color.
 */
func GetColor()
{
	if (GetTeamConfig(TEAM_AutoGenerateTeams) && GetTeamPlayerCount(FactionID) == 1)
	{
		return GetPlayerColor(GetTeamPlayer(FactionID, 0));
	}
	else
	{
		return GetTeamColor(FactionID);
	}
}

/**
	Gets the name of the faction.

	@return String the team name.
 */
func GetName()
{
	return GetTeamName(FactionID);
}
