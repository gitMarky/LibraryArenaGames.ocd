/** Object that spawns items. Has two functions: It can spawn items for each player in the game,
 or it can spawn decoration/items in fixed intervals, not depending on players.
@title Spawnpoint
@version 0.1.0
@author Marky
@credits Hazard Pack, Zapper
*/

static const SPAWNPOINT_Timer_Default = 1000;
static const SPAWNPOINT_Timer_Infinite = -1;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// definitions

local Name = "Spawnpoint";
local Collectible = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// finished functions

local timer_interval;		// the interval, in frames, until the next object is spawned after it disappeared
local spawn_id;				// spawn this id, or if it is a string, try accessing the id from a game manager object (does not exist yet)
local deco_transformation;  // deco objects have this transformation
local is_active;			// is it active? yes or no

local spawn_object;			// map: player index to spawned object
local spawn_timer;			// map: player index to respawn countdown


/**
 Marks the object as a spawn point.
 @version 0.1.0
 */
public func IsSpawnPoint(){ return true; }


protected func Construction(object by_object)
{
	timer_interval = SPAWNPOINT_Timer_Default;
	spawn_id = nil;
	is_active = false;
	deco_transformation = nil;
	
	spawn_object = nil;
	spawn_timer = 0;
}

/**
 A default function that configures the spawn point for spawning deco objects.@br
 - enables the spawn point@br
 - does not react to collection@br
 @par definition See SetSpawnID()
 @par transformation See SetTransformation()
 @return Returns the spawn point object, so that further function calls can be issued.
 @version 0.1.0
 */
public func SpawnDecoration(definition, proplist transformation)
{
	SetSpawnID(definition);
	SetTransformation(transformation);
	SetActive(true);
	SetCollectible(false);
	return this;
}

/**
 A default function that configures the spawn point for spawning collectible items.@br
 - enables the spawn point@br
 - does react to collection@br
 @par definition See SetSpawnID()
 @par transformation See SetTransformation()
 @return Returns the spawn point object, so that further function calls can be issued.
 @version 0.1.0
 */
public func SpawnItem(definition, proplist transformation)
{
	SetSpawnID(definition);
	SetTransformation(transformation);
	SetActive(true);
	SetCollectible(true);
	return this;
}

/**
 Enables or disables the spawn point.
 @par active {@c true} enables the spawn point,@br
             {@c false} disables the spawn point.
 @return Returns the spawn point object, so that further function calls can be issued.
 @version 0.1.0
 */
public func SetActive(bool active)
{
	is_active = active;
	return this;
}

/**
 Configures the spawn point so that it reacts to collection.
 This means, that Clonks running past the spawn point can collect
 the item, if their inventory permits it.
 @par collectible {@c true} enables collection,@br
                  {@c false} disables collection.
 @return Returns the spawn point object, so that further function calls can be issued.
 @version 0.1.0
 */
public func SetCollectible(bool collectible)
{
	this.Collectible = collectible;
	return this;
}

/**
 Configures the object that is spawned by the spawn point.
 @par definition An object of this type is spawned. This can be an id or a string. 
                 Passing a string has no functionality at the moment.
 @return Returns the spawn point object, so that further function calls can be issued.
 @version 0.1.0
 */
public func SetSpawnID(definition)
{
	if (GetType(definition) == C4V_Def)
	{
			spawn_id = definition;
	}
	else if (GetType(definition) == C4V_String)
	{
		FatalError("This is not supported yet, but it will be in the future.");
		// TODO: add functionality, update docu :)
	}
	else
	{
		FatalError("Function SetSpawnID() must be called with an ID or String parameter!");
	}
	return this;
}

/**
 Sets a transformation for the object. The transformation is applied to the object
 when it is spawned.
 @par transformation Passing {@c nil} disables custom transformations for the spawned object.
                     Otherwise, pass a proplist that contains data according to the format
                     specified here
 */
// TODO: specify data format, update docu
public func SetTransformation(proplist transformation)
{
	deco_transformation = transformation;
	return this;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// non-functional and temporary stuff

// temporary function probably
protected func DoSpawnObject()
{
	if (is_active && spawn_id != nil)
	{
		spawn_object = CreateObject(spawn_id, 0, 0, NO_OWNER);
	}
}

protected func RemoveSpawnedObject()
{
	if (GetType(spawn_object) == C4V_C4Object)
	{
		 spawn_object->RemoveObject();
	}
}

protected func OnRoundStart()
{
	RemoveSpawnedObject();
	DoSpawnObject();
}

protected func OnRoundEnd()
{
	RemoveSpawnedObject();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// relics

// idea by Zapper!
func ApplyDrawTransform()
{
	if(!deco_transformation) return;
	spawn_object->SetObjDrawTransform(deco_transformation.w, 0, 0, 0, deco_transformation.h);
}
