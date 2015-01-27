/*-- Basic Rule --*/

func Initialize()
{
	// Only one at a time.
	if (ObjectCount(Find_ID(GetID())) > RuleMaximumCount) 
		return RemoveObject();
}

public func Activate(int plr)
{
	MessageWindow(GetProperty("Description"), plr);
	return true;
}

local RuleMaximumCount = 1;

local Name = "$Name$";
local Description = "$Description$";
