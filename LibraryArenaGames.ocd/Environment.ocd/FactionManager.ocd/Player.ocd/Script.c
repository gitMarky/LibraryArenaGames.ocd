/**
	Faction representation: A faction corresponds to a player.

	@author Marky
 */

/* --- Global logic --- */

/**
	Definition call: Counts the number of factions.

	@return int the amount of players.
 */
func Count()
{
	AssertDefinitionContext();
	return GetPlayerCount();
}

/**
	Definition call: Gets a faction identifier by its index.

	@par index the index.

	@return int the player ID, by index.
 */
func GetIdentifierByIndex(int index)
{
	AssertDefinitionContext();
	return GetIdentifierByPlayer(GetPlayerByIndex(index));
}

/**
	Definition call: Gets a faction identifier by player number.

	@par player the player number, must point to a valid player.

	@return int the player ID, by player number.
 */
func GetIdentifierByPlayer(int player)
{
	AssertDefinitionContext();
	return GetPlayerID(player);
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
	return GetPlayerColor(GetPlayerByID(FactionID));
}

/**
	Gets the name of the faction.

	@return String the team name.
 */
func GetName()
{
	return GetPlayerColor(GetPlayerByID(FactionID));
}
