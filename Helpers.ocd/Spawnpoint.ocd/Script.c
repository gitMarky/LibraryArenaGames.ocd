/** Object that spawns items. Has two functions: It can spawn items for each player in the game,
 or it can spawn decoration/items in fixed intervals, not depending on players.
 {@section Constants}
 The object offers new constants:
 <table>
 	<tr><th>Name</th>                     <th>Value</th> <th>Description</th></tr>
 	<tr><td>SPAWNPOINT_Timer_Default</td> <td>1000</td>  <td>The default timer until an object respawns is this many frames.</td></tr>
 	<tr><td>SPAWNPOINT_Timer_Infinite</td> <td>-1</td>   <td>Use this value if you want the object to spawn only once, when the game starts.</td></tr>
 </table>
@title Spawnpoint
@id index
@version 0.1.0
@author Marky
@credits Hazard Pack, Zapper
*/

static const SPAWNPOINT_Timer_Default = 1000;
static const SPAWNPOINT_Timer_Infinite = -1;

static const SPAWNPOINT_Effect = "IntSpawn";
static const SPAWNPOINT_Effect_Interval = 10;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// definitions

local Name = "Spawnpoint";

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// global functions

/**
 Creates a spawn point at the given coordinates. The coordinates are relative to object coordinates in local context.
 @par x The x coordinate.
 @par y The y coordinate.
 @return object Returns the spawn point object, so that further function calls can be issued.
 @version 0.1.0
 */
global func CreateSpawnPoint(int x, int y)
{
	if (!this && (x == nil || y == nil))
	{
		FatalError("You have to specify x and y values in global context");
	}
	
	var point = CreateObject(SpawnPoint, x, y, NO_OWNER);
	return point;
}

/**
 Uses the settings of an existing spawn point and creates a new spawn point with these settings at the given coordinates.
 The coordinates are relative to object coordinates in local context.
 @par spawn_point This has to be a spawn point object.
 @par x The x coordinate.
 @par y The y coordinate.
 @return object Returns the spawn point object, so that further function calls can be issued.
 @version 0.1.0
 */
// TODO: code example
global func CopySpawnPoint(object spawn_point, int x, int y)
{
	if (!this && (x == nil || y == nil))
	{
		FatalError("You have to specify x and y values in global context");
	}
	else if (spawn_point == nil)
	{
		FatalError("spawn_point has to be an existing object");
	}
	else if (!(spawn_point->~IsSpawnPoint()))
	{
		FatalError("spawn_point has to be return true in IsSpawnPoint()");
	}
	
	var point = CreateObject(spawn_point->GetID(), x, y, NO_OWNER);
	point->CopyDataFromTemplate(spawn_point);
	return point;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// finished functions

local timer_interval;		// int: the interval, in frames, until the next object is spawned after it disappeared
local spawn_id;				// id: spawns objects of this type
local spawn_id_parameter;	// id / string: the parameter that was used for configuring the spawned object:
                            // spawn this id, or if it is a string, try accessing the id from a game manager object (does not exist yet)
local respawn_if_removed;	// bool: per default the respawn countdown begins if the item is collected
							//       if this is set to true, then it begins if the item does not exist anymore
                            
local draw_transformation;  // proplist: deco objects have this transformation
local is_active;			// bool: is it active? yes or no

local spawn_object;			// array map: player index to spawned object
local spawn_timer;			// array map: player index to respawn countdown
local spawn_globally;		// bool: spawn objects for every player individually?
							//       true - there is only one object - first come, first serve
							//       false - every player can collect one object


/**
 Marks the object as a spawn point.
 @version 0.1.0
 */
public func IsSpawnPoint(){ return true; }


protected func Construction(object by_object)
{
	timer_interval = SPAWNPOINT_Timer_Default;
	spawn_id = nil;
	spawn_id_parameter = nil;
	respawn_if_removed = false;
	
	is_active = false;
	draw_transformation = nil;
	
	spawn_object = CreateArray();
	spawn_timer = CreateArray();
	spawn_globally = false;
	
	this.Collectible = false;
}

/**
 Copies the configuration of another spawn point of the same ID.
 @par template This should be a preconfigured spawn point. It comes handy if you want to place many
      spawn points of the same kind.
 @related {@link CopySpawnPoint} 
 @version 0.1.0
 */
public func CopyDataFromTemplate(object template)
{
	if (template == nil)
	{
		FatalError("Must specify an existing object");
	}
	else if (template->GetID() != GetID())
	{
		FatalError("Copying works only with objects of the same ID");
	}
	
	timer_interval = template.timer_interval;
	spawn_id = template.spawn_id;
	spawn_id_parameter = template.spawn_id_parameter;
	respawn_if_removed = template.respawn_if_removed;
	draw_transformation = template.draw_transformation;
	is_active = template.is_active;
	// spawn_object = template.spawn_object;
	// spawn_timer = template.spawn_timer;
	spawn_globally = template.spawn_globally;
	// spawn_category = template.spawn_category;
	
	this.Collectible = template.Collectible;
}

/**
 A default function that configures the spawn point for spawning deco objects.@br
 - enables the spawn point@br
 - does not react to collection@br
 - object spawns globally@br
 @par definition See {@link SpawnPoint#SetID}.
 @par transformation See {@link SpawnPoint#SetTransformation}.
 @return object Returns the spawn point object, so that further function calls can be issued.
 @version 0.1.0
 */
public func SpawnDeco(definition, proplist transformation)
{
	ProhibitedWhileSpawning();
	
	SetID(definition);
	SetTransformation(transformation);
	SetActive(true);
	SetCollectible(false);
	SetGlobal(true);
	return this;
}

/**
 A default function that configures the spawn point for spawning collectible items.@br
 - enables the spawn point@br
 - does react to collection@br
 @par definition See {@link SpawnPoint#SetID}.
 @par transformation See {@link SpawnPoint#SetTransformation}.
 @par spawn_global See {@link SpawnPoint#SetGlobal}.
 @return object Returns the spawn point object, so that further function calls can be issued.
 @version 0.1.0
 */
public func SpawnItem(definition, proplist transformation, bool spawn_global)
{
	ProhibitedWhileSpawning();
	
	SetID(definition);
	SetTransformation(transformation);
	SetActive(true);
	SetCollectible(true);
	SetGlobal(spawn_global);
	return this;
}

/**
 Enables or disables the spawn point.
 @par active {@c true} enables the spawn point,@br
             {@c false} disables the spawn point.
 @return object Returns the spawn point object, so that further function calls can be issued.
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
 @return object Returns the spawn point object, so that further function calls can be issued.
 @version 0.1.0
 */
public func SetCollectible(bool collectible)
{
	ProhibitedWhileSpawning();

	this.Collectible = collectible;
	return this;
}

/**
 Configures the object that is spawned by the spawn point.
 @par definition An object of this type is spawned. This can be an id or a string. 
                 Passing a string has no functionality at the moment.
 @return object Returns the spawn point object, so that further function calls can be issued.
 @version 0.1.0
 */
public func SetID(definition)
{
	ProhibitedWhileSpawning();
	
	spawn_id_parameter = definition;

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
		FatalError("Function SetID() must be called with an ID or String parameter!");
	}
	return this;
}

