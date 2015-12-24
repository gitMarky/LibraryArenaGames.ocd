/**
 Relaunch container.
 
 {@section Constants} The object offers new constants:
 <table>
 	<tr><th>Name</th>                     <th>Value</th> <th>Description</th></tr>
 	<tr><td>RELAUNCH_Default_Time</td>  <td>360</td> <td>Time in frames, until the relaunch container ejects the player.</td></tr>
 	<tr><td>RELAUNCH_Default_Hold</td>  <td>true</td>  <td>If true, then the player cannot exit the container himself.</td></tr>
 </table>
 
 @author Marky
 @credits Maikel
 @version 0.1.0
*/



static const RELAUNCH_Default_Time = 360; // hold 10 seconds
static const RELAUNCH_Default_Hold = true;
static const RELAUNCH_Factor_Second = 36;

//----------------------------------------------------------------------------
/*--
	Relaunch container
	Author: Maikel

	This container holds the clonk after relaunches.
	* The time the clonk is held can be specified with SetRelaunchTime(int time);
	* After that time the clonk is released and OnClonkLeftRelaunch(object clonk) is called in the scenario script.
	* Optionally the clonk can choose a weapon if GetRelaunchWeaponList in the scenario script returns a valid id-array.
--*/

local time;
local menu;
local hold;
local has_selected;

local crew;

/**
 Sets the time, in seconds, the crew is held in the container.
 @par to_time The time to hold the crew, in seconds.
 @par to_hold If set to true, then the crew will be contained in
              the container until the time has fully run out.
              Otherwise the player can exit the container earlier.
 @author Maikel
 @version 0.2.1
 */
public func SetRelaunchTime(int to_time, bool to_hold)
{
	time = to_time * RELAUNCH_Factor_Second;
	hold = to_hold;
}

/**
 Returns the time, in seconds the clonk is held.
 @author Maikel
 @version 0.2.1
 */
public func GetRelaunchTime() { return time / RELAUNCH_Factor_Second; }

/**
 Retrieve weapon list from scenario.
 This issues a game call "RelaunchWeaponList" that should return an array of IDs.
 @author Maikel
 @version 0.2.1
 */ 
public func WeaponList() { return GameCall("RelaunchWeaponList"); }


private func OpenWeaponMenu(object clonk)
{
	if (!clonk)	return;	
	if (!menu)
	{
		var weapons = WeaponList();
		if (weapons && GetLength(weapons) > 0)
		{
			menu = CreateObject(MenuStyle_Default, nil, nil, clonk->GetOwner());
			menu->SetPermanent();
			menu->SetTitle(Format("$MsgWeapon$", time / RELAUNCH_Factor_Second));
			clonk->SetMenu(menu); 

			for (var weapon in weapons)
				menu->AddItem(weapon, weapon->GetName(), nil, this, "OnWeaponSelected", weapon);
	
			menu->Open();
		}
	}
}

func FxIntTimeLimitTimer(object target, effect, int fxtime)
{
	if (!crew)
	{
		RemoveObject();
		return FX_Execute_Kill;
	}
	if (fxtime >= time)
	{
		var weapons = WeaponList();
		if (!has_selected && weapons && GetLength(weapons) > 0)
			GiveWeapon(weapons[Random(GetLength(weapons))]);
		RelaunchCrew();
		return FX_Execute_Kill;
	}
	if (menu)
		menu->SetTitle(Format("$MsgWeapon$", (time - fxtime) / RELAUNCH_Factor_Second));
	else
		PlayerMessage(crew->GetOwner(), Format("$MsgRelaunch$", (time - fxtime) / RELAUNCH_Factor_Second));
	return FX_OK;
}

public func OnWeaponSelected(id weapon)
{
	GiveWeapon(weapon);
	
	has_selected = true;
	// Close menu manually, to prevent selecting more weapons.
	if (menu)
		menu->Close();

	if (!hold)
		RelaunchCrew();
	return true;
}

private func RelaunchCrew()
{
	crew->Exit();
	if (menu)
		menu->Close();
	PlayerMessage(crew->GetOwner(), "");
	OnRelaunchCrew(crew);
	RemoveObject();
}


/**
 Callback when the contained crew is relaunched.
 This happens after the crew has left the container and
 before the container is removed. 
 @version 0.2.0
 */
public func OnRelaunchCrew(object crew)
{
}

/**
 Is called when the player selects a weapon.
 This method should create the weapon and
 let the player collect it. By default this function
 does nothing.
 @par weapon_id This weapon was selected.
 */
private func GiveWeapon(id weapon_id)
{
}

public func SaveScenarioObject() { return false; }

local Name = "$Name$";

//----------------------------------------------------------------------------

protected func Initialize()
{
	time = RELAUNCH_Default_Time;
	hold = RELAUNCH_Default_Hold;
}

public func PrepareRelaunch(object clonk)
{
	if (!clonk) return false;
	if (crew)
	{
		if (clonk == crew) 
		{
			return true;
		}
		return false;
	}
	// save clonk for later use
	crew = clonk;
	clonk->Enter(this);
	return true;
}

public func StartRelaunch(object clonk)
{
	if (PrepareRelaunch(clonk))
	{
		ScheduleCall(this, "OpenWeaponMenu", RELAUNCH_Factor_Second, 0, clonk);
		AddEffect("IntTimeLimit", this, 100, RELAUNCH_Factor_Second, this);
		return true;
	}
	else
	{
		return false;
	}
}

public func InstantRelaunch()
{
	if (!crew)
	{
		FatalError("There was no object that can be relaunched. The function PrepareRelaunch() or StartRelaunch() should be called first.");
	}
	
	RelaunchCrew();
}
