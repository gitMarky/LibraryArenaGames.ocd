/**
 * Helper for default scenario script.
 *
 * @author Marky
 * @version 0.3
*/


/**
 * Callback from configuration menu object.
 */
private func OnConfigurationEnd()
{
	_inherited(...);

	// the players have been released from their containers by
	// the round countdown. Put them in relaunch containers
	// at the starting position
	for (var i = 0; i < GetPlayerCount(); i++)
	{
		SpawnPlayer(GetPlayerByIndex(i));
	}
}

/**
 * Callback from the round manager.
 */
private func OnRoundStart(int round)
{
	_inherited(round, ...);

    // release all players from their relaunch containers, with waiting period.
	ReleasePlayers(false);
}

/**
 * Callback from the thing that relaunches the players.
 */
private func RelaunchPlayer(int player, int killer)
{
	_inherited(player, killer, ...);

	var crew = SpawnPlayer(player);
	ReleaseCrew(crew);
}

/**
 * Puts a player in a relaunch container, without releasing him.
 */
private func SpawnPlayer(int player)
{
	var crew = SpawnPlayerCrew(player);
	var relaunch_location = GetRelaunchLocation(player);
    ContainPlayer(relaunch_location, crew);
    CreateStartingEquipment(crew);
    return crew;
}


/**
 * Spawns one crew member for the player.
 * 
 * @par player The player number.
 *
 * @return object The spawned crew member. Returns {@link Global#GetHiRank}
 *                if the player already has a crew.
 */
private func SpawnPlayerCrew(int player)
{
    var crew = GetHiRank(player);

	if (crew == nil)
	{
		crew = CreateObject(GetPlayerCrewID(player), 0, 0, player);
		crew->MakeCrewMember(player);
	}

	SetCursor(player, crew);
	return crew;
}


/**
 * Creates starting equipment for the player.
 * 
 * @par crew Starting equipment will be created in this object.
 */
private func CreateStartingEquipment(object crew)
{
    RemoveEquipment(crew);
	StartingEquipment(crew);
}


/**
 * Removes all contents from a crew member.
 *
 * @par crew Equipment will be removed from this object.
 */
private func RemoveEquipment(object crew)
{
	// remove previous contents
	for (var contents = crew->Contents(); contents != nil; contents = crew->Contents())
	{
		contents->RemoveObject();
	}
}


/**
 * Defines which crew will be spawned for the player.
 *
 * @par player The player number.
 *
 * @return id The type of crew member that will be created.
 *            Defaults to {@c Clonk}.
 */
public func GetPlayerCrewID(int player)
{
	return Clonk;
}


/**
 * Defines the starting equipment after spawning.
 * By default this function does nothing, so
 * overload it to add starting equipment.
 * 
 * @par crew The crew member that will get starting equpment.
 */
public func StartingEquipment(object crew)
{
    // does nothing, overload this function
}


/**
 * Chooses a relaunch location for a player.
 *
 * @par player The player number.
 */
private func GetRelaunchLocation(int player)
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


/**
 * Contains the player crew in a {@link RelaunchContainerEx}.
 *
 * @par relaunch_location A location definition where the player should relaunch.
 * @par crew The crew member that should be contained.
 */
private func ContainPlayer(proplist relaunch_location, object crew)
{
    // From existing one?
	var relaunch_container = crew->Contained();
	
	// Delete foreign invalid containers...
	if (relaunch_container != nil && relaunch_container->GetID() != RelaunchContainerEx)
	{
		relaunch_container->RemoveObject(true);
	}
	
	// Get the position
	var x = relaunch_location->GetX();
	var y = relaunch_location->GetY();

	// Create the container
	relaunch_container = relaunch_container ?? CreateObject(RelaunchContainerEx, 0, 0, crew->GetOwner());
    relaunch_container->SetPosition(x, y);
	relaunch_container->PrepareRelaunch(crew);
}


/**
 * Releases all players from their relaunch containers.
 *
 * @par instant If {@c true}, then the relaunch container exits the player immediately.
 */
public func ReleasePlayers(bool instant)
{
	for (var i = 0; i < GetPlayerCount(); i++)
	{
		ReleasePlayer(GetPlayerByIndex(i), instant);
	}
}


/**
 * Releases a single player from his relaunch container.
 *
 * @par instant If {@c true}, then the relaunch container exits the player immediately.
 */
public func ReleasePlayer(int player, bool instant)
{
	for (var i = 0; i < GetCrewCount(player); i++)
	{
		ReleaseCrew(GetCrew(player, i), instant);
	}
}


/**
 * Releases the crew member from their relaunch container.
 *
 * @par instant If {@c true}, then the relaunch container exits the player immediately.
 */
public func ReleaseCrew(object crew, bool instant)
{
	var container = crew->Contained();
	
	if ((container != nil) && (container->GetID() == RelaunchContainerEx))
	{
		if (instant)
		{
			container->InstantRelaunch();
		}
		else
		{
			container->StartRelaunch(crew);
		}
	}
}


/**
 * Defines relaunch locations where the players can launch.
 * 
 * @return array An array of proplists. Each proplist has the following
 *               attributes:<br>
 *               {@c x}: The x position.<br>
 *               {@c y}: The y position.<br>
 *               {@c team}: Defines the location as exclusive for a team or player.<br>
 *                          If there is more than one team, this reserves the location
 *                          for a specific team.<br>
 *                          Otherwise, this defines a specific player number.<br>
 *                          A value of {@c NO_OWNER} makes the location accessible for
 *                          all players.
 */
public func RelaunchLocations()
{
	return [{ x = LandscapeWidth() / 2, y = 20, team = NO_OWNER}];
}