/**
 Configures, whether all players share the spawned object or if every player has his own object.
 @par spawn_global {@c true} The spawn point spawns one object, if one player collects it then all other players have
                   to wait for it to respawn@br
                   {@c false} The spawn point spawns one object for every player individually. This is the default option of
                   the spawn point.
 @return object Returns the spawn point object, so that further function calls can be issued.
 @version 0.1.0
 */
public func SetGlobal(bool spawn_global)
{
	ProhibitedWhileSpawning();
	
	spawn_globally = spawn_global;

	return this;
}

/**
 Sets the time until an object respawns, in frames.
 @par timer The new timer value. Use {@c SPAWNPOINT_Timer_Infinite} if you want the object to spawn at round start
            only, or a custom timer, or {@c SPAWNPOINT_Timer_Default}.
 @return object Returns the spawn point object, so that further function calls can be issued.
 @version 0.1.0
 */
public func SetRespawnTimer(int timer)
{
	if (timer < SPAWNPOINT_Timer_Infinite)
	{
		FatalError(Format("Use values >= %d", SPAWNPOINT_Timer_Infinite));
	}
	
	timer_interval = timer;
	return this;
}

/**
 Sets a transformation for the object. The transformation is applied to the object
 when it is spawned.
 @par transformation Passing {@c nil} disables custom transformations for the spawned object.
                     Otherwise, pass a proplist that contains data according to the format
                     specified here
 @version 0.1.0
 */
// TODO: specify data format, update docu
public func SetTransformation(proplist transformation)
{
	draw_transformation = transformation;
	return this;
}

/**
 Spawn point starts spawning, if it is enabled.
 @note This is an internal function, if you want to switch a spawn point on or off, use {@link SpawnPoint#SetActive}.
 */
private func StartSpawning()
{
	if (!IsSpawning())
	{
		AddEffect(SPAWNPOINT_Effect, this, 1, SPAWNPOINT_Effect_Interval, this);
	}
}

/**
 Spawn point stops spawning, for round end and so forth. 
 @note This is an internal function, if you want to switch a spawn point on or off, use {@link SpawnPoint#SetActive}.
 */
private func StopSpawning()
{
	var effect = GetEffect(SPAWNPOINT_Effect, this);
	if (effect != nil)
	{
		RemoveEffect(effect, this);
	}
}


private func IsSpawning()
{
	return GetEffect(SPAWNPOINT_Effect, this) != nil;
}

/**
 The internal timer function of the spawn effect.
 This function is called every {@c SPAWNPOINT_Effect_Interval} frames. It calls
 {@c EffectTimer(int timer)} in the spawn point. The original implementation has no effect,
 but you can implement this function for custom effects.
 */
