/**
	There may exist a lmited amount of this type of object.

	Limitation modes:
	- Global (regards all objects)
	- Player (regards objects per player)
	- Mixed (both apply)
	
	Creation modes:
	- Rotate (new objects may be created, old objects are removed if the limit is hit)
	- Prevent (new objects may not be created if there are enough old objects)

	Must call {@code _inherited(creator, ...)} in {@ code Construction(object creator)}.

	@author Marky
*/


func Construction(object creator)
{
	_inherited(creator, ...);
	
	EvaluateObjectLimit(GetOwner());
}

/* --- Internal --- */

func AllowNewObjects(int player)
{
	var type = this->GetID();
	// New objects may always be created if the strategy is "replace old objects"
	if (this.ObjectLimitReplace)
	{
		return true;
	}
	else
	{
		// Player limits are enforced, if the player is not "-1"
		if (this.ObjectLimitPlayer > -1 && player > -1)
		{
			return ObjectCount(Find_ID(type), Find_Owner(player)) < this.ObjectLimitPlayer;
		}
		
		// Global limits?
		if (this.ObjectLimitGlobal > -1)
		{
			return ObjectCount(Find_ID(type)) < this.ObjectLimitGlobal;
		}
		
		// Defaul to: allow new objects
		return true;
	}
}

func EvaluateObjectLimit(int player)
{
	Log("Evaluating object limit for owner %d; Player limit %d, global limit %d", player, this.ObjectLimitPlayer, this.ObjectLimitGlobal);
	var replace = this.ObjectLimitReplace;
	
	// Player limits are enforced, if the player is not "-1"
	if (this.ObjectLimitPlayer > -1 && player > -1)
	{
		var objects = FindObjects(Find_ID(GetID()), Find_Owner(player));
		ApplyObjectLimit(this.ObjectLimitPlayer, objects, replace);

	}
		
	// Global limits?
	if (this.ObjectLimitGlobal > -1)
	{
		var objects = FindObjects(Find_ID(GetID()));
		ApplyObjectLimit(this.ObjectLimitGlobal, objects, replace);
	}
}

func ApplyObjectLimit(int limit, array objects, bool replace)
{
	Log("Applying object limit to %v", objects);
	while (GetLength(objects) > limit)
	{
		var remove;
		if (replace) // Relies on the sorting order. Objects with high object number are listed first
		{
			remove = PopBack(objects);
		}
		else
		{
			remove = PopFront(objects);
		}
		Log("Removing object %v", remove);
		remove->RemoveObject();
	}
}

/* --- Properties --- */

local ObjectLimitPlayer = -1; // Sets the player limit; -1 means: unlimited; 0 means: None may be created
local ObjectLimitGlobal = -1; // Sets the global limit; -1 means: unlimited; 0 means: None may be created
local ObjectLimitReplace = true; // Sets the replacement strategy: true mean: new objects replace old objects; false means: new objects are deleted first
