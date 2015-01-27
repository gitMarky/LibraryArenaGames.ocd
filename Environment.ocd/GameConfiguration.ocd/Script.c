/**
 Allows the player to configure the game.
 @author Marky
 @version 0.1.0
 */


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// definitions

local Name = "$Name$";
local Description = "$Description$";


//static g_iPlayerCount;
static g_bChooserFinished;
local aRulesNr;
local g_iRoundPlr;

local is_rule_configured;		// array: index i == Definition i configured? true/false (TODO: has to become a proplist)

local configured_goal;			// object: the goal that has been created

local aGoals, Death, iDarkCount, aAI;
local aTempGoalSave;


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
 Initializes the local variables, then calls in order:@br
 1.) {@link Environment_Configuration#PreconfigureRules}@br
 2.) {@link Environment_Configuration#OnChooserInitialized}@br
 3.) {@link Environment_Configuration#OpenMenu}@br
 @version 0.1.0
 */
protected func Initialize()
{
	SetPosition();
	is_rule_configured = {};
	aRulesNr = [];
	aGoals = [];
	aTempGoalSave = [];
	aAI = [];
	g_bChooserFinished = false;
	g_iRoundPlr = 0;
	
	// wait for other rules, etc. to be initialized
	ScheduleCall(this, "PostInitialize", 1);
}
	
protected func PostInitialize()
{
	ScanRules();
	PreconficureRules();
	
	GameCallEx("OnConfigurationStart");
	
	if (RoundManager() == nil)
	{
		OpenMainMenu();
	}
}

public func OnRoundReset(int round_number)
{
	RoundManager()->RegisterRoundStartBlocker(this);
	
	// cycle the player who configures the round
	
	var players  = GetPlayerCount(C4PT_User);
	g_iRoundPlr = (round_number - 1) % players;
	
	OpenMainMenu();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// non-functional and temporary stuff



func OpenMainMenu()
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


func ConfigurationFinished()
{
	this->~OnCloseMainMenu();
	
	GameCallEx("OnConfigurationFinished");
	
	// TODO: Release players
	// TODO: InitializePlayer in rule objects
	
	RoundManager()->RemoveRoundStartBlocker(this);
	
	// RemoveObject();
}

protected func ScanRules()
{
	for (var i = 0, rule_id; rule_id = GetDefinition(i); i++)
	{
		if (rule_id->~GameConfigIsChooseable())
		{
			var rule_proplist = {};

			rule_proplist.instances = FindObjects(Find_ID(rule_id));
			rule_proplist.is_active = GetLength(rule_proplist.instances) > 0;

			SetProperty(Format("%i", rule_id), rule_proplist, is_rule_configured);
		}
	}
}


func PreconficureRules()
{
	var preconfigured_rules = this->~GetDefaultRules();
	if (preconfigured_rules == nil) // || !GetLength(preconfigured_rules))
		return;
		
	
	for (var rule_id in preconfigured_rules)
	{
		var prop = GetProperty(Format("%v", rule_id), preconfigured_rules);
		if (prop != nil)
		{
			prop.is_active = true;
			Log("Preconfiguring rule %v", rule_id);
		}
	}
	
	//for (var rule_proplist in GetProperties(is_rule_configured))
	//{
	//}
}

protected func CreateMainMenu(object player)
{
	CreateDefaultMenu(player, GetID(), "$MenuCaption$");

	CreateMainMenuGoal(player, Goal_Random);
	CreateMainMenuGoalConfig(player);
	CreateMainMenuRuleConfig(player);

	player->AddMenuItem("$Finished$", "ConfigurationFinished", Icon_Ok, nil, nil, "$Finished$");
}

protected func CreateMainMenuGoal(object player, id menu_symbol)
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

protected func CreateMainMenuGoalConfig(object player)
{
	// do nothing if we have a goal already
	if (configured_goal == nil) return;
	
	player->AddMenuItem(configured_goal->GetName(), "MenuConfigureGoal", configured_goal->GetID(), nil, player);
}

protected func CreateMainMenuRuleConfig(object player)
{
}

protected func CreateGoal(id goal_id)
{
	var goal = CreateObject(goal_id);
	
	if (goal != nil)
	{
		configured_goal = goal;
	}
}

protected func MenuChooseGoal(id menu_symbol, object player)
{
	//var player = GetChoosingPlayer();
	var goals = this->~GetAvailableGoals();
	if (!player || !goals) return ScheduleCall(this, "OpenMainMenu", 1);

	CreateDefaultMenu(player, menu_symbol, "$TxtConfigureGoals$");
	
	for (var goal in goals)
	{
		player->AddMenuItem(goal->GetName(), "MenuSetGoal", goal);
	}

	this->~MenuChooseGoalCustomEntries(player);
}

protected func MenuConfigureGoal(id menu_symbol, object player, int selection)
{
	CreateDefaultMenu(player, menu_symbol, menu_symbol->GetName());
	
	player->AddMenuItem(" ", Format("OpenGoalMenu(%i, Object(%d), %d)", menu_symbol, player->ObjectNumber(), 0), menu_symbol, 0);
	player->AddMenuItem("$MoreWinScore$", Format("ChangeWinScore(%i, Object(%d), %d, %d)", menu_symbol, player->ObjectNumber(), 1, +1), Environment_Configuration, nil, nil, "$MoreWinScore$", 2, 1);
	player->AddMenuItem("$LessWinScore$", Format("ChangeWinScore(%i, Object(%d), %d, %d)", menu_symbol, player->ObjectNumber(), 1, -1), Environment_Configuration, nil, nil, "$LessWinScore$", 2, 2);
	player->AddMenuItem("$Finished$", "OpenMainMenu", Icon_Ok, nil, nil, "$Finished$");
	player->SelectMenuItem(selection);
}

protected func ChangeWinScore(id menu_symbol, object player, int selection, int change)
{
	// TODO: actually change something
	
	// TODO: sound
	
	MenuConfigureGoal(menu_symbol, player, selection);
}

public func GetChoosingPlayer()
{
	return GetCursor(GetPlayerByIndex(g_iRoundPlr, C4PT_User));
}

private func CreateDefaultMenu(object player, id menu_symbol, string caption)
{
	player->CloseMenu();
	player->CreateMenu(menu_symbol, this, nil, caption, nil, C4MN_Style_Context);
}

private func MenuSetGoal(id goal)
{
	CreateGoal(goal);
	OpenMainMenu();
}
