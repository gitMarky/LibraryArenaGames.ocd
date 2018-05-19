/**
	Unit test for the goal library
 */
 
static script_plr;

func Initialize()
{
	// Create a script player for some tests.
	script_plr = nil;
	CreateScriptPlayer("Buddy", RGB(0, 0, 255), nil, CSPF_NoEliminationCheck);
}


func InitializePlayer(int player)
{
	// Set zoom and move player to the middle of the scenario.
	SetPlayerZoomByViewRange(player, LandscapeWidth(), nil, PLRZOOM_Direct);
	SetFoW(false, player);
	GetCrew(player)->SetPosition(120, 190);
	GetCrew(player)->MakeInvincible();

	LaunchTest(1);
	return true;
}


func RemovePlayer(int player)
{
	// Remove script player.
	if (GetPlayerType(player) == C4PT_Script && player == script_plr)
	{
		script_plr = nil;
	}
}

/*-- Tests --*/

global func Test1_OnStart(int player){ return true;}
global func Test1_OnFinished(){ return; }
global func Test1_Execute()
{
	Log("Test the behaviour of *WinScore");

	// Preparation

	var goal = CreateObject(Test_Goal_ForPlayer);
	
	// Test
	
	doTest("Default win score returned %d, should be %d", goal->GetWinScore(), 1);
	
	goal->SetWinScore(20);
	doTest("SetWinScore changes the score. Got %d, expected %d.", goal->GetWinScore(), 20);
	
	goal->DoWinScore(-1);
	doTest("DoWinScore changes the score. Got %d, expected %d.", goal->GetWinScore(), 19);
	
	goal->DoWinScore(5);
	doTest("DoWinScore changes the score. Got %d, expected %d.", goal->GetWinScore(), 24);

	// Cleanup
	
	goal->RemoveObject();

	// Result
	return Evaluate();
}

global func Test2_OnStart(int player){ return true;}
global func Test2_OnFinished(){ return; }
global func Test2_Execute()
{
	Log("Test the behaviour of *FactionScore with player-based goal");

	// Preparation

	var goal = CreateObject(Test_Goal_ForPlayer);
	var win_score = 5;
	goal->SetWinScore(win_score);
	
	var player_a = GetPlayerByIndex(0, C4PT_User);
	var player_b = GetPlayerByIndex(0, C4PT_Script);
	
	Log("> Default is 0");
	doTestFactionScores(goal, player_a, 0, player_b, 0);
	
	Log("> Only the relevant score is updated");
	goal->DoFactionScore(player_a, 1);
	doTestFactionScores(goal, player_a, 1, player_b, 0);
	
	Log("> Negative values are ignored by default");
	goal->DoFactionScore(player_a, -2);
	doTestFactionScores(goal, player_a, 1, player_b, 0);
	
	Log("> Negative values are OK with parameter");
	goal->DoFactionScore(player_a, -2, true);
	doTestFactionScores(goal, player_a, -1, player_b, 0);
	
	Log("> Setting works, too");
	goal->SetFactionScore(player_b, 3);
	doTestFactionScores(goal, player_a, -1, player_b, 3);
	
	goal->SetFactionScore(player_b, -4);
	doTestFactionScores(goal, player_a, -1, player_b, -4);

	// Cleanup
	
	goal->RemoveObject();

	// Result
	return Evaluate();
}

global func Test3_OnStart(int player){ return true;}
global func Test3_OnFinished(){ return; }
global func Test3_Execute()
{
	Log("Test the behaviour of *FactionScore with team-based goal");

	// Preparation

	var goal = CreateObject(Test_Goal_ForTeam);
	var win_score = 5;
	goal->SetWinScore(win_score);
	
	var team_a = 1;
	var team_b = 2;
	
	goal->EnsureArraySize(2);
	
	Log("> Default is 0");
	doTestFactionScores(goal, team_a, 0, team_b, 0);
	
	Log("> Only the relevant score is updated");
	goal->DoFactionScore(team_a, 1);
	doTestFactionScores(goal, team_a, 1, team_b, 0);
	
	Log("> Negative values are ignored by default");
	goal->DoFactionScore(team_a, -2);
	doTestFactionScores(goal, team_a, 1, team_b, 0);
	
	Log("> Negative values are OK with parameter");
	goal->DoFactionScore(team_a, -2, true);
	doTestFactionScores(goal, team_a, -1, team_b, 0);
	
	Log("> Setting works, too");
	goal->SetFactionScore(team_b, 3);
	doTestFactionScores(goal, team_a, -1, team_b, 3);
	
	goal->SetFactionScore(team_b, -4);
	doTestFactionScores(goal, team_a, -1, team_b, -4);

	// Cleanup
	
	goal->RemoveObject();

	// Result
	return Evaluate();
}

