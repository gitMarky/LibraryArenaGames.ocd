/**
 Overloads/implements callbacks from the game configuration object.
 @author Marky
 @version 0.1
 */
 
 #appendto Environment_Configuration

func GetDefaultRules()
{
	return [];
}

func GetAvailableGoals()
{
	return [Goal_LastManStandingEx, Goal_DeathMatchEx];
}

func GetDefaultItemConfigurations()
{
	return [
	{ keys = "default", name = "Default", icon = Environment_Configuration, items = { a = { def=Firestone },
																	                  b = { def=Firestone },
																					  c = { def=Firestone },
																					  d = { def=Firestone }
																					}},
	];
}

func GetConfigurableItems()
{
	return [Firestone, IronBomb, Dynamite, Bread];
}