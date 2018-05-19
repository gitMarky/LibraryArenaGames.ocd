#include Library_IndividualGoal

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
		DoScore(killer, +1);
	}
}
