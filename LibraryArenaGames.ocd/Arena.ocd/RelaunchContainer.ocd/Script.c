/**
	Relaunch container.

 	This container holds the crew after relaunches.

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
local time_interval;
local hold;
local has_selected;

local relaunch_crew;

local Name = "$Name$";


/* --- Engine callbacks --- */

func Initialize()
{
	time = RELAUNCH_Default_Time;
	hold = RELAUNCH_Default_Hold;
	time_interval = RELAUNCH_Factor_Second;
}

/* --- Scenario saving --- */

// Disabled
public func SaveScenarioObject() { return false; }


/* --- Interface --- */

/**
	Returns the time, in seconds, that the crew is held inside the container.

	@author Maikel
 */
public func GetRelaunchTime() { return time / RELAUNCH_Factor_Second; }


/**
	Sets the time, in seconds, the crew is held in the container.

	@par to_time The time to hold the crew, in seconds.
	@par to_hold If set to true, then the crew will be contained in
                 the container until the time has fully run out.
                 Otherwise the player can exit the container earlier.
    @par interval Sets the time interval for the countdown.

	@author Maikel
 */
public func SetRelaunchTime(int to_time, bool to_hold, int interval)
{
	time = to_time * RELAUNCH_Factor_Second;
	hold = to_hold;
	time_interval = interval ?? RELAUNCH_Factor_Second;
}


/**
	Starts the relaunch process for a crew.

	@par crew This crew will be relaunched.
	@return bool Returns whether the relaunch was started successfully.
	             Reasons for unsuccessful relaunch are: The container
	             already contains another crew.
 */
public func StartRelaunch(object crew)
{
	if (PrepareRelaunch(crew))
	{
		CreateEffect(RelaunchCountdown, 100, time_interval);
		return true;
	}
	else
	{
		return false;
	}
}


/**
	Relaunches the crew immediately, if there is one.
 */
public func InstantRelaunch()
{
	if (!relaunch_crew)
	{
		FatalError("There was no object that can be relaunched. The function PrepareRelaunch() or StartRelaunch() should be called first.");
	}

	RelaunchCrew();
}


/**
	Gets the crew member that will be relaunched.

	@return the crew member, or nil if there is none.
 */
public func GetRelaunchCrew()
{
	return relaunch_crew;
}


/**
	Find out whether a relaunch is currently blocked.

	@return bool Returns {@code true} if the relaunch is blocked.
	             By default, this is if the crew has a menu,
	             or if a callback "RejectRelaunch" in the crew
	             returns {@code true}. 
 */
public func IsRelaunchBlocked()
{
	return relaunch_crew->GetMenu()
		|| relaunch_crew->~RejectRelaunch();
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

/**
	Callback from the relaunch timer.

	By default this displays the remaining time as a message above the container.

	@par frames This many frames are remaining.
 */
public func OnTimeRemaining(int frames)
{
	if (!IsRelaunchBlocked())
	{
		PlayerMessage(relaunch_crew->GetOwner(), Format("$MsgRelaunch$", frames / RELAUNCH_Factor_Second));
	}
}


/* --- Internals --- */

local RelaunchCountdown = new Effect
{
	Timer = func (int time)
	{
		// Remove empty container
		if (!this.Target.relaunch_crew)
		{
			this.Target->RemoveObject();
			return FX_Execute_Kill;
		}

		// Block a successful relaunch?
		var blocked = this.Target->IsRelaunchBlocked();

		// Message output or user-defined effects
		this.Target->OnTimeRemaining(this.Target.time - time);

		// Time has come and not blocked?
		if (time >= this.Target.time && !blocked)
		{
			this.Target->RelaunchCrew();
			return FX_Execute_Kill;
		}
		return FX_OK;
	},
};


func PrepareRelaunch(object crew)
{
	// Some sanity checks first
	AssertNotNil(crew);
	if (relaunch_crew)
	{
		return crew == relaunch_crew;
	}

	// Save crew for later use, this also signals that the callback was issued
	relaunch_crew = crew;

	// Contain crew and issue callback
	ContainCrew(crew);
	OnInitializeCrew(crew);
	return true;
}


func RelaunchCrew()
{
	// When relaunching from disabled state (i.e base respawn), reset view to crew.
	if (!relaunch_crew->GetCrewEnabled())
	{
		relaunch_crew->SetCrewEnabled(true);
		SetCursor(relaunch_crew->GetOwner(), relaunch_crew);
		SetPlrView(relaunch_crew->GetOwner(), relaunch_crew);
	}

	// Eject crew
	EjectCrew(relaunch_crew);

	// Remove relaunch time message
	PlayerMessage(relaunch_crew->GetOwner(), "");

	// Callback
	OnRelaunchCrew(relaunch_crew);
	RemoveObject();
}


func ContainCrew(object crew)
{
	if (crew->Contained() != this)
	{
		crew->Enter(this);
	}
}


func EjectCrew(object crew)
{
	crew = crew ?? FindObject(Find_Container(this), Find_OCF(OCF_CrewMember));
	if (crew)
	{
		// Eject crew and set it to the container position (because the crew would exit above the container)
		crew->Exit();
		crew->SetPosition(GetX(), GetY());
	}
}

func Ejection(object outgoing)
{
	if (!GetEffect("RelaunchCountdown", this))
	{
		RemoveObject();
	}
}
