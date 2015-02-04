/**
 Overloads/implements callbacks from the game configuration object.
 @author Marky
 @version 0.1
 */
 
 #appendto Environment_Configuration

protected func GetDefaultRules()
{
	return [];
}

protected func GetAvailableGoals()
{
	return [Goal_LastManStanding, Goal_DeathMatch];
}

protected func GetDefaultItemConfigurations()
{
	return [
	{ name = "Default", icon = Environment_Configuration, items = { a = { def=Firestone },
																	b = { def=Firestone },
																	c = { def=Firestone },
																	d = { def=Firestone }
																	}},
	];
}

protected func GetConfigurableItems()
{
	return [Firestone, IronBomb, Dynamite, Bread];
}