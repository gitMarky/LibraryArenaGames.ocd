/**
	Plugin for scenario script.

	Players start at random locations.

	@author Marky
*/


/* --- Internals --- */

/**
	Chooses a relaunch location for a player.

	@par player The player number.
 */
func GetRelaunchLocation(int player)
{
	// sort the possible locations
	var possible_locations = [];

	var team_nr;
	if (GetTeamCount() > 1)
	{
		team_nr = GetPlayerTeam(player);
	}
	else
	{
		team_nr = player;
	}

	for (var location in RelaunchLocations())
	{
		var team = location->GetTeam();
		if (team == nil      // Free for all teams
		 || team == NO_OWNER // As above, but compatibility implementation
		 || team == team_nr) // Available for the specific team number
		{
			PushBack(possible_locations, location);
		}
	}

	// determine a random location
	return possible_locations[Random(GetLength(possible_locations))];
}


/* --- Overloadable callbacks --- */

/**
	Defines relaunch locations where the players can launch.

	@return array An array of proplists. Each proplist has the following
	              attributes:<br>
	              {@c x}: The x position.<br>
	              {@c y}: The y position.<br>
	              {@c team}: Defines the location as exclusive for a team or player.<br>
	                         If there is more than one team, this reserves the location
	                         for a specific team.<br>
	                         Otherwise, this defines a specific player number.<br>
	                         A value of {@c NO_OWNER} makes the location accessible for
	                         all players.
 */
public func RelaunchLocations()
{
	return [{ x = LandscapeWidth() / 2, y = 20, team = NO_OWNER}];
}
