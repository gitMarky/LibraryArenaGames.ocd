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

	var goal = CreateObject(Test_Goal_ForPlayer);
	
	doTest("Default win score returned %d, should be %d", goal->GetWinScore(), 1);
	
	goal->SetWinScore(20);
	doTest("SetWinScore changes the score. Got %d, expected %d.", goal->GetWinScore(), 20);
	
	goal->DoWinScore(-1);
	doTest("DoWinScore changes the score. Got %d, expected %d.", goal->GetWinScore(), 19);
	
	goal->DoWinScore(5);
	doTest("DoWinScore changes the score. Got %d, expected %d.", goal->GetWinScore(), 24);

	return Evaluate();
}
