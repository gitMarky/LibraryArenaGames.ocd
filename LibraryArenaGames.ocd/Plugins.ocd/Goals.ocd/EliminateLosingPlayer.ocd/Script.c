/**
	Plugin for goal script.

	The goal lasts eliminates players that lost the round.

	@author Marky
*/


/**
	Marks players for elimination at round end if they are not winning.

	@par factions An array of factions that won the round.
 */
public func DoWinRound(array winning_factions)
{
	for (var i = 0; i < GetPlayerCount(); ++i)
	{
		var player = GetPlayerByIndex(i);
		var faction = this->GetFactionByPlayer(player);

		Arena_PlayerManager->GetInstance()->MarkForElimination(player, !IsValueInArray(winning_factions, faction));
	}
	_inherited(winning_factions);
}
