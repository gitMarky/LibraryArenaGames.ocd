/**
	There may exist only one of this type of object.
	
	This allows for definition calls pointing to that object.
	Furthermore, an error is noted if you create this object multiple times.

	Must call {@code _inherited(creator, ...)} in {@ code Construction(object creator)}.

	@author Marky
*/


func Construction(object creator)
{
	_inherited(creator, ...);
	
	if (ObjectCount(Find_ID(GetID())) > 1)
	{
		RemoveObject();
		FatalError("This object may exist only once, removed the new object.");
	}
}


/**
	Gets the object instance.
	
	@return object The object, or {@code nil} if no such goal exists.
 */
public func GetInstance()
{
	AssertDefinitionContext();

	var instance = FindObject(Find_ID(this));
	
	if (this.Singleton_GetterCreatesInstance && nil == instance)
	{
		return CreateObject(this);
	}
	
	return instance;
}


local Singleton_GetterCreatesInstance = false;
