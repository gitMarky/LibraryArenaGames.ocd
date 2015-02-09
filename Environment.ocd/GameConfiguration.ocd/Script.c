/**
 Allows the player to configure the game.
 @author Marky
 @version 0.1.0
 */

static const GAMECONFIG_Proplist_Items = "items";
static const GAMECONFIG_Proplist_Def = "def";
static const GAMECONFIG_Proplist_Desc = "description";

static const GAMECONFIG_Icon_DefaultItemConfiguration = Environment_Configuration;
static const GAMECONFIG_Icon_Goals = Icon_Goals;
static const GAMECONFIG_Icon_Rules = Icon_Rules;
static const GAMECONFIG_Icon_Teams = Icon_Teams;
static const GAMECONFIG_Icon_Bots = Icon_Bots;
static const GAMECONFIG_Icon_Items = Icon_Items;
static const GAMECONFIG_Icon_ItemsCustom = Icon_Items;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// definitions

local Name = "$Name$";
local Description = "$Description$";

local configuration_rules;		// proplist, consists of:
								// * def - id: the id of the rule
								// * instances - array: all objects with the given id
								// * is_active - bool: true, if the rule is configured
								// * symbol - a symbol dummy for the menus

local configured_items;			// proplist, structure not defined yet.
								// * key - string: this identifies the 
                                // * name - string: a descriptive name for the configuration
                                // * icon - id: an icon to choose in the menu
                                // * items - proplist: the item configurations
                                
// spawn point configurations:
// name - string: parameter for the spawn point
// description - string: spawn point description, will be displayed in the menu
// ??? - reference which slot to take from a default configuration set?
// item - id: this item will be spawned

local selected_goals;			// array, bool: The goals, in order of GetAvailableGoals(). True means, that the goal is selected
local configured_goal;			// object: the goal that has been created
local player_index;				// int: the player that configures the current round

local configuration_finished;	// bool: true once the configuration is done

local spawnpoint_keys; 				// array: contains spawnpoint keys.

local color_conflict = -6946816; // = RGB(150, 0, 0);
local color_inactive = -4934476; // = RGB(180, 180, 180);
local color_active = -1; // = RGB(255, 255, 255);


/**
 Makes bots configurable if it returns {@c true}.
 @return bool The default value is {@c true}.
 */
public func CanConfigureBots()
{
	return true;
}

/**
 Makes configurable spawn points configurable if it returns {@c true}.
 @return bool The default value is {@c true}.
 */
public func CanConfigureSpawnPoints()
{
	return true;
}


/**
 Tells objects, that this is a game configuration. This becomes important if you include the
 configuration object.
 @return bool {@c true}
 */
