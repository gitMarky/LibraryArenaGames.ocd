/**
	Faction representation: A faction in {@link Library_Goal_Configurable#index} corresponds to a player.

	@author Marky
 */

func GetFactionCount()
{
	return GetPlayerCount();
}

func GetFactionByIndex(int index)
{
	return GetFactionByPlayer(GetPlayerByIndex(index));
}

func GetFactionByPlayer(int player)
{
	return GetPlayerID(player);
}

func GetFactionColor(int player)
{
	return GetPlayerColor(player);
}

func GetFactionName(int player)
{
	return GetPlayerName(player);
}
