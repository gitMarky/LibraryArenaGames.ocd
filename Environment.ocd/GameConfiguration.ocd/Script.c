/**
 Allows the player to configure the game.
 @author Marky
 @version 0.1.0
 */

static const GAMECONFIG_Proplist_Items = "items";
static const GAMECONFIG_Proplist_Def = "def";
static const GAMECONFIG_Proplist_Desc = "description";

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

local configured_goal;			// object: the goal that has been created
local player_index;				// int: the player that configures the current round

local configuration_finished;	// bool: true once the configuration is done

local spawnpoint_keys; 				// array: contains spawnpoint keys.

local color_conflict = -6946816; // = RGB(150, 0, 0);
local color_inactive = -4934476; // = RGB(180, 180, 180);
local color_active = -1; // = RGB(255, 255, 255);


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
	configured_items = { name = "default", icon = GetID(), items = {}};
	player_index = 0;
	
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

/*-----------------------------------------------------------------------------------------------------------------------------------------

  Main menu
  
  -----------------------------------------------------------------------------------------------------------------------------------------*/

/**
 Opens the configuration menu in the choosing player and closes menus in other players.
 Calls {@link Environment_Configuration#CreateMainMenu} and has a callback {@c OnOpenMainMenu()}
 for custom effects.
 
 @version 0.1.0
 */
protected func OpenMainMenu(id dummy)
{
	var player = GetChoosingPlayer();
	if (!player) return ScheduleCall(this, "OpenMainMenu", 1);
	
	for (var i = 0; i < GetPlayerCount(); i++)
	{
		var cursor = GetCursor(GetPlayerByIndex(i));
		if (cursor->GetMenu())
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

	MainMenuAddItemGoal(player, Goal_Random);
	MainMenuAddItemWinScore(player);
	MainMenuAddItemRules(player);
	MainMenuAddItemTeams(player);
	MainMenuAddItemBots(player);
	MainMenuAddItemItems(player);
	
	this->~MainMenuAddItemCustom(player);

	MainMenuAddItemFinishConfiguration(player);
}

/**
 Adds an option for choosing a goal for the scenario. Behaviour depens on the callback@br
 {@c GetAvailableGoals()}, which should return an array of definitions:@br
 - It does nothing if the function returns {@c nil}. This is ideal if you already have a goal in the scenario.
   Should there be such a goal, then it will not be configurable with {@link Enviroment_Configuration#MainMenuAddItemWinScore}@br
 - It creates the only available goal if the array has exactly one entry. It will be configurable with {@link Enviroment_Configuration#MainMenuAddItemWinScore}@br
 - It creates a menu with the available goals and has the player choose one goal.
 
 @par player The menu is displayed in this object.
 @par menu_symbol The menu has this icon.
 @version 0.1.0
 */
protected func MainMenuAddItemGoal(object player, id menu_symbol)
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
		player->AddMenuItem("$TxtConfigureGoals$", "MenuChooseGoal", menu_symbol, nil, player);
	}
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
 
 @par player The menu is displayed in this object.
 @par menu_symbol The menu has this icon.
 @version 0.1.0
 */
protected func MenuChooseGoal(id menu_symbol, object player)
{
	//var player = GetChoosingPlayer();
	var goals = this->~GetAvailableGoals();
	if (!player || !goals) return ScheduleCall(this, "OpenMainMenu", 1);

	CreateConfigurationMenu(player, menu_symbol, "$TxtConfigureGoals$");
	
	for (var goal in goals)
	{
		player->AddMenuItem(goal->GetName(), "MenuSetGoal", goal);
	}

	this->~MenuChooseGoalCustomEntries(player);
}

/**
 Opens a menu for changing the win score of goals.
 
 @par player The menu is displayed in this object.
 @par menu_symbol The menu has this icon. This should be the id of the goal.
 @par selection This entry will be selected. Makes choosing the same option several times a lot more easy.
 @version 0.1.0
 */
