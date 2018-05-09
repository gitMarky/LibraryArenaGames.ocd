/**
 A temporary object.
 @author Marky
  */

static const TEMPOBJECT_Effect = "IntTemporary";

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// definitions

local Name = "Dummy";

local lifetime = 1;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// global functions

/**
 Creates a temporary object at the given coordinates. The coordinates are relative to object coordinates in local context.
 @par x The x coordinate.
 @par y The y coordinate.
 @return object Returns the temporary object, so that further function calls can be issued.
  */
global func CreateTemporaryObject(int x, int y)
{
	if (!this && (x == nil || y == nil))
	{
		FatalError("You have to specify x and y values in global context");
	}
	
	var point = CreateObject(TemporaryObject, x, y, NO_OWNER);
	return point;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// finished functions

/**
 * Defines how long the object exists once it has been activated with {@link TemporaryObject#Activate}.
 * @par frames The object will exist for this many frames.
 * @return object Returns the temporary object, so that further function calls can be issued.
 *  */
public func SetLifetime(int frames)
{
	ProhibitedWhileActive();
	
	if (lifetime < 1)
	{
		FatalError(Format("The lifetime has to be at least 1, you specified %d", frames));
	}

	lifetime = frames;

	return this;
}

/**
 Activates the temporary object. That means that the object then lives the defined time.
 */
public func Activate()
{
	if (!IsActive())
	{
		AddEffect(TEMPOBJECT_Effect, this, 1, 1, this);
	}
	
	return this;
}

private func IsActive()
{
	return GetEffect(TEMPOBJECT_Effect, this) != nil;
}

private func ProhibitedWhileActive()
{
	if (IsActive())
	{
		FatalError("This function should be used for configuring the temporary object");
	}
}

private func FxIntTemporaryTimer(object target, proplist effect_nr, int timer)
{
	if (target != this)
	{
		FatalError(Format("The effect \"%s\" may only be applied to the temporary object", TEMPOBJECT_Effect));
	}
	
	if (timer > lifetime)
	{
		target->RemoveObject();
		
		return FX_Execute_Kill;
	}

	return FX_OK;
}