public func IsGameConfiguration()
{
	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// global functions

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// finished functions

/**
 You cannot cancel the configuration process. This could have catastrophic results, such as the round not starting.
 */
func MenuQueryCancel()
{
	return true;
}

/**
 Initializes the local variables, then 1 frame later calls in order:@br
 1.) {@link Environment_Configuration#ScanRules}@br
 2.) {@link Environment_Configuration#PreconfigureRules}@br
 3.) {@c GameCall("OnConfigurationStart")}@br
 3.) {@link Environment_Configuration#OpenMainMenu}@br
 @version 0.1.0
 */
protected func Initialize()
{
	SetPosition();
	configuration_rules = {};
	configured_items = { key="default", name="default", icon = GAMECONFIG_Icon_DefaultItemConfiguration, items = {}};
	player_index = 0;
	selected_goals = [];
	
	// wait for other rules, etc. to be initialized
	ScheduleCall(this, "PostInitialize", 1);
}
	
protected func PostInitialize()
{
	ScanRules();
	PreconfigureRules();
	
	ScanSpawnPoints();
	
	GameCallEx("OnConfigurationStart", this);
	
	if (RoundManager() == nil)
	{
		OpenMainMenu();
	}
}

/**
 Callback from the round manager, calls {@link Environment_Configuration#OpenMainMenu}.
 */
public func OnRoundReset(int round_number)
{
	RoundManager()->RegisterRoundStartBlocker(this);
	
	configuration_finished = false;
	
	// put players in spawn points
	ContainPlayers(); 
	
	// cycle the player who configures the round
	var players  = GetPlayerCount(C4PT_User);
	player_index = (round_number - 1) % players;
	
	OpenMainMenu();
}

/**
 Engine callback. Puts the player into a relaunch container if the configuration process is not finished.
 */
public func InitializePlayer(int player, int x, int y, object base, int team, id extra_data)
{
	if (!configuration_finished)
	{
		ContainPlayer(player);
	}
}


/*-----------------------------------------------------------------------------------------------------------------------------------------

  Main menu
  
  -----------------------------------------------------------------------------------------------------------------------------------------*/

/**
 Opens the configuration menu in the choosing player and closes menus in other players.
 Calls {@link Environment_Configuration#CreateMainMenu}.
 for custom effects.
 @note Has a callback {@c OnOpenMainMenu()}.
 @version 0.1.0
 */
protected func OpenMainMenu(id dummy)
{
	var player = GetChoosingPlayer();
	if (!player) return ScheduleCall(this, "OpenMainMenu", 1);
	
	for (var i = 0; i < GetPlayerCount(); i++)
	{
		var cursor = GetCursor(GetPlayerByIndex(i));
		if (cursor != nil
		&&  cursor->GetMenu())
			cursor->CloseMenu();
	}
	
	Message("", player);
	
	CreateMainMenu(player);
		
	// callback for effects, etc.
	this->~OnOpenMainMenu();
}

/**
 Opens the main menu. You can/should overload this function in your own project
 if you do not like the current choice or order of menu options.@br
 By default the function calls, in order:@br
 1.) {@link Environment_Configuration#CreateConfigurationMenu}@br
 2.) {@link Environment_Configuration#MainMenuAddItemGoal}@br
 3.) {@link Environment_Configuration#MainMenuAddItemWinScore}@br
 4.) {@link Environment_Configuration#MainMenuAddItemRules}@br
 5.) Callback {@c MainMenuAddItemCustom(object player)}@br]
 6.) {@link Environment_Configuration#MainMenuAddItemFinishConfiguration}
 
 @par player The menu is displayed in this object.
 @version 0.1.0
 */
protected func CreateMainMenu(object player)
{
	CreateConfigurationMenu(player, GetID(), "$MenuCaption$");

	MainMenuAddItemGoal(player);
	MainMenuAddItemWinScore(player);
	MainMenuAddItemRules(player);
	MainMenuAddItemTeams(player);
	MainMenuAddItemBots(player);
	MainMenuAddItemItems(player);
	
	this->~MainMenuAddItemCustom(player);

	MainMenuAddItemFinishConfiguration(player);
}

/**
 Adds an option for adding and removing AI players (bots, 
 if {@link Environment_Configuration#CanConfigureSpawnPoints} returns {@c true}.
 @par player The menu is displayed in this object.
 @version 0.1.0
 */
protected func MainMenuAddItemBots(object player)
{
	if (CanConfigureBots())
	{
		player->AddMenuItem("$TxtConfigureBots$", "MenuConfigureBots", GAMECONFIG_Icon_Bots, nil, player);
	}
}

/**
 Adds an option for choosing a goal for the scenario. Behaviour depens on the callback@br
 {@c GetAvailableGoals()}, which should return an array of definitions:@br
 - It does nothing if the function returns {@c nil}. This is ideal if you already have a goal in the scenario.
   Should there be such a goal, then it will not be configurable with {@link Enviroment_Configuration#MainMenuAddItemWinScore}@br
 - It creates the only available goal if the array has exactly one entry. It will be configurable with {@link Enviroment_Configuration#MainMenuAddItemWinScore}@br
 - It creates a menu with the available goals and has the player choose one goal.
 
 @par player The menu is displayed in this object.
 @version 0.1.0
 */
protected func MainMenuAddItemGoal(object player)
{
	// do nothing if we have a goal already
	if (configured_goal != nil) return;

	// see if there are any goals
	// otherwise assume that the scenario configured the goals already
	var goals = this->~GetAvailableGoals();
	if (goals == nil) return;

	// create the only available goal
	if (GetLength(goals) == 1)
	{
		CreateGoal(goals[0]);
	}
	else // or configure a goal
	{
		// passing an array as parameter is not allowed
		//player->AddMenuItem("$TxtConfigureGoals$", "MenuChooseGoal", menu_symbol, nil, goals);
		player->AddMenuItem("$TxtConfigureGoals$", "MenuChooseGoal", GAMECONFIG_Icon_Goals, nil, player);
	}
}

/**
 Adds an option for configuration of the equipment that is spawned by spawn points, 
 if {@link Environment_Configuration#CanConfigureSpawnPoints} returns {@c true}.
 @par player The menu is displayed in this object.
 @version 0.1.0
 */
protected func MainMenuAddItemItems(object player)
{
	if (CanConfigureSpawnPoints())
	{
		player->AddMenuItem("$TxtConfigureItems$", "MenuConfigureItems", GAMECONFIG_Icon_Items, nil, player);
	}
}

/**
 Adds an option for configuration of game rules.
 @par player The menu is displayed in this object.
 @version 0.1.0
 */
protected func MainMenuAddItemRules(object player)
{
	// do nothing if no rules are configurable
	if (GetLength(GetProperties(configuration_rules)) > 0)
		player->AddMenuItem("$TxtConfigureRules$", "MenuConfigureRules", GAMECONFIG_Icon_Rules, nil, player);
}

/**
 Adds an option for switching players between teams, if the scenario supports teams.
 @par player The menu is displayed in this object.
 @version 0.1.0
 */
protected func MainMenuAddItemTeams(object player)
{
	// do nothing if the goal is not a team goal
	//if (configured_goal != nil && !(configured_goal->~IsTeamGoal())) return;
	if (!GetTeamCount()) return;
	
	player->AddMenuItem("$TxtConfigureTeams$", "MenuConfigureTeams", GAMECONFIG_Icon_Teams, nil, player);
}

/**
 Adds an option for choosing the win score of the chosen goal.
 
 @par player The menu is displayed in this object.
 @version 0.1.0
 */
protected func MainMenuAddItemWinScore(object player)
{
	// do nothing if we have a goal already
	if (configured_goal == nil) return;
	
	player->AddMenuItem(configured_goal->GetName(), "MenuConfigureGoal", configured_goal->GetID(), nil, player);
}

/**
 Adds a finish option to a menu, the option calls {@link Environment_Configuration#ConfigurationFinished}.
 
 @par player The menu is displayed in this object.
 */
protected func MainMenuAddItemFinishConfiguration(object player)
{
	player->AddMenuItem("$Finished$", "ConfigurationFinished", Icon_Ok, nil, nil, "$Finished$");
}



/*-----------------------------------------------------------------------------------------------------------------------------------------

  Sub menus
  
  -----------------------------------------------------------------------------------------------------------------------------------------*/

/**
 Opens a menu for choosing goals. Has a callback {@c MenuChooseGoalCustomEntries(object player)} for adding further options.
 
 @note Has a callback {@c OnMenuChooseGoal()}.
 @par menu_symbol The menu has this icon.
 @par player The menu is displayed in this object.
 @version 0.1.0
 */
protected func MenuChooseGoal(id menu_symbol, object player, int selection)
{
	this->~OnMenuChooseGoal();

	var goals = this->~GetAvailableGoals();
	if (!player || !goals) return ScheduleCall(this, "OpenMainMenu", 1);

	CreateConfigurationMenu(player, menu_symbol, "$TxtConfigureGoals$");
	
	var has_selection = false;
	
	for (var i = 0; i < GetLength(goals); i++)
	{
		var goal = goals[i];
		var selected = selected_goals[i];
		
		var caption = goal->GetName();
		
		if (selected)
		{
			has_selection = true;
			caption = ColorizeString(caption, color_active);
		}
		else
		{
			caption = ColorizeString(caption, color_inactive);
		}
	
		player->AddMenuItem(caption, Format("ChooseGoal(%i, %i, Object(%d), %d)", menu_symbol, goal, player->ObjectNumber(), i), goal);
	}

	this->~MenuChooseGoalCustomEntries(player);
	
	if (has_selection) player->AddMenuItem("$Finished$", "SetupGoal()", Icon_Ok, nil, nil, "$Finished$");
	
	player->SelectMenuItem(selection);
}

/**
 Opens a menu for adding and removing bots.
 @note Has a callback {@c OnMenuConfigureBots()}.
 @par menu_symbol The menu has this icon.
 @par player The menu is displayed in this object.
 @par selection This option will be selected when the menu is open. First option is 0.
 @par block_interaction If {@c true}, then the menu will be displayed grey and options have effect. This
                        should visualize that a player is being added or removed. 
 @version 0.1.0
 */
protected func MenuConfigureBots(id menu_symbol, object player, int selection, bool block_interaction)
{
	this->~OnMenuConfigureBots();

	CreateConfigurationMenu(player, menu_symbol, "$TxtConfigureBots$");

	var number_bots = GetPlayerCount(C4PT_Script);
	var number_players = GetPlayerCount();
	
	var command0, command1, command2, command3;
	var caption0 = Format("$TxtPlayersBots$", number_bots, number_players);
	var caption1 = "$MoreBots$";
	var caption2 = "$LessBots$";
	var caption3 = "$Finished$";
	

	if (block_interaction)
	{
		command0 = Format("MenuConfigureBots(%i, Object(%d), %d, true)", menu_symbol, player->ObjectNumber(), 0);
		command1 = Format("MenuConfigureBots(%i, Object(%d), %d, true)", menu_symbol, player->ObjectNumber(), 1);
		command2 = Format("MenuConfigureBots(%i, Object(%d), %d, true)", menu_symbol, player->ObjectNumber(), 2);
		command3 = Format("MenuConfigureBots(%i, Object(%d), %d, true)", menu_symbol, player->ObjectNumber(), 3);
		
		caption0 = ColorizeString(caption0, color_conflict);
		caption1 = ColorizeString(caption1, color_conflict);
		caption2 = ColorizeString(caption2, color_conflict);
		caption3 = ColorizeString(caption3, color_conflict);
	}
	else
	{
		command0 = Format("MenuConfigureBots(%i, Object(%d), %d)", menu_symbol, player->ObjectNumber(), 0);
		command1 = Format("ChangeBotAmount(%i, Object(%d), %d, %d)", menu_symbol, player->ObjectNumber(), 1, +1);
		command2 = Format("ChangeBotAmount(%i, Object(%d), %d, %d)", menu_symbol, player->ObjectNumber(), 2, -1);
		command3 = "OpenMainMenu()";
	}
	
	player->AddMenuItem(caption0, command0, menu_symbol);
	player->AddMenuItem(caption1, command1, Icon_Plus, nil, nil, "$MoreBots$");
	player->AddMenuItem(caption2, command2, Icon_Minus, nil, nil, "$LessBots$");
	player->AddMenuItem(caption3, command3, Icon_Ok, nil, nil, "$Finished$");
	
	player->SelectMenuItem(selection);
}

/**
 Opens a menu for changing the win score of goals.
 
 @note Has a callback {@c OnMenuConfigureGoal()}.
 @par player The menu is displayed in this object.
 @par menu_symbol The menu has this icon. This should be the id of the goal.
 @par selection This entry will be selected. Makes choosing the same option several times a lot more easy.
 @version 0.1.0
 */
protected func MenuConfigureGoal(id menu_symbol, object player, int selection)
{
	this->~OnMenuConfigureGoal();

	CreateConfigurationMenu(player, menu_symbol, menu_symbol->GetName());
	
	player->AddMenuItem(" ", Format("MenuConfigureGoal(%i, Object(%d), %d)", menu_symbol, player->ObjectNumber(), 0), menu_symbol, configured_goal->~GetWinScore());
	player->AddMenuItem("$MoreWinScore$", Format("ChangeWinScore(%i, Object(%d), %d, %d)", menu_symbol, player->ObjectNumber(), 1, +1), Icon_Plus, nil, nil, "$MoreWinScore$");
	player->AddMenuItem("$LessWinScore$", Format("ChangeWinScore(%i, Object(%d), %d, %d)", menu_symbol, player->ObjectNumber(), 2, -1), Icon_Minus, nil, nil, "$LessWinScore$");

	MenuAddItemReturn(player);

	player->SelectMenuItem(selection);
}

/**
 Opens a menu for configuring the spawn points and equipment. Preconfigured options are
 available as defined by the callback {@c GetDefaultItemConfigurations()}. This should
 return an array of configurations.

 @note Has a callback {@c OnMenuConfigureItems()}.
 @par menu_symbol The menu has this icon.
 @par player The menu is displayed in this object.
 @par selection This option will be selected when the menu is open. First option is 0.
 @version 0.1.0
 */
protected func MenuConfigureItems(id menu_symbol, object player, int selection)
{	
	this->~OnMenuConfigureItems();

	var configurations = this->~GetDefaultItemConfigurations();
	if (configurations != nil)
	{
		CreateConfigurationMenu(player, GAMECONFIG_Icon_Items, "$TxtConfigureItems$");

		for (var i = 0; i < GetLength(configurations); i++)
		{
			var config = configurations[i];
			if (config == nil)
			{
				Log("this is seriously wrong: %d", i);
				continue;
			}
			
			var caption = config.name;
			
			var selected = config.key == configured_items.key;
			
			for (var key in spawnpoint_keys)
			{
				var current = GetItemConfiguration(key);
				var source = GetItemConfiguration(key, config);
				
				if (GetProperty(GAMECONFIG_Proplist_Def, current) != GetProperty(GAMECONFIG_Proplist_Def, source))
				{
					selected = false;
					break;
				}
			}
			
			if (selected)
			{
				caption = ColorizeString(caption, color_active);
			}
			else
			{
				caption = ColorizeString(caption, color_inactive);
			}

			player->AddMenuItem(caption, Format("ConfigureItemSet(%i, Object(%d), %d)", config.icon, player->ObjectNumber(), i), config.icon);
		}
		
		player->AddMenuItem("$TxtConfigureSpecificItems$", Format("MenuConfigureItemsCustom(%i, Object(%d), 0, true)", GAMECONFIG_Icon_ItemsCustom, player->ObjectNumber()), GAMECONFIG_Icon_ItemsCustom);	

		MenuAddItemReturn(player);
		
		player->SelectMenuItem(selection);
	}
	else
	{
		// open the custom menu right away
		MenuConfigureItemsCustom(menu_symbol, player);
	}
}

/**
 Displays all configurable spawn points in the map so that their spawned item can be modified.
 @note A spawnpoint has to be configured with a string in {@link SpawnPoint#SetID} for it to be recognized
       as configurable. You should provide a localized description string with {@link SpawnPoint#SetDescription}.
       Has a callback {@c OnMenuConfigureItemsCustom()}.
 @par menu_symbol The menu has this icon.
 @par player The menu is displayed in this object.
 @par selection This option will be selected when the menu is open. First option is 0.
 @par has_default_configurations If true, then the option returns to {@link Environment_Configuration#MenuConfigureItems}
 @version 0.1.0
 */
protected func MenuConfigureItemsCustom(id menu_symbol, object player, int selection, bool has_default_configurations)
{
	this->~OnMenuConfigureItemsCustom();

	CreateConfigurationMenu(player, menu_symbol, "$TxtConfigureSpecificItems$");

	// spawn points
		
	var selection_counter = 0;
	
	for (var key in spawnpoint_keys)
	{
		var current_config = GetItemConfiguration(key);
		var current_item = GetProperty(GAMECONFIG_Proplist_Def, current_config);
		var description = GetProperty(GAMECONFIG_Proplist_Desc, current_config);
	
		var command = Format("MenuConfigureItemSlot(%i, Object(%d), \"%s\", %d, true, 0)", menu_symbol, player->ObjectNumber(), key, selection_counter++);
		player->AddMenuItem(description, command, current_item);
	}
	
	// equipment

	if (has_default_configurations)
	{
		player->AddMenuItem("$Finished$", "MenuConfigureItems", Icon_Ok, nil, player, "$Finished$");
	}
	else
	{
		MenuAddItemReturn(player);
	}
	
	player->SelectMenuItem(selection);
}

/**
 Configures a spawn point or equipment from a list of available items. These are specified by the callback
 {@c GetConfigurableItems()} which should return an array of IDs.
 
 @note Has a callback {@c OnMenuConfigureItemSlot()}.
 @par menu_symbol The menu has this icon.
 @par player The menu is displayed in this object.
 @par key This is the value configured in {@link SpawnPoint#SetID} of the spawn point you want to configure.
 @par index This option was selected in the parent menu. Is necessary for smooth navigation through the menus.
 @par configure_spawnpoint Should be {@true} if you want to configure a spawnpoint, or {@c false} if you want to configure equipment.
 @par selection This option will be selected when the menu is open. First option is 0.
 @version 0.1.0
 */
protected func MenuConfigureItemSlot(id menu_symbol, object player, string key, int index, bool configure_spawnpoint, int selection)
{
	this->~OnMenuConfigureItemSlot();

	var current_config = GetItemConfiguration(key);
	var current_item = GetProperty(GAMECONFIG_Proplist_Def, current_config);
	var description = GetProperty(GAMECONFIG_Proplist_Desc, current_config);
	CreateConfigurationMenu(player, menu_symbol, Format("$TxtConfigureSlot$", description));
	
	var items = this->~GetConfigurableItems();
	
	for (var i = 0; i < GetLength(items); i++)
	{
		var item = items[i];
		var selected = item == current_item;
		
		var name = item->GetName();
		
		if (selected)
		{
			name = ColorizeString(name, color_active);
		}
		else
		{
			name = ColorizeString(name, color_inactive);
		}		
		
		var command = Format("ConfigureItemSlot(%i, Object(%d), \"%s\", %d, %v, %i, %d)", menu_symbol, player->ObjectNumber(), key, index, configure_spawnpoint, item, i);
		
		player->AddMenuItem(name, command, item);
	}
	
	player->AddMenuItem("$Finished$", Format("MenuConfigureItemsCustom(%i, Object(%d), %d, true)", menu_symbol, player->ObjectNumber(), index), Icon_Ok, nil, nil, "$Finished$");

	player->SelectMenuItem(selection);
}

/**
 Opens a menu that allows the configuration of rules objects. Inactive rules are colored in {@c color_inactive},
 active rules in {@c color_active}. Some rules exclude each other or require rules to be active. In case of such a
 conflict a rule that cannot be chosen is colored in {@c color_conflict}.
 @note An object is displayed in this list if it has the category {@c C4D_Rule} and includes {@c Library_ConfigurableRule}.
       Has a callback {@c OnMenuConfigureBots()}.
 @par menu_symbol The menu has this icon.
 @par player The menu is displayed in this object.
 @par selection This option will be selected when the menu is open. First option is 0.
 @version 0.1.0
 */
protected func MenuConfigureRules(id menu_symbol, object player, int selection)
{
	this->~OnMenuConfigureRules();

	CreateConfigurationMenu(player, menu_symbol, "TxtConfigureRules");
	
	var select = 0;
	
	var keys = GetProperties(configuration_rules);
	
	for (var i = 0, check; i < GetLength(keys); i++)
	{
		var rule_info = GetProperty(keys[i], configuration_rules);

		var rule_id = rule_info.def;

		var dummy = rule_info.symbol;
		
		var conflict = false;
		
		var color = color_inactive;
		
		var rules_required = rule_id->~GameConfigRequiredRules();

		for (var k = 0; k < GetLength(keys); k++)
		{
			if (k == i) continue; // it should not conflict with itself
			
			var conflict_info = GetProperty(keys[k], configuration_rules);
			var conflict_id = conflict_info.def;
			
			var has_conflict = false, has_dependency = false;
			
			var rules_conflicts = conflict_id->~GameConfigConflictingRules();
			
			if (GetType(rules_conflicts) == C4V_Array
		    &&  IsValueInArray(rules_conflicts, rule_id)
		    &&  conflict_info.is_active)
			{
				has_conflict = true;
				color = color_conflict;
			}
			
			if (GetType(rules_required) == C4V_Array
			&&  IsValueInArray(rules_required, conflict_id)
			&& !conflict_info.is_active)
			{
				has_dependency = true;
			}
			
			if (has_conflict || has_dependency)
			{
				rule_info.is_active = false;
				conflict = true;
				break;
			}
		}
		
		
		if (rule_info.is_active)
		{
			color = color_active;
		}
		else
		{
		}
		
		var command;
		
		if (conflict)
		{
			command = "";
		}
		else
		{
			command = Format("ChangeRuleConf(%i, Object(%d), %d)", menu_symbol, player->ObjectNumber(), i);
		}
		
		dummy->SetClrModulation(color);
		
		player->AddMenuItem(ColorizeString(rule_info.def->GetName(), color), command, rule_info.def, nil, i, nil, 4, dummy);
		
		if(i == selection && !conflict) check = true;
		
		if(!check) select++;
	}
	
	MenuAddItemReturn(player);
	player->SelectMenuItem(select);
}

/**
 Lists all players and switches the teams of a player if he is selected. The menu is sorted
 by teams. After a player changes sides, his entry will be selected again. 
 @note Has a callback {@c OnMenuConfigureTeams()}.
 @par menu_symbol The menu has this icon.
 @par player The menu is displayed in this object.
 @par selection This option will be selected when the menu is open. First option is 0.
 @version 0.1.0
 */
protected func MenuConfigureTeams(id menu_symbol, object player, int selection)
{
	this->~OnMenuConfigureTeams();

	CreateConfigurationMenu(player, menu_symbol, "$TxtConfigureTeams$");
	
	var item = 0;
	
	for (var i = 0; i < GetTeamCount(); i++)
	{
		var team = GetTeamByIndex(i);
		var team_name = GetTeamName(team);
		
		for (var p = 0; p < GetTeamPlayerCount(team); p++)
		{
			var index = GetTeamPlayer(team, p);
			
			if (index >= 0)
			{
				player->AddMenuItem(Format("%s (%s)", GetTaggedPlayerName(index), team_name), Format("MenuSwitchTeam(Object(%d), %d)", player->ObjectNumber(), index), Rule_TeamAccount);
				
				if (selection != nil && index == selection)
				{
					player->SelectMenuItem(item);
				}
				
				item++;
			}
		}
	}

	MenuAddItemReturn(player);
}

/*-----------------------------------------------------------------------------------------------------------------------------------------

  Menu actions
  
  -----------------------------------------------------------------------------------------------------------------------------------------*/

protected func ChangeRuleConf(id menu_symbol, object player, int i)
{
	var rule_info = GetProperty(GetProperties(configuration_rules)[i], configuration_rules);

	rule_info.is_active = !rule_info.is_active;
  
	MenuConfigureRules(menu_symbol, player, i);
}

protected func ChangeWinScore(id menu_symbol, object player, int selection, int change)
{
	configured_goal->~DoWinScore(change);
	
	MenuConfigureGoal(menu_symbol, player, selection);
}

protected func ConfigurationFinished(id menu_symbol, parameter)
{
	this->~OnCloseMainMenu();
	
	configuration_finished = true;
	
	SetupGoal();
	CreateRules();
	ReleasePlayers(true);
	
	GameCallEx("OnConfigurationEnd", this);
	
	RoundManager()->RemoveRoundStartBlocker(this);
}

protected func ConfigureItemSlot(id menu_symbol, object player, string key, int selection, bool configure_spawnpoint, id item, int index)
{
	var current_config = GetItemConfiguration(key);
	
	if (current_config == nil)
	{
		current_config = {};
	}
	
	SetProperty(GAMECONFIG_Proplist_Def, item, current_config);
	SetItemConfiguration(key, current_config);

	MenuConfigureItemSlot(menu_symbol, player, key, selection, configure_spawnpoint, index);
}

protected func CreateGoal(id goal_id)
{
	var goal = CreateObject(goal_id);
	
	if (goal != nil)
	{
		configured_goal = goal;
	}
}

protected func CreateRules()
{
	var keys = GetProperties(configuration_rules);
	
	for (var i = 0; i < GetLength(keys); i++)
	{
		var rule_info = GetProperty(keys[i], configuration_rules);
		
		RemoveAll(Find_ID(rule_info.def));
		
		if (rule_info.is_active)
		{
			var rule = CreateObject(rule_info.def);
			
			rule->~Configure(rule_info.settings);
			
			rule_info.instances = [rule];
		}
	}
}

protected func PreconfigureRules()
{
	var preconfigured_rules = this->~GetDefaultRules();
	if (preconfigured_rules == nil)
		return;
		
	
	for (var rule_id in preconfigured_rules)
	{
		var prop = GetProperty(Format("%v", rule_id), configuration_rules);
		if (prop != nil)
		{
			prop.is_active = true;
			Log("Preconfiguring rule %v", rule_id);
		}
	}
}

protected func ScanRules()
{
	for (var i = 0, rule_id; rule_id = GetDefinition(i); i++)
	{
		if ((rule_id->GetCategory() & C4D_Rule) && (rule_id->~GameConfigIsChoosable()))
		{
			var rule_proplist = {};

			rule_proplist.def = rule_id;
			rule_proplist.instances = FindObjects(Find_ID(rule_id));
			rule_proplist.is_active = GetLength(rule_proplist.instances) > 0;
			
			var dummy = CreateObject(Dummy);
			dummy->SetGraphics(nil, rule_id, 0, GFXOV_MODE_Picture);
			
			rule_proplist.symbol = dummy;

			SetProperty(Format("%i", rule_id), rule_proplist, configuration_rules);
		}
	}
}

protected func ScanSpawnPoints()
{
	spawnpoint_keys = [];
	var points = FindObjects(Find_Func("IsSpawnPoint"));
	
	if (this->~GetDefaultItemConfigurations())
	{
		configured_items = this->~GetDefaultItemConfigurations()[0];
	}
	
	for (var spawnpoint in points)
	{
		var key = spawnpoint->GetIDParameter();
		
		if (GetType(key) == C4V_String)
		{
			if (IsValueInArray(spawnpoint_keys, key)) continue;
			
			PushFront(spawnpoint_keys, key);
			
			var current_config = GetItemConfiguration(key);
	
			if (current_config == nil)
			{
				current_config = {};
			}
			
			SetProperty(GAMECONFIG_Proplist_Desc, spawnpoint->~GetDescription(), current_config);
			SetItemConfiguration(key, current_config);
		}
	}

}

private func ChangeBotAmount(id menu_symbol, object player, int selection, int change)
{
	var amount = Abs(change);
	var delay;
	
	if (change > 0)
	{
		// create a new ai player
		while(amount > 0)
		{
			amount--;
			
			var color = HSL(Random(16) * 16, RandomX(200, 255), RandomX(100, 150));
			CreateScriptPlayer("$BotName$", color);
		}
		
		delay = 3;
	}
	else
	{
		// eliminate the latest created ai players
		for(var count = GetPlayerCount(C4PT_Script); count > 0 && amount > 0; count = GetPlayerCount(C4PT_Script))
		{
			amount--;
			EliminatePlayer(GetPlayerByIndex(count - 1, C4PT_Script));
		}
		
		delay = 80;
	}

	// opens the menu right away, but updates the number
	// as soon as a bot joins (waiting_for_bot_to_join)
	MenuConfigureBots(GAMECONFIG_Icon_Bots, player, selection, true);
	ScheduleCall(this, "MenuConfigureBots", delay, 0, GAMECONFIG_Icon_Bots, player, selection);
}

private func ConfigureItemSet(id menu_symbol, object player, int selection)
{
	var configurations = this->~GetDefaultItemConfigurations();
	if (configurations != nil)
	{
		var configuration = configurations[selection];
		Log("Set configuration to %s", configured_items.name);
		
		configured_items.key = configuration.key;
		configured_items.name = configuration.name;
		configured_items.icon = configuration.items;
		
		for (var key in spawnpoint_keys)
		{
			var update = GetItemConfiguration(key);
			var source = GetItemConfiguration(key, configuration);
		
			for (var prop in GetProperties(source))
			{
				SetProperty(prop, GetProperty(prop, source), update);
			}
		
			SetItemConfiguration(key, update);
		}
	}
	
	MenuConfigureItems(GAMECONFIG_Icon_Items, player, selection);
}

private func CreateConfigurationMenu(object player, id menu_symbol, string caption)
{
	player->CloseMenu();
	player->CreateMenu(menu_symbol, this, nil, caption, nil, C4MN_Style_Context);
}

private func MenuAddItemReturn(object player)
{
	player->AddMenuItem("$Finished$", "OpenMainMenu()", Icon_Ok, nil, nil, "$Finished$");
}

private func ChooseGoal(id menu_symbol, id goal, object player, int selection)
{
	selected_goals[selection] = !selected_goals[selection];

	MenuChooseGoal(menu_symbol, player, selection);
}

private func SetupGoal()
{
	var goals = this->~GetAvailableGoals();

	if (!configured_goal && goals)
	{
		var multiple = [];
		
		for(var i = 0; i < GetLength(goals); i++)
		{
			if (selected_goals[i])
			{
					PushBack(multiple, goals[i]);
			}
		}
		
		var selected;
		// create one of the chosen goals randomly	
		if (GetLength(multiple))
		{
			selected = multiple[Random(GetLength(multiple))];
		}
		// or one of the available goals, if no goal was chosen
		else
		{
			selected = goals[Random(GetLength(goals))];
		}
			
		CreateGoal(selected);
	}

	if (!configuration_finished) OpenMainMenu();
}

private func MenuSwitchTeam(object player, int index)
{
	var player_nr = index; //GetPlayerByIndex(index);

	var team = GetPlayerTeam(player_nr);
	if(GetTeamName(GetTeamByIndex(team)))
	{
		team = GetTeamByIndex(team);
	}
	else
	{
		team = GetTeamByIndex(0);
	}

	SetPlayerTeam(player_nr, team);

	MenuConfigureTeams(GAMECONFIG_Icon_Teams, player, index);
}

/*-----------------------------------------------------------------------------------------------------------------------------------------

  Player and crew actions
  
  -----------------------------------------------------------------------------------------------------------------------------------------*/

/**
 Gets the player who will configure the round. 
 @return object The currently choosing player.
 @version 0.1.0
 */
public func GetChoosingPlayer()
{
	return GetCursor(GetPlayerByIndex(player_index, C4PT_User));
}


/**
 Puts all players in relaunch containers.
 @version 0.1.0
 */
public func ContainPlayers()
{
	for (var i = 0; i < GetPlayerCount(); i++)
	{
		ContainPlayer(GetPlayerByIndex(i));
	}
}

/**
 Puts a single player in a relaunch container.
 @par player The player index.
 @version 0.1.0
 */
public func ContainPlayer(int player)
{
	for (var i = 0; i < GetCrewCount(player); i++)
	{
		ContainCrew(GetCrew(player, i));
	}
}

/**
 Puts a single clonk in a relaunch container.
 @par crew The player's clonk.
 @version 0.1.0
 */
public func ContainCrew(object crew)
{
	var container = CreateObject(RelaunchContainerEx, crew->GetX() - GetX(), crew->GetY() - GetY());
	container->PrepareRelaunch(crew);
}

/**
 Releases all players from their relaunch containers.
 @instant If {@c true}, then the relaunch container exits the player immediately.
 */
public func ReleasePlayers(bool instant)
{
	for (var i = 0; i < GetPlayerCount(); i++)
	{
		ReleasePlayer(GetPlayerByIndex(i), instant);
	}
}

/**
 Releases a single players from their relaunch containers.
 @instant If {@c true}, then the relaunch container exits the player immediately.
 */
public func ReleasePlayer(int player, bool instant)
{
	for (var i = 0; i < GetCrewCount(player); i++)
	{
		ReleaseCrew(GetCrew(player, i), instant);
	}
}

/**
 Releases a clonk from his owner's service, relaunches the Clonk
 @instant If {@c true}, then the relaunch container exits the player immediately.
 */
public func ReleaseCrew(object crew, bool instant)
{
	var container = crew->Contained();
	
	if ((container != nil) && (container->GetID() == RelaunchContainerEx))
	{
		if (instant)
		{
			container->InstantRelaunch();
		}
		else
		{
			container->StartRelaunch(crew);
		}
	}
}

/**
 Interface for the spawn points, so that the configuration can be looked up.
 @return id The definition that is configured for the spawn point.
 */
public func GetSpawnPointItem(string key)
{
	return GetProperty(GAMECONFIG_Proplist_Def, GetProperty(key, GetProperty(GAMECONFIG_Proplist_Items, configured_items)));
}

private func GetItemConfiguration(string key, proplist configuration)
{
	if (configuration == nil) configuration = configured_items;
	
	return GetProperty(key, GetProperty(GAMECONFIG_Proplist_Items, configuration));
}

private func SetItemConfiguration(string key, proplist value)
{
	return SetProperty(key, value, GetProperty(GAMECONFIG_Proplist_Items, configured_items));
}