global func Test4_OnStart(int player){ return true;}
global func Test4_OnFinished(){ return; }
global func Test4_Execute()
{
	Log("Test the behaviour of *Score with team-based goal");

	// Preparation

	var goal = CreateObject(Test_Goal_ForTeam);
	var win_score = 5;
	goal->SetWinScore(win_score);
	
	var player_a = GetPlayerByIndex(0, C4PT_User);
	var player_b = GetPlayerByIndex(0, C4PT_Script);
	
	goal->EnsureArraySize(2);
	
	doTest("Can set team of player A to 1, call returned %v, expected %v.", SetPlayerTeam(player_a, 1), true);
	doTest("Can set team of player B to 2, call returned %v, expected %v.", SetPlayerTeam(player_b, 2), true);
	
	Log("> Initial team assigment");
	doTest("Faction of player A is team %d, expected team %d.", goal->GetFactionByPlayer(player_a), 1);
	doTest("Faction of player B is team %d, expected team %d.", goal->GetFactionByPlayer(player_b), 2);
	
	Log("> Default is 0");
	doTestScores(goal, player_a, 0, player_b, 0);
	
	Log("> Only the relevant score is updated");
	goal->DoScore(player_a, 1);
	doTestScores(goal, player_a, 1, player_b, 0);
	
	Log("> Negative values are ignored by default");
	goal->DoScore(player_a, -2);
	doTestScores(goal, player_a, 1, player_b, 0);
	
	Log("> Negative values are OK with parameter");
	goal->DoScore(player_a, -2, true);
	doTestScores(goal, player_a, -1, player_b, 0);
	
	Log("> Setting works, too");
	goal->SetScore(player_b, 3);
	doTestScores(goal, player_a, -1, player_b, 3);
	
	goal->SetScore(player_b, -4);
	doTestScores(goal, player_a, -1, player_b, -4);
	
	
	Log("> Switch both players to the same team");

	doTest("Can set team of player B to 1, call returned %v, expected %v.", SetPlayerTeam(player_b, 1), true);
	doTestScores(goal, player_a, -1, player_b, -1);

	doTest("Can set team of player a to 2, call returned %v, expected %v.", SetPlayerTeam(player_a, 2), true);
	doTest("Can set team of player B to 2, call returned %v, expected %v.", SetPlayerTeam(player_b, 2), true);
	doTestScores(goal, player_a, -4, player_b, -4);

	// Cleanup
	
	goal->RemoveObject();

	// Result
	return Evaluate();
}

global func Test5_OnStart(int player){ return true;}
global func Test5_OnFinished(){ return; }
global func Test5_Execute()
{
	Log("Test the behaviour of GetFactionBy* and other minor things");

	// Preparation

	var goal_team = CreateObject(Test_Goal_ForTeam);
	goal_team->EnsureArraySize(2);
	var goal_player = CreateObject(Test_Goal_ForPlayer);
	
	var player_a = GetPlayerByIndex(0, C4PT_User);
	var player_b = GetPlayerByIndex(0, C4PT_Script);
	
	doTest("Can set team of player A to 1, call returned %v, expected %v.", SetPlayerTeam(player_a, 1), true);
	doTest("Can set team of player B to 2, call returned %v, expected %v.", SetPlayerTeam(player_b, 2), true);
	
	Log("> GetFactionByPlayer");
	doTest("Faction of player A is player %d, expected player %d.", goal_player->GetFactionByPlayer(player_a), GetPlayerID(player_a));
	doTest("Faction of player B is player %d, expected player %d.", goal_player->GetFactionByPlayer(player_b), GetPlayerID(player_b));
	doTest("Faction of player A is team %d, expected team %d.", goal_team->GetFactionByPlayer(player_a), 1);
	doTest("Faction of player B is team %d, expected team %d.", goal_team->GetFactionByPlayer(player_b), 2);


	Log("> GetFactionByIndex");
	doTest("Faction 0 is player %d, expected player %d.", goal_player->GetFactionByIndex(0), GetPlayerID(player_a));
	doTest("Faction 1 is player %d, expected player %d.", goal_player->GetFactionByIndex(1), GetPlayerID(player_b));
	doTest("Faction 0 is team %d, expected team %d.", goal_team->GetFactionByIndex(0), 1);
	doTest("Faction 1 is team %d, expected team %d.", goal_team->GetFactionByIndex(1), 2);
	
	Log("> GetFactionSize");
	doTest("Faction size for player A is %d, expected %d.", goal_player->GetFactionSize(1), 1);
	doTest("Faction size for player B is %d, expected %d.", goal_player->GetFactionSize(2), 1);
	doTest("Faction size for team A is %d, expected %d.", goal_team->GetFactionSize(1), 1);
	doTest("Faction size for team B is %d, expected %d.", goal_team->GetFactionSize(2), 1);
	
	doTest("Can set team of player B to 1, call returned %v, expected %v.", SetPlayerTeam(player_b, 1), true);
	doTest("Faction size for team A is %d, expected %d.", goal_team->GetFactionSize(1), 2);
	doTest("Faction size for team B is %d, expected %d.", goal_team->GetFactionSize(2), 0);

	// Cleanup
	
	goal_team->RemoveObject();
	goal_player->RemoveObject();

	// Result
	return Evaluate();
}

