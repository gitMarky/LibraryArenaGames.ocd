/**
	Relaunch container.
 
 	This container holds the clonk after relaunches.
	* The time the clonk is held can be specified with SetRelaunchTime(int time);
	* After that time the clonk is released and OnClonkLeftRelaunch(object clonk) is called in the scenario script.
	* Optionally the clonk can choose a weapon if GetRelaunchWeaponList in the scenario script returns a valid id-array.
 
 
	{@section Constants} The object offers new constants:
	<table>
 		<tr><th>Name</th>                     <th>Value</th> <th>Description</th></tr>
 		<tr><td>RELAUNCH_Default_Time</td>  <td>360</td> <td>Time in frames, until the relaunch container ejects the player.</td></tr>
 		<tr><td>RELAUNCH_Default_Hold</td>  <td>true</td>  <td>If true, then the player cannot exit the container himself.</td></tr>
	</table>
 
 	@author Marky
	@credits Maikel
*/

/* --- Constants --- */

static const RELAUNCH_Default_Time = 360; // hold 10 seconds
static const RELAUNCH_Default_Hold = true;
static const RELAUNCH_Factor_Second = 36;

/* --- Properties --- */

local time;
local hold;
local has_selected;

local crew;

local Name = "$Name$";


/* --- Engine callbacks --- */

func Initialize()
{
	time = RELAUNCH_Default_Time;
	hold = RELAUNCH_Default_Hold;
}

/* --- Scenario saving --- */

// Disabled
public func SaveScenarioObject() { return false; }


/* --- Interface --- */

/**
	Returns the time, in seconds, that the clonk is held inside the container.
	
	@author Maikel
 */
public func GetRelaunchTime() { return time / RELAUNCH_Factor_Second; }


/**
	Sets the time, in seconds, the crew is held in the container.
 
	@par to_time The time to hold the crew, in seconds.
	@par to_hold If set to true, then the crew will be contained in
                 the container until the time has fully run out.
                 Otherwise the player can exit the container earlier.
              
	@author Maikel
 */
public func SetRelaunchTime(int to_time, bool to_hold)
{
	time = to_time * RELAUNCH_Factor_Second;
	hold = to_hold;
}


/**
	Starts the relaunch process for a clonk.
	
	@par clonk This clonk will be relaunched.
	@return bool Returns whether the relaunch was started successfully.
	             Reasons for unsuccessful relaunch are: The container
	             already contains another clonk.
 */
public func StartRelaunch(object clonk)
{
	if (PrepareRelaunch(clonk))
	{
		AddEffect("IntTimeLimit", this, 100, RELAUNCH_Factor_Second, this);
		return true;
	}
	else
	{
		return false;
	}
}


/**
	Relaunches the clonk immediately, if there is one.
 */
public func InstantRelaunch()
{
	if (!crew)
	{
		FatalError("There was no object that can be relaunched. The function PrepareRelaunch() or StartRelaunch() should be called first.");
	}
	
	RelaunchCrew();
}

/* --- Overloadable callbacks --- */


/**
	Callback when the crew is contained.
	This happens after the crew has entered the container.
	
	@par crew This object is being initialized.
 */
public func OnInitializeCrew(object crew)
{
}


/**
	Callback when the contained crew is relaunched.
	This happens after the crew has left the container and
	before the container is removed. 

	@par crew This object is being relaunched.
 */
public func OnRelaunchCrew(object crew)
{
}

/* --- Internals --- */

func FxIntTimeLimitTimer(object target, proplist effect, int fxtime)
{
	if (!crew)
	{
		RemoveObject();
		return FX_Execute_Kill;
	}
	if (fxtime >= time)
	{
		RelaunchCrew();
		return FX_Execute_Kill;
	}
	PlayerMessage(crew->GetOwner(), Format("$MsgRelaunch$", (time - fxtime) / RELAUNCH_Factor_Second));
	return FX_OK;
}


func PrepareRelaunch(object clonk)
{
	// Some sanity checks first
	AssertNotNil(clonk);
	if (crew)
	{
		return clonk == crew;
	}
	
	// Save clonk for later use and contain it
	crew = clonk;
	if (clonk->Contained() != this)
	{
		clonk->Enter(this);
	}
	
	// Callback
	OnInitializeCrew(clonk);
	return true;
}


func RelaunchCrew()
{
	// When relaunching from disabled state (i.e base respawn), reset view to clonk.
	if (!crew->GetCrewEnabled())
	{
		crew->SetCrewEnabled(true);
		SetCursor(crew->GetOwner(), crew);
		SetPlrView(crew->GetOwner(), crew);
	}
	
	// Eject crew and set it to the container position (because the crew would exit above the container)
	crew->Exit();
	crew->SetPosition(GetX(), GetY());

	// Remove relaunch time message
	PlayerMessage(crew->GetOwner(), "");
	
	// Callback
	OnRelaunchCrew(crew);
	RemoveObject();
}