private func FxIntSpawnTimer(object target, int effect_nr, int timer)
{
	// error handling
	var error_message = nil;
	if (target != this)
	{
		error_message = Format("The effect \"%s\" may only be applied to the spawn point", SPAWNPOINT_Effect);
	}
	else if (spawn_id == nil)
	{
		error_message = Format("Spawn point is used without an id that should be spawned. It was configured with '%s' and should spawn '%i'", spawn_id_parameter, spawn_id);
	}
	
	if (error_message != nil)
	{
		FatalError(error_message);
		return FX_Execute_Kill;
	}

	// regular behaviour
	
	if (!is_active) return FX_OK;
	
	this->~EffectTimer(timer);
	
	if (spawn_globally)
	{
		DecreaseTimer(0);
	}
	else
	{
		for(var i = 0; i < GetPlayerCount(); i++)
		{
			DecreaseTimer(i);
		}
	}
	
	return FX_OK;
}

private func ProhibitedWhileSpawning()
{
	if (IsSpawning())
	{
		FatalError("This function should be used for configuring the spawn point - it is not to be called while the spawn point is spawning");
	}
}

private func DecreaseTimer(int index)
{
	if (spawn_object[index] == nil)
	{		
		if (spawn_timer[index] <= 0)
		{
			DoSpawnObject(index);
		}
		
		if (timer_interval != SPAWNPOINT_Timer_Infinite)
		{
			spawn_timer[index] -= SPAWNPOINT_Effect_Interval;
		}
	}
}

/**
 Spawns the configured object.
 @note This calls {@c EffectSpawn(int index)} in the spawn point. The original implementation has no effect,
       but you can implement this function for custom effects.
 @par index The objects are saved in an array, this parameter indicates the position in the array. 
 */
private func DoSpawnObject(int index)
{
	ResetTimer(index);

	var vis, owner;
	
	if (spawn_globally)
	{
		vis = VIS_All;
		owner = NO_OWNER;
	}
	else
	{
		vis = VIS_Owner | VIS_God;
		owner = GetPlayerByIndex(index);
	}

	if (is_active && spawn_id != nil)
	{
		spawn_object[index] = CreateObject(spawn_id, 0, 0, owner);
		spawn_object[index].Visibility = vis;
		
		if (this.Collectible)
		{
			//spawn_object[index]->SetObjectLayer(spawn_object[index]);
			//spawn_object[index].Collectible = false;
			spawn_object[index]->Enter(this);
			SetGraphics(nil, nil, GetOverlay(index), GFXOV_MODE_Object, nil, nil, spawn_object[index]);
		}
		
		if (draw_transformation != nil)
		{
			if (draw_transformation.mesh != nil)
			{
				spawn_object[index].MeshTransformation = draw_transformation.mesh;
			}
			else
			{
				spawn_object[index]->SetObjDrawTransform(draw_transformation.width,
														 draw_transformation.xskew, 
														 draw_transformation.xadjust,
														 draw_transformation.yskew,
														 draw_transformation.height,
														 draw_transformation.yadjust,
														 draw_transformation.overlay_id);
			}
		}
		
		this->~EffectSpawn(index);
	}
}

/**
 Deletes a spawned object.
 @par index The objects are saved in an array, this parameter indicates the position in the array. 
 */
private func RemoveSpawnedObject(int index)
{
	if (GetType(spawn_object[index]) == C4V_C4Object)
	{
		 spawn_object[index]->RemoveObject();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// non-functional and temporary stuff


protected func RejectEntrance(object clonk)
{
 	if (!is_active
	 || !(clonk->GetOCF() & OCF_CrewMember)
	 || (clonk->~CannotCollectFromSpawnpoints()))
	{
		return true;
	}
	
	var index = -1;
	
	if (spawn_globally)
	{
		index = 0;
	}
	else
	{
		for (var i = 0; i < GetLength(spawn_object); i++)
		{
			if (GetPlayerByIndex(i) == clonk->GetOwner())
			{
				index = i;
				break;
			}
		}
	}
	
	if (index > -1)
	{
		var item = spawn_object[index];
		
		if (item != nil)
		{
			DoCollectObject(item, index, clonk);
		}
	}
	return true;
}

private func EffectCollect(object item, object clonk)
{
	clonk->Sound("Grab", 0, 0, clonk->GetOwner());
}

private func DoCollectObject(object item, int index, object clonk)
{	
	clonk->Collect(item);
		
	if (item->Contained() == this)
	{
		// collecting did not work
		// item->RemoveObject();
		
	}
	else
	{
		this->~EffectCollect(item, clonk);
		
		if (!respawn_if_removed)
		{
			spawn_object[index] = nil;
		}
		
		SetGraphics(nil, this->GetID(), GetOverlay(index), GFXOV_MODE_Base);
	}
}

protected func OnRoundStart()
{
	RemoveSpawnedObjects();
	StartSpawning();
}

protected func OnRoundEnd()
{
	StopSpawning();
	RemoveSpawnedObjects();
}

protected func RemoveSpawnedObjects()
{
	if (spawn_globally)
	{
		RemoveSpawnedObject(0);
	}
	else
	{
		for(var i=0; i < GetPlayerCount(); i++)
		{
			RemoveSpawnedObject(i);
		}
	}
}

protected func GetOverlay(int index)
{
	return GFX_Overlay + index;
}

protected func ResetTimer(int index)
{
	spawn_timer[index] = timer_interval;
}