global func Test6_OnStart(int player){ return true;}
global func Test6_OnFinished(){ return; }
global func Test6_Execute()
{
	Log("Test the behaviour of singleton goal");

	// Preparation

	var goal_singleton = CreateObject(Test_Goal_Singleton);

	doTest("GetInstance() returned %v, expected %v.", Test_Goal_Singleton->GetInstance(), goal_singleton);
	
	var copy = CreateObject(Test_Goal_Singleton);
	doTest("Cannot create another goal object. Got %v, expected %v.", copy, nil);
	
	if (goal_singleton) goal_singleton->RemoveObject();
	
	var instance = Test_Goal_Singleton->GetInstance();

	doTest("GetInstance() returned %v, expected %v.", !!instance, true);

	// Cleanup
	
	RemoveAll(Find_ID(Test_Goal_Singleton));

	// Result
	return Evaluate();
}


global func Test7_OnStart(int player){ return true;}
global func Test7_OnFinished(){ return; }
global func Test7_Execute()
{
	Log("Test the behaviour of GetLeadingFaction for single player goal");

	// Preparation

	var goal = CreateObject(Test_Goal_ForPlayer);
	var win_score = 25;
	goal->SetWinScore(win_score);
	
	var player_a = GetPlayerID(GetPlayerByIndex(0, C4PT_User));
	var player_b = GetPlayerID(GetPlayerByIndex(0, C4PT_Script));
	
	doTest("Leading faction before any values are manipulated is %d, expected %d.", goal->GetLeadingFaction(), NO_OWNER);
	
	Log("> Take the lead with faction %d", player_a);
	goal->DoFactionScore(player_a, 1);
	doTestFactionScores(goal, player_a, 1, player_b, 0);
	doTest("Leading faction is %d, expected %d.", goal->GetLeadingFaction(), player_a);
	
	Log("> Go equal to the lead with faction %d, first faction should still have the lead", player_b);
	goal->DoFactionScore(player_b, 1);
	doTestFactionScores(goal, player_a, 1, player_b, 1);
	doTest("Leading faction is %d, expected %d.", goal->GetLeadingFaction(), player_a);
	
	Log("> Take the lead with faction %d", player_b);
	goal->DoFactionScore(player_b, 1);
	doTestFactionScores(goal, player_a, 1, player_b, 2);
	doTest("Leading faction is %d, expected %d.", goal->GetLeadingFaction(), player_b);
	
	Log("> Take the lead with faction %d, by setting the value", player_a);
	goal->SetFactionScore(player_a, 10);
	doTestFactionScores(goal, player_a, 10, player_b, 2);
	doTest("Leading faction is %d, expected %d.", goal->GetLeadingFaction(), player_a);

	Log("> Go equal to the lead with faction %d, by setting the value", player_b);
	goal->SetFactionScore(player_b, 10);
	doTestFactionScores(goal, player_a, 10, player_b, 10);
	doTest("Leading faction is %d, expected %d.", goal->GetLeadingFaction(), player_a);
	
	Log("> Reduce score of faction %d, best faction should take the lead", player_a);
	goal->DoFactionScore(player_a, -1, true);
	doTestFactionScores(goal, player_a, 9, player_b, 10);
	doTest("Leading faction is %d, expected %d.", goal->GetLeadingFaction(), player_b);
	
	Log("> Increase score of faction %d, it should take the lead", player_a);
	goal->DoFactionScore(player_a, 2);
	doTestFactionScores(goal, player_a, 11, player_b, 10);
	doTest("Leading faction is %d, expected %d.", goal->GetLeadingFaction(), player_a);
	
	Log("> Reduce score of faction %d, the other should take the lead", player_a);
	goal->DoFactionScore(player_a, -1, true);
	doTestFactionScores(goal, player_a, 10, player_b, 10);
	doTest("Leading faction is %d, expected %d.", goal->GetLeadingFaction(), player_b);

	// Cleanup
	
	goal->RemoveObject();

	// Result
	return Evaluate();
}

global func doTestFactionScores(object goal, int factionA, int score_factionA, int factionB, int score_factionB)
{
	doTest("Score for faction A is %d, expected %d.", goal->GetFactionScore(factionA), score_factionA);
	doTest("Score for faction B is %d, expected %d.", goal->GetFactionScore(factionB), score_factionB);
}


global func doTestScores(object goal, int playerA, int score_playerA, int playerB, int score_playerB)
{
	doTest("Score for player A is %d, expected %d.", goal->GetScore(playerA), score_playerA);
	doTest("Score for player B is %d, expected %d.", goal->GetScore(playerB), score_playerB);
}
