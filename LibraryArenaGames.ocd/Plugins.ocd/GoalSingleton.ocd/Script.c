/**
	Plugin for goal script.
	
	There may exist only one of this goal. This allows for definition calls pointing to that goal.
	Furthermore, an error is noted if you create this goal multiple time.

	@author Marky
*/


func Initialize()
{
	_inherited(...);
	
	if (ObjectCount(Find_ID(GetID())) > 1)
	{
		FatalError("This object may exist only once");
	}
}


/**
	Gets the goal instance.
	
	@return object The goal object, or {@code nil} if no such goal exists.
 */
public func Get()
{
	AssertDefinitionContext();

	return FindObject(Find_ID(this));
}
