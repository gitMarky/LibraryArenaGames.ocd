#include Library_Goal_ForPlayer

local Name = "$Name$";

public func GetDefaultWinScore()
{
	return 5;
}

func DoSetup(int round_number)
{
	for (var i = 0; i < GetPlayerCount(); i++)
	{
		var player = GetPlayerByIndex(i);

		SetScore(player, GetWinScore() - 1);
	}
}

func RelaunchPlayer(int victim, int killer)
{
	_inherited(victim, killer, ...);

	DoScore(victim, -1, true);

	if (IsPlayerEliminated(victim))
	{
		// TODO: stop relaunching, or whatever
	}

	DoWinCheck();

	return;
}

func DoWinCheck()
{
	var winner;
	var candidates = 0;

	for (var i = 0; i < GetPlayerCount(); i++)
	{
		var player = GetPlayerByIndex(i);

		if (!IsPlayerEliminated(player))
		{
			candidates++;
			winner = player;
		}
	}

	if (candidates == 1)
	{
		SetScore(winner, GetWinScore());
	}
	else if (candidates < 1)
	{
		FatalError("This situation should not occurr... at least I hoped so");
	}
}

func IsPlayerEliminated(int player)
{
	return (GetScore(player) < 0);
}
