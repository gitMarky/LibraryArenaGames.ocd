/**
	Library that gives the basic functionality for upgradeable objects:@br

	@id index
	@title Upgradeable object
	@author Marky, Hazard Team (original script in the weapon object)
 */

/* --- Properties --- */

local library_upgradeable; // proplist, saves the upgrades; this is used so that the variable namespace of the including object is not cluttered

/* --- Engine callbacks --- */

func Initialize()
{
	library_upgradeable = {upgrades = []};
	_inherited(...);
}

/* --- Interface --- */

/**
	Use this to check whether this object has an upgrade or not.

	@par upgrade the id of the upgrade.
	
	@return {@code true}, if this object has this upgrade.
 */
public func HasUpgrade(id upgrade)
{
	return IsValueInArray(GetUpgrades(), upgrade);
}


/**
	Gives access to the list of upgrades that this object has at the moment.

	@return an array with all upgrades.
 */
public func GetUpgrades()
{
	return library_upgradeable.upgrades;
} 


/**
	Tries to upgrade this object with an upgrade.

	@related {@link Library_UpgradeableObject#OnUpgrade}
	@par upgrade the id of the upgrade.
 */
public func Upgrade(id upgrade)
{
	if (this->~IsUpgradeable(upgrade))
	{
		PushBack(GetUpgrades(), upgrade);
		this->~OnUpgrade(upgrade);
		return true;
	}
}


/**
	Tries to remove an upgrade from this object.

	@related {@link Library_UpgradeableObject#OnDowngrade}
	@par upgrade the id of the upgrade.
 */
public func Downgrade(id upgrade)
{
	if (this->HasUpgrade(upgrade))
	{
		RemoveArrayValue(GetUpgrades(), upgrade, false);
		this->~OnDowngrade(upgrade);
	}
}


/**
	Check whether the object can be upgraded.

	@par upgrade the id of the upgrade.

	@return {@code true}, if the object can be upgraded with the id.
 */
public func IsUpgradeable(id upgrade){ return false; }


/**
	Callback from {@link Library_UpgradeableObject#Upgrade}.
	Does nothing by default.

	@par upgrade the id of the upgrade.
 */
public func OnUpgrade(id upgrade){}


/**
	Callback from {@link Library_UpgradeableObject#Downgrade}.
	Does nothing by default.
	
	@par upgrade the id of the upgrade.
 */
public func OnDowngrade(id upgrade){}
