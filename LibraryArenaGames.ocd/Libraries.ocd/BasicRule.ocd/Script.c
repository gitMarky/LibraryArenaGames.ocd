/**
 Library that gives the basic functionality for rules objects:@br
 - no more than {@c RuleMaximumCount} rule objects are created@br
 - show description of the rule when selecting the object in the rules menu
  @id index
 @title Basic Rule
 @author Marky
 */

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
