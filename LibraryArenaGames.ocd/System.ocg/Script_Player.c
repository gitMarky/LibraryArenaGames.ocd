
/**
	Inverse function for {@link Global#GetPlayerByIndex}.

	@par player The player whose player index you want to get.
	@par player_type [optional] Specifies a player type, such as {@c C4PT_User}.

	@return The player index, for use in GetPlayerByIndex(); Returns -1 if the index is not found.
 */
global func GetPlayerIndex(int player, int player_type)
{
	for (var index = 0; index < GetPlayerCount(player_type); ++index)
	{
		if (GetPlayerByIndex(index, player_type) == player)
		{
			return index;
		}
	}

	return -1;
}
