/*-- 
	Ruins
	Author: Mimmo_O
	
	An arena like last man standing round for up to 12 players.
--*/

static const RUINS_RAIN_PERIOD_TIME=3200;

protected func Initialize()
{
	CreateObject(Rule_KillLogs);
	CreateObject(Rule_Gravestones);
	
	// Mood.
	SetSkyAdjust(RGBa(255, 255, 255, 127), RGB(255, 200, 150));
	SetGamma(RGB(40, 35, 30), RGB(140, 135, 130), RGB(255, 250, 245));
	
	// Chests with weapons.
	var template_flint = CreateSpawnPoint(0, 0)->SetRespawnTimer(36*5)->SpawnItem(Firestone);

	CopySpawnPoint(template_flint, 230, 224)->SpawnItem("a");
	CopySpawnPoint(template_flint, 500,  64)->SpawnItem("b");
	CopySpawnPoint(template_flint, 124, 128)->SpawnItem("c");
	CopySpawnPoint(template_flint, 340, 440)->SpawnItem("d");
	
	template_flint->RemoveObject();
	
	var template_grass = CreateSpawnPoint(0, 0)->SetRespawnTimer(SPAWNPOINT_Timer_Infinite)->SpawnDeco(Grass);

	CopySpawnPoint(template_grass, 365, 346);
	CopySpawnPoint(template_grass, 370, 346);
	CopySpawnPoint(template_grass, 375, 346);
	CopySpawnPoint(template_grass, 380, 346);
	CopySpawnPoint(template_grass, 385, 346);
	CopySpawnPoint(template_grass, 390, 346);
	CopySpawnPoint(template_grass, 395, 346);
	CopySpawnPoint(template_grass, 400, 346);

	CopySpawnPoint(template_grass, 200, 123);
	CopySpawnPoint(template_grass, 245, 130);
	CopySpawnPoint(template_grass, 255, 130);

	CopySpawnPoint(template_grass, 385, 103);
	CopySpawnPoint(template_grass, 415,  81);
	CopySpawnPoint(template_grass, 465,  65);
	
	template_grass->RemoveObject();

	
	//for (var spawnpoint in FindObjects(Find_Func("IsSpawnPoint")))
	//{
	//	spawnpoint->OnRoundStart();
	//}
	
	// Ropeladders to get to the upper part.
	//CreateObject(Ropeladder, 380, 112, NO_OWNER)->Unroll(-1,0,19);
	//CreateObject(Ropeladder, 135, 135, NO_OWNER)->Unroll(1,0,16);
	
	// Objects fade after 5 seconds.
	//CreateObject(Rule_ObjectFade)->DoFadeTime(5 * 36);

	// Smooth brick edges.
	var x=[188, 205, 261, 244, 308, 325];
	var y=[124, 124, 132, 132, 108, 108];
	var d=[3, 2, 2, 3, 3, 2];
	for (var i = 0; i < GetLength(x); i++)
	{
		var edge=CreateObject(BrickEdge, x[i], y[i], NO_OWNER);
		edge->Initialize();
		edge->SetP(d[i]);
		edge->SetPosition(x[i],y[i]);
		edge->PermaEdge();
	}
	
	CreateObject(Environment_RoundManager);
	CreateObject(Environment_Configuration);
	CreateObject(RoundTester, 100, 100, NO_OWNER);
	
	AddEffect("DryTime",nil,100,2);
	return;
}

// Gamecall from LastManStanding goal, on respawning.
protected func OnPlayerRelaunch(int plr)
{
	var clonk = GetCrew(plr);
	var relaunch = CreateObject(RelaunchContainer, LandscapeWidth() / 2, LandscapeHeight() / 2, clonk->GetOwner());
	relaunch->StartRelaunch(clonk);
	return;
}


global func FxRainTimer(object pTarget, effect, int timer)
{
	if(timer<400)
	{
		InsertMaterial(Material("Water"),Random(LandscapeWidth()-60)+30,1,Random(7)-3,100+Random(100));
		return 1;
	} 
		for(var i=0; i<(6+Random(3)); i++)
	{
		InsertMaterial(Material("Water"),Random(LandscapeWidth()-60)+30,1,Random(7)-3,100+Random(100));
	}
	if(timer>(RUINS_RAIN_PERIOD_TIME+Random(800))) 
	{
	AddEffect("DryTime",nil,100,2);
	return -1;	
	}
	
	return 1;
}
global func FxDryTimeTimer(object pTarget, effect, int timer)
{
	if(timer<(380+Random(300))){
	InsertMaterial(Material("Water"),Random(LandscapeWidth()-60)+30,1,Random(7)-3,100+Random(100));
		return 1;
	}
	ExtractLiquidAmount(310+Random(50),430+Random(10),6+Random(4));
	
	if(!GBackLiquid(335,430))
	{
		//AddEffect("Rain",nil,100,2);
		return -1;
	}	
}

// GameCall from RelaunchContainer.
func OnClonkLeftRelaunch(object clonk)
{
	clonk->SetPosition(RandomX(200, LandscapeWidth() - 200), -20);
}

func KillsToRelaunch() { return 0; }
func RelaunchWeaponList() { return [Firestone]; }
