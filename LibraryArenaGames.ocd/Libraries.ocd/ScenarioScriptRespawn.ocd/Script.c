/**
	Helper for default scenario script.

	@author Marky
*/

/* --- Callbacks from other objects --- */

/**
	Callback from configuration menu object.
 */
func OnConfigurationEnd()
{
	_inherited(...);

	// The players have been released from their containers by
	// the round countdown. Put them in relaunch containers
	// at the starting position
	for (var i = 0; i < GetPlayerCount(); i++)
	{
		SpawnPlayer(GetPlayerByIndex(i));
	}
}

/**
	Callback from the round manager.
 */
func OnRoundStart(int round)
{
	_inherited(round, ...);

    // Release all players from their relaunch containers, with waiting period.
	ReleasePlayers(false);
}

/**
	Callback from the thing that relaunches the players.
 */
func RelaunchPlayer(int player, int killer)
{
	_inherited(player, killer, ...);

	var crew = SpawnPlayer(player);
	ReleaseCrew(crew);
}

/* --- Interface --- */

/**
	Releases all players from their relaunch containers.

	@par instant If {@c true}, then the relaunch container exits the player immediately.
 */
public func ReleasePlayers(bool instant)
{
	for (var i = 0; i < GetPlayerCount(); i++)
	{
		ReleasePlayer(GetPlayerByIndex(i), instant);
	}
}


/**
	Releases a single player from his relaunch container.

	@par instant If {@c true}, then the relaunch container exits the player immediately.
 */
public func ReleasePlayer(int player, bool instant)
{
	for (var i = 0; i < GetCrewCount(player); i++)
	{
		ReleaseCrew(GetCrew(player, i), instant);
	}
}


/**
	Releases the crew member from their relaunch container.

	@par instant If {@c true}, then the relaunch container exits the player immediately.
 */
public func ReleaseCrew(object crew, bool instant)
{
	var container = crew->Contained();
	
	if ((container != nil) && (container->GetID() == Arena_RelaunchContainer))
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

/* --- Internals --- */

/**
	Puts a player in a relaunch container, without releasing him.
 */
func SpawnPlayer(int player)
{
	var crew = SpawnPlayerCrew(player);
	var relaunch_location = this->~GetRelaunchLocation(player);
    ContainPlayer(relaunch_location, crew);
    return crew;
}


/**
	Spawns one crew member for the player.

	@par player The player number.

	@return object The spawned crew member. Returns {@link Global#GetHiRank}
                   if the player already has a crew.
 */
func SpawnPlayerCrew(int player)
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
	Contains the player crew in a {@link Arena_RelaunchContainer}.

	@par relaunch_location A location definition where the player should relaunch.
	@par crew The crew member that should be contained.
 */
private func ContainPlayer(proplist relaunch_location, object crew)
{
    // From existing one?
	var relaunch_container = crew->Contained();
	
	// Delete foreign invalid containers...
	if (relaunch_container != nil && relaunch_container->GetID() != Arena_RelaunchContainer)
	{
		relaunch_container->RemoveObject(true);
	}
	
	// Get the position
	var x, y;
	if (relaunch_location)
	{
		x = relaunch_location->GetX();
		y = relaunch_location->GetY();
	}
	else
	{
		x = LandscapeWidth() / 2;
		y = LandscapeHeight() / 2;
	}

	// Create the container
	relaunch_container = relaunch_container ?? CreateObject(Arena_RelaunchContainer, 0, 0, crew->GetOwner());
    relaunch_container->SetPosition(x, y);
	relaunch_container->ContainCrew(crew);
}


/* --- Overloadable callbacks --- */

/**
	Defines which crew will be spawned for the player.

	@par player The player number.

	@return id The type of crew member that will be created.
               Defaults to {@c Clonk}.
 */
public func GetPlayerCrewID(int player)
{
	return Clonk;
}
