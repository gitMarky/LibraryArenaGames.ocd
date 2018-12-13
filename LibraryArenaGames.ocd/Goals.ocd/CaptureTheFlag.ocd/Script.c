#include Library_Goal_ForTeam

local Name = "$Name$";

public func GetDefaultWinScore()
{
	return 3;
}

public func GetFlagID()
{
	return Goal_Flag;
}

public func GetFlagBaseID()
{
	return Goal_FlagBase;
}

public func SetFlagBase(int team, int x, int y)
{
	var base = CreateObject(GetFlagBaseID(), x, y, NO_OWNER);
	base->SetTeam(team);

	var flag = CreateObject(GetFlagID(), x, y, NO_OWNER);
	flag->SetAction("AttachBase", base);
	flag->SetTeam(team);

	return;
}
