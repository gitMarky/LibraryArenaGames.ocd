#include Library_TeamGoal

local Name = "$Name$";

public func GetDefaultWinScore()
{
	return 10;
}

public func RelaunchPlayer(int victim, int killer)
{
	_inherited(victim, killer, ...);

	if (victim != killer)
	{
		DoScore(GetFactionByPlayer(killer), +1);
	}

	return;
}
