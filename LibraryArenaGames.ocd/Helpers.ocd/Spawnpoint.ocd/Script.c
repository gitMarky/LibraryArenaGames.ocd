/** Object that spawns items. Has two functions: It can spawn items for each player in the game,
 or it can spawn decoration/items in fixed intervals, not depending on players.
 {@section Constants}
 The object offers new constants:
 <table>
 	<tr><th>Name</th>                     <th>Value</th> <th>Description</th></tr>
 	<tr><td>SPAWNPOINT_Timer_Default</td> <td>1000</td>  <td>The default timer until an object respawns is this many frames.</td></tr>
 	<tr><td>SPAWNPOINT_Timer_Infinite</td> <td>-1</td>   <td>Use this value if you want the object to spawn only once, when the game starts.</td></tr>
    <tr><td>SpawnPointEffectInterval()</td> <td>10</td>  <td>Defines how often the spawn point checks if the object should respawn, in frames.</td></tr>
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
public func  SpawnPointEffectInterval(){return 10;}

static const SPAWNPOINT_Effect_Collection = "IntSpawnCollect";
public func  SpawnPointCollectionRadius(){return 10;}

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
 @par mirrored If {@c true} the object will be created at {@c LandscapeWidth() - x} instead of {@c x}.]
 @return object Returns the spawn point object, so that further function calls can be issued.
 @version 0.1.0
 */
