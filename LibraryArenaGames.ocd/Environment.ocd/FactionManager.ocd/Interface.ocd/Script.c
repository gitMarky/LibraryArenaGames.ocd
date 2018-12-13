/**
	Faction interface definition.

	@author Marky
 */

static const ERROR_INTERFACE_DEFINITION = "Implement this function by overloading it. This is just an interface definition";

/* --- Global logic --- */

/**
	Definition call: Counts the number of factions.

	@return int the amount of factions.
 */
func Count()
{
	FatalError(ERROR_INTERFACE_DEFINITION);
}


/**
	Definition call: Gets a faction identifier by its index.

	@par index the index.

	@return int the faction identifier, by index.
 */
func GetIdentifierByIndex(int index)
{
	FatalError(ERROR_INTERFACE_DEFINITION);
}


/**
	Definition call: Gets a faction identifier by player number.

	@par player the player number, must point to a valid player.

	@return int the faction identifier, by player number.
 */
func GetIdentifierByPlayer(int player)
{
	FatalError(ERROR_INTERFACE_DEFINITION);
}

/* --- Individual logic --- */

local FactionID;

/**
	Gets the ID of the faction.
	
	@return int an individual identifier for the faction.
 */
func GetID()
{
	return FactionID;
}

/**
	Gets the color of the faction.

	@return int the faction color.
 */
func GetColor()
{
	FatalError(ERROR_INTERFACE_DEFINITION);
}

/**
	Gets the name of the faction.

	@return String the faction name.
 */
func GetName()
{
	FatalError(ERROR_INTERFACE_DEFINITION);
}
