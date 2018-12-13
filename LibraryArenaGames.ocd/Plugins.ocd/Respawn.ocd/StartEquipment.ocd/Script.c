/**
	Plugin for respawn scenario script.

	The crew gets starting equipment.

	@author Marky
*/

/* --- Internals --- */

/**
	Puts a player in a relaunch container, without releasing him.
 */
func SpawnPlayer(int player)
{
	var crew = _inherited(player);
    CreateStartingEquipment(crew);
    return crew;
}


/**
	Creates starting equipment for the player.

	@par crew Starting equipment will be created in this object.
 */
func CreateStartingEquipment(object crew)
{
    RemoveEquipment(crew);
	StartingEquipment(crew);
}


/**
	Removes all contents from a crew member.

	@par crew Equipment will be removed from this object.
 */
func RemoveEquipment(object crew)
{
	// Remove previous contents
	for (var contents = crew->Contents(); contents != nil; contents = crew->Contents())
	{
		contents->RemoveObject();
	}
}


/* --- Overloadable callbacks --- */

/**
	Defines the starting equipment after spawning.
	By default this function does nothing, so
	overload it to add starting equipment.

	@par crew The crew member that will get starting equpment.
 */
public func StartingEquipment(object crew)
{
    // does nothing, overload this function
}