// TODO: code example
global func CopySpawnPoint(object spawn_point, int x, int y, bool mirrored)
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
	
	if (mirrored)
	{
		x = LandscapeWidth() - x;
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
local spawn_description;	// string: describes the spawnpoint type for the configuration menu.
                            
local draw_transformation;  // proplist: deco objects have this transformation
local is_active;			// bool: is it active? yes or no

local spawn_object;			// array map: player index to spawned object
local spawn_timer;			// array map: player index to respawn countdown
local spawn_globally;		// bool: spawn objects for every player individually?
							//       true - there is only one object - first come, first serve
							//       false - every player can collect one object
local spawn_team;			// int: item can be collected by a team only
local spawn_visibility;		// array map: player index to spawned object, original visibility
local spawn_collectible;    // bool: object can be collected

local spawn_callback;       // proplist:
                            // * command - string or function: the function to call in the spawned object
                            // * parameters - array: parameters for the function call, in order
                            
/**
 Marks the object as a spawn point.
 @version 0.1.0
 */
public func IsSpawnPoint(){ return true; }


public func Construction(object by_object)
{
	timer_interval = SPAWNPOINT_Timer_Default;
	spawn_id = nil;
	spawn_id_parameter = nil;
	respawn_if_removed = false;
	
	is_active = false;
	draw_transformation = nil;
	
	spawn_object = CreateArray();
	spawn_timer = CreateArray();
	spawn_visibility = CreateArray();
	spawn_globally = false;
	
	spawn_team = nil;

	spawn_collectible = false;
	
	spawn_callback = {
		command = nil,
		parameters = [],
	};
}


/**
 Copies the configuration of another spawn point of the same ID.
 @par template This should be a preconfigured spawn point. It comes handy if you want to place many
      spawn points of the same kind.
 @related {@link SpawnPoint#CopySpawnPoint} 
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
	spawn_description = template.spawn_description;

	spawn_team = template.spawn_team;
	spawn_collectible = template.spawn_collectible;
	
	spawn_callback = { Prototype = template.spawn_callback };
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

	spawn_collectible = collectible;
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
		// TODO: update docu :)
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
 @return object Returns the spawn point object, so that further function calls can be issued.
 @version 0.1.0
 */
// TODO: specify data format, update docu
public func SetTransformation(proplist transformation)
{
	draw_transformation = transformation;
	return this;
}


/**
 Sets a description string for the spawn point. This will be displayed in the game configuration
 menu when you reconfigure the spawnpoint.
 @par description This is the description. It is best to use a localized string.
 @return object Returns the spawn point object, so that further function calls can be issued.
 @related {@link Environment_Configuration#index}.
 @version 0.1.0
 */
public func SetDescription(string description)
{
	ProhibitedWhileSpawning();

	if (description == nil)
	{
		FatalError("You have to specify a parameter that is not nil");
	}
	if (GetType(spawn_id_parameter) != C4V_String)
	{
		FatalError("This function should only be used if SetID() was configured with a string");
	}

	spawn_description = description;
	return this;
}


/**
 Sets a team, so that items are collectible by team members only.

 @par team The team number that can collect this item.

 @return object Returns the spawn point object, so that further function calls can be issued.
 @version 0.3.0
 */
public func SetTeam(int team)
{
	ProhibitedWhileSpawning();

	spawn_team = team;
	return this;
}


/**
 Sets a callback that is executed on the spawned object.

 @par command This function is called in the spawned object.
              Can be a string or function pointer.
 @par parameters These parameters are added to the call.

 @return object Returns the spawn point object, so that further function calls can be issued.
 */
public func SetCallbackOnSpawn(command, par0, par1, par2, par3, par4, par5, par6, par7, par8)
{
	ProhibitedWhileSpawning();

	spawn_callback.command = command;
	
	spawn_callback.parameters[0] = par0;
	spawn_callback.parameters[1] = par1;
	spawn_callback.parameters[2] = par2;
	spawn_callback.parameters[3] = par3;
	spawn_callback.parameters[4] = par4;
	spawn_callback.parameters[5] = par5;
	spawn_callback.parameters[6] = par6;
	spawn_callback.parameters[7] = par7;
	spawn_callback.parameters[8] = par8;
	return this;
}


/**
 Gets a description of the spawn point type, if it is configurable in the game configuration.
 @related {@link SpawnPoint#SetDescription}
 @return string The description.
 @version 0.1.0
 */
public func GetDescription()
{
	return spawn_description;
}


/**
 Gets the type of object that is spawned by the spawn point.
 @return The parameter that was used in {@link SpawnPoint#SetID} to configure the spawned object.
 @version 0.1.0
 */
public func GetIDParameter()
{
	return spawn_id_parameter;
}


/**
 Gets the type of object that is actually spawned by the spawn point.
 @return The ID that was determined from the spawn ID parameter.
 @version 0.3.0
 */
public func GetIDSpawned()
{
	return spawn_id_parameter;
}


/**
 Find out whether a clonk can collect an item from this spawn point
 @return true, if an item is available for the clonk to collect.
 @version 0.3.0
 */
public func HasCollectibleItem(object clonk)
{
	return GetCollectibleItemIndex(clonk) > -1;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Spawning and object


/**
 Spawn point starts spawning, if it is enabled.
 @note This is an internal function, if you want to switch a spawn point on or off, use {@link SpawnPoint#SetActive}.
 @version 0.1.0
 */
private func StartSpawning()
{
	if (!IsSpawning())
	{
		AddEffect(SPAWNPOINT_Effect, this, 1, SpawnPointEffectInterval(), this);
	}

	if (spawn_collectible && !GetEffect(SPAWNPOINT_Effect_Collection, this))
	{
		AddEffect(SPAWNPOINT_Effect_Collection, this, 1, 1, this);
	}
}


/**
 Spawn point stops spawning, for round end and so forth. 
 @note This is an internal function, if you want to switch a spawn point on or off, use {@link SpawnPoint#SetActive}.
 @version 0.1.0
 */
private func StopSpawning()
{
	var effect = GetEffect(SPAWNPOINT_Effect, this);
	if (effect != nil)
	{
		RemoveEffect(nil, this, effect);
	}

	var collect = GetEffect(SPAWNPOINT_Effect_Collection, this);
	if (collect != nil)
	{
		RemoveEffect(nil, this, collect);
	}
}


private func IsSpawning()
{
	return GetEffect(SPAWNPOINT_Effect, this) != nil;
}


/**
 The internal timer function of the spawn effect.
 This function is called every {@c SpawnPointEffectInterval()} frames. It calls
 {@c EffectTimer(int timer)} in the spawn point. The original implementation has no effect,
 but you can implement this function for custom effects.
 @version 0.1.0
 */
private func FxIntSpawnTimer(object target, proplist effect_nr, int timer)
{
	// error handling
	var error_message = nil;
	if (target != this)
	{
		error_message = Format("The effect \"%s\" may only be applied to the spawn point", SPAWNPOINT_Effect);
	}
	else if (spawn_id == nil)
	{
		error_message = Format("Spawn point is used without an id that should be spawned. It was configured with '%v' and should spawn '%i'", spawn_id_parameter, spawn_id);
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
		for (var i = 0; i < GetPlayerCount(); i++)
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
			spawn_timer[index] -= SpawnPointEffectInterval();
		}
		return true;
	}
}


/**
 Spawns the configured object.
 @note This calls {@c EffectSpawn(int index)} in the spawn point. The original implementation has no effect,
       but you can implement this function for custom effects.
 @par index The objects are saved in an array, this parameter indicates the position in the array. 
 @version 0.1.0
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
		spawn_visibility[index] = spawn_object[index].Visibility;
		spawn_object[index].Visibility = vis;
		
		if (spawn_collectible)
		{
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
		
		if (spawn_callback && spawn_callback.command)
		{
			spawn_object[index]->Call(spawn_callback.command,
			                          spawn_callback.parameters[0],
			                          spawn_callback.parameters[1],
			                          spawn_callback.parameters[2],
			                          spawn_callback.parameters[3],
			                          spawn_callback.parameters[4],
			                          spawn_callback.parameters[5],
			                          spawn_callback.parameters[6],
			                          spawn_callback.parameters[7],
			                          spawn_callback.parameters[8]);
		}
		
		this->~EffectSpawn(index);
	}
}


/**
 Deletes a spawned object.
 @par index The objects are saved in an array, this parameter indicates the position in the array. 
 @version 0.1.0
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
// handle collection


private func FxIntSpawnCollectTimer(object target, proplist effect_nr, int timer)
{
	for (var crew in FindObjects(Find_OCF(OCF_CrewMember), Find_Distance(SpawnPointCollectionRadius())))
	{
		if (TryCollectObject(crew))
		{
			break;
		}
	}
}


private func TryCollectObject(object clonk)
{
	if (!clonk)
	{
		FatalError("Trying to give the object to nil.");
	}

	// Prevent collection if inactive
 	if (!is_active)
 	{
 		return false;
 	}

	// Prevent collection for invalid teams
	if (spawn_team && (spawn_team != GetPlayerTeam(clonk->GetOwner())))
	{
		return false;
	}

 	// Prevent collection if the clonk is not suited
 	if (!(clonk->GetOCF() & OCF_CrewMember) || (clonk->~CannotCollectFromSpawnpoints()))
	{
		return false;
	}
	
	// Prevent collection for contained clonks
	if (clonk->Contained())
	{
		return false;
	}
	
	var item_index = GetCollectibleItemIndex(clonk);
	if (item_index > -1)
	{
		DoCollectObject(item_index, clonk);
	}
}


private func GetCollectibleItemIndex(object clonk)
{
	if (spawn_globally)
	{
		return 0;
	}
	else
	{
		return GetPlayerIndex(clonk->GetOwner());
	}
}


private func DoCollectObject(int index, object clonk)
{
	var item = spawn_object[index];

	if (!item)
	{
		return false;
	}

	item.Visibility = spawn_visibility[index] ?? VIS_All; // Make item visible!
	
	if (!item->~RejectCollectionFromSpawnPoint(this, clonk))
	{
		clonk->Collect(item);
	}

	if (item && item->Contained() == this)
	{
		// collecting did not work
		// item->RemoveObject();
		return false;
	}
	else
	{
		this->~EffectCollect(item, clonk);
		
		if (!respawn_if_removed)
		{
			spawn_object[index] = nil;
		}

		SetGraphics(nil, this->GetID(), GetOverlay(index), GFXOV_MODE_Base);
		return true;
	}
}


private func EffectCollect(object item, object clonk)
{
	clonk->Sound("Clonk::Action::Grab", 0, 0, clonk->GetOwner());
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Callbacks from game configuration and round managers


public func OnConfigurationEnd(object configuration)
{
	if (GetType(spawn_id_parameter) == C4V_String && configuration != nil)
	{
		spawn_id = configuration->GetSpawnPointItem(spawn_id_parameter);
	}

	// Update name, so that the collected item is not "spawn point"
	// TODO: check if this is still necessary
	if (spawn_id && spawn_collectible)
	{
		SetName(spawn_id->~GetName() ?? GetName());
	}
}


public func OnRoundStart()
{
	RemoveSpawnedObjects();
	StartSpawning();
}


public func OnRoundEnd()
{
	StopSpawning();
	RemoveSpawnedObjects();
}


private func RemoveSpawnedObjects()
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


private func GetOverlay(int index)
{
	return GFX_Overlay + index;
}


private func ResetTimer(int index)
{
	spawn_timer[index] = timer_interval;
}