protected func MenuConfigureGoal(id menu_symbol, object player, int selection)
{
	CreateConfigurationMenu(player, menu_symbol, menu_symbol->GetName());
	
	player->AddMenuItem(" ", Format("MenuConfigureGoal(%i, Object(%d), %d)", menu_symbol, player->ObjectNumber(), 0), menu_symbol, configured_goal->~GetWinScore());
	player->AddMenuItem("$MoreWinScore$", Format("ChangeWinScore(%i, Object(%d), %d, %d)", menu_symbol, player->ObjectNumber(), 1, +1), Icon_Plus, nil, nil, "$MoreWinScore$");
	player->AddMenuItem("$LessWinScore$", Format("ChangeWinScore(%i, Object(%d), %d, %d)", menu_symbol, player->ObjectNumber(), 2, -1), Icon_Minus, nil, nil, "$LessWinScore$");

	MenuAddItemReturn(player);

	player->SelectMenuItem(selection);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// non-functional and temporary stuff




protected func ConfigurationFinished()
{
	this->~OnCloseMainMenu();
	
	CreateRules();
	ReleasePlayers(true);
	
	GameCallEx("OnConfigurationEnd", this);
	
	RoundManager()->RemoveRoundStartBlocker(this);
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

protected func MainMenuAddItemRules(object player)
{
	// do nothing if no rules are configurable
	if (GetLength(GetProperties(configuration_rules)) > 0)
		player->AddMenuItem("$TxtConfigureRules$", "MenuConfigureRules", GetID(), nil, player);
}

protected func MainMenuAddItemTeams(object player)
{
	// do nothing if the goal is not a team goal
	if (configured_goal != nil && !(configured_goal->~IsTeamGoal())) return;
	
	player->AddMenuItem("$TxtConfigureTeams$", "MenuConfigureTeams", GetID(), nil, player);
}

protected func MainMenuAddItemBots(object player)
{
	if (CanConfigureBots())
	{
		player->AddMenuItem("$TxtConfigureBots$", "MenuConfigureBots", GetID(), nil, player);
	}
}

protected func MainMenuAddItemItems(object player)
{
	if (CanConfigureSpawnPoints())
	{
		player->AddMenuItem("$TxtConfigureItems$", "MenuConfigureItems", GetID(), nil, player);
	}
}

protected func MenuConfigureRules(id menu_symbol, object player, int selection)
{
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

protected func MenuConfigureTeams(id menu_symbol, object player, int selection)
{
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


protected func MenuConfigureBots(id menu_symbol, object player, int selection, bool block_interaction)
{
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
		
		caption0 = ColorizeString(caption0, color_inactive);
		caption1 = ColorizeString(caption1, color_inactive);
		caption2 = ColorizeString(caption2, color_inactive);
		caption3 = ColorizeString(caption3, color_inactive);
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

protected func MenuConfigureItems(id menu_symbol, object player, int selection)
{	
	var configurations = this->~GetDefaultItemConfigurations();
	if (configurations != nil)
	{
		CreateConfigurationMenu(player, menu_symbol, "$TxtConfigureItems$");

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

			player->AddMenuItem(caption, Format("ConfigureItemSet(%i, Object(%d), %d)", menu_symbol, player->ObjectNumber(), i), config.icon);
		}
		
		player->AddMenuItem("$TxtConfigureSpecificItems$", Format("MenuConfigureItemsCustom(%i, Object(%d), 0, true)", menu_symbol, player->ObjectNumber()), menu_symbol);	

		MenuAddItemReturn(player);
		
		player->SelectMenuItem(selection);
	}
	else
	{
		// open the custom menu right away
		MenuConfigureItemsCustom(menu_symbol, player);
	}
}

protected func MenuConfigureItemsCustom(id menu_symbol, object player, int selection, bool has_default_configurations)
{
	CreateConfigurationMenu(player, GetID(), "$TxtConfigureSpecificItems$");

	// spawn points
		
	var selection_counter = 0;
	
	for (var type in spawnpoint_keys)
	{
		var current_config = GetItemConfiguration(type);
		var current_item = GetProperty(GAMECONFIG_Proplist_Def, current_config);
		var description = GetProperty(GAMECONFIG_Proplist_Desc, current_config);
	
		var command = Format("MenuConfigureItemSlot(%i, Object(%d), \"%s\", %d, true, 0)", menu_symbol, player->ObjectNumber(), type, selection_counter++);
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

protected func ChangeRuleConf(id menu_symbol, object player, int i)
{
	var rule_info = GetProperty(GetProperties(configuration_rules)[i], configuration_rules);

	rule_info.is_active = !rule_info.is_active;
  
	MenuConfigureRules(menu_symbol, player, i);
}

protected func CreateGoal(id goal_id)
{
	var goal = CreateObject(goal_id);
	
	if (goal != nil)
	{
		configured_goal = goal;
	}
}

protected func ChangeWinScore(id menu_symbol, object player, int selection, int change)
{
	configured_goal->~DoWinScore(change);
	
	MenuConfigureGoal(menu_symbol, player, selection);
}

public func GetChoosingPlayer()
{
	return GetCursor(GetPlayerByIndex(player_index, C4PT_User));
}

private func CreateConfigurationMenu(object player, id menu_symbol, string caption)
{
	player->CloseMenu();
	player->CreateMenu(menu_symbol, this, nil, caption, nil, C4MN_Style_Context);
}

private func MenuSetGoal(id goal)
{
	CreateGoal(goal);
	OpenMainMenu();
}

private func MenuAddItemReturn(object player)
{
	player->AddMenuItem("$Finished$", "OpenMainMenu()", Icon_Ok, nil, nil, "$Finished$");
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

	MenuConfigureTeams(GetID(), player, index);
}

public func InitializePlayer(int player, int x, int y, object base, int team, id extra_data)
{
	if (!configuration_finished)
	{
		ContainPlayer(player);
	}
}

public func ContainPlayers()
{
	for (var i = 0; i < GetPlayerCount(); i++)
	{
		ContainPlayer(GetPlayerByIndex(i));
	}
}

public func ContainPlayer(int player)
{
	for (var i = 0; i < GetCrewCount(player); i++)
	{
		ContainCrew(GetCrew(player, i));
	}
}

public func ContainCrew(object crew)
{
	var container = CreateObject(RelaunchContainerEx, crew->GetX() - GetX(), crew->GetY() - GetY());
	container->PrepareRelaunch(crew);
}

public func ReleasePlayers(bool instant)
{
	for (var i = 0; i < GetPlayerCount(); i++)
	{
		ReleasePlayer(GetPlayerByIndex(i), instant);
	}
}

public func ReleasePlayer(int player, bool instant)
{
	for (var i = 0; i < GetCrewCount(player); i++)
	{
		ReleaseCrew(GetCrew(player, i), instant);
	}
}

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

public func CanConfigureBots()
{
	return true;
}

public func CanConfigureSpawnPoints()
{
	return true;
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
	MenuConfigureBots(GetID(), player, selection, true);
	ScheduleCall(this, "MenuConfigureBots", delay, 0, GetID(), player, selection);
}

public func IsGameConfiguration()
{
	return true;
}

public func GetItemConfiguration(string key, proplist configuration)
{
	if (configuration == nil) configuration = configured_items;
	
	return GetProperty(key, GetProperty(GAMECONFIG_Proplist_Items, configuration));
}

public func GetSpawnPointItem(string key)
{
	return GetProperty(GAMECONFIG_Proplist_Def, GetProperty(key, GetProperty(GAMECONFIG_Proplist_Items, configured_items)));
}

public func SetItemConfiguration(string key, proplist value)
{
	return SetProperty(key, value, GetProperty(GAMECONFIG_Proplist_Items, configured_items));
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
	
	MenuConfigureItems(GetID(), player, selection);
}

protected func MenuConfigureItemSlot(id menu_symbol, object player, string type, int selection, bool configure_spawnpoint, int sel)
{
	var current_config = GetItemConfiguration(type);
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
		
		var command = Format("ConfigureItemSlot(%i, Object(%d), \"%s\", %d, %v, %i, %d)", menu_symbol, player->ObjectNumber(), type, selection, configure_spawnpoint, item, i);
		
		player->AddMenuItem(name, command, item);
	}
	
	player->AddMenuItem("$Finished$", Format("MenuConfigureItemsCustom(%i, Object(%d), %d, true)", menu_symbol, player->ObjectNumber(), selection), Icon_Ok, nil, nil, "$Finished$");

	player->SelectMenuItem(sel);
}

protected func ConfigureItemSlot(id menu_symbol, object player, string type, int selection, bool configure_spawnpoint, id item, int index)
{
	var current_config = GetItemConfiguration(type);
	
	if (current_config == nil)
	{
		current_config = {};
	}
	
	SetProperty(GAMECONFIG_Proplist_Def, item, current_config);
	SetItemConfiguration(type, current_config);

	MenuConfigureItemSlot(menu_symbol, player, type, selection, configure_spawnpoint, index);
}
