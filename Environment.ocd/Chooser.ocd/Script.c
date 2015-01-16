/*-- Spielzielauswahl --*/


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// definitions

local Name = "$Name$";
local Description = "$Description$";

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// global functions

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// finished functions

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// non-functional and temporary stuff

static g_iPlayerCount;
static g_bChooserFinished;
local aRulesNr;
local g_iRoundPlr;



// das bleibt immer offen
protected func MenuQueryCancel(){ return true; }

//C4D_Goal | C4D_Rule | C4D_Environment
static const Chooser_Cat = 524384;

/* Nur eine Mitteilung für Neugierige */

protected func Activate(iPlr)
{
	if(iPlr == GetPlayerByIndex(g_iRoundPlr, C4PT_User))
		return OpenMenu();
	MessageWindow(Format("$Choosing$", GetPlayerName(GetPlayerByIndex(g_iRoundPlr, C4PT_User))), iPlr);
}

/* Initialisierung */

local aRules, pGoal, aGoals, Death, iDarkCount, aAI;
local aTempGoalSave;

protected func Initialize()
{
	SetPosition();
	aRules = CreateArray();
	aRulesNr = CreateArray();
	aGoals = CreateArray();
	aTempGoalSave = CreateArray();
	aAI = CreateArray();
	ScheduleCall(this(), "RemoveGoals", 1);
	
	// Standard-Regelauswahl
	PreconficureRules();
	g_bChooserFinished = false;
	// Menu-Musik
	SetPlayList("UT_Menu.ogg");
}

protected func RemoveGoals()
{
  // Spielziele entfernen
  for(var goal in FindObjects(Find_Category(C4D_Goal)))
  {
    aGoals[GetLength(aGoals)] = GetID(goal);
    //TODO: aTempGoalSave[GetLength(aTempGoalSave)] = GetWinScore(goal);
    goal->RemoveObject();
  }
  // Einmal doof hacken (wo wir das hier ja sonst schon nicht gemacht haben °-°)
  OpenGoalChooseMenu();
  // Menü wird einfach nochmal geöffnet, diesmal richtig!
}

/* Spielerinitialisierung */

protected func InitializePlayer(int iPlr, int iX, int iY, object pBase, int iTeam, id idExtra)
{
	if(GetPlayerType(iPlr) == C4PT_Script)
	for(var i=1 ; i < aAI[iTeam] ; i++)
    	CreateClonk(iPlr);
	aAI[iTeam] = 0;
	if(Death) return;
	// Alle Clonks des Spielers verstauen
	for(var i=0, pCrew, tmp ; pCrew = GetCrew(iPlr, i) ; i++)
    {
		tmp = CreateObject(Rock, GetX(pCrew), GetY(pCrew), -1); // TODO: id was TIM1
		SetCategory(GetCategory(tmp) | C4D_Foreground,tmp);
		SetGraphics(0, tmp, GetID(pCrew), 1, 5, 0, 1, pCrew);
		Enter(tmp, pCrew);
		Eastern(tmp);

		//while(Contents(0, pCrew))
		//	RemoveObject(Contents(0, pCrew));

    }
	// Spieler 1? Dann Menü öffnen
	if(iPlr == g_iRoundPlr)
	{
		Log("$ChoosingPlayer$", GetPlayerName(iPlr));
		return(OpenMenu());
	}
}

private func CreateClonk(int iPlr)
{
	/*
  var bot = CreateObject(HZCK, 0,0, iPlr);
  MakeCrewMember(bot, iPlr);
  //GameCallEx("RelaunchPlayer", iPlr, bot, -1);
  GameCall("RelaunchPlayer", iPlr, bot, -1);
  */
  // TODO: AddBot(GetPlayerTeam(iPlr));
}

/* Hauptmenü */

protected func OpenMenu()
{
	if(GetLength(aGoals))
		return OpenGoalChooseMenu();
	ChooserSmallSelect();

	var pClonk = GetCursor(GetPlayerByIndex(g_iRoundPlr, C4PT_User));
	if(!pClonk)
		return ScheduleCall(this, "OpenMenu", 1);

	for(var i = 0; i < GetPlayerCount(); i++)
	{
		var clnk = GetCursor(GetPlayerByIndex(i));
		if(GetMenu(clnk))
			CloseMenu(clnk);
	}

	Message("", pClonk);

	CreateMenu(GetID(), pClonk, 0, 0, "$Chooser$", 0, 1);
	//Spielregeln
	if(!GetLeague())
		AddMenuItem("$CreateRules$", "OpenRuleMenu", Rock, pClonk, 0,0, "$RuleInfo$");

	//Spielziel
	if(GoalIsCompatible() && !GetLeague())
		AddMenuItem("%s", "OpenGoalMenu", GetID(pGoal), pClonk,0,0,"$GoalChose$");
	//Teameinteilung
	if(GetTeamCount() && !GetLeague())
		AddMenuItem("$TeamArrangement$", "OpenTeamMenu", Rock, pClonk, 0, 0, "$TeamInfo$");
	//KI
	if(ObjectCount(WAYP) && !GetLeague())
		AddMenuItem("$AIMenu$", "OpenAIMenu", Rock, pClonk, 0,0, "$AIInfo$");
	//Effekte
	//AddMenuItem("$Effects$", "OpenEffectMenu", EFMN, pClonk, 0,0, "$EffectInfo$");
	//Waffen
		AddMenuItem("$WeaponMenu$","OpenWeaponMenu",Rock,pClonk, 0,0, "$WeaponInfo$");
	// Sonstige Sachen
	OpenMenuCustomEntries( pClonk );
	//Fertig
	AddMenuItem("$Finished$", "ConfigurationFinished", Rock, pClonk,0,0,"$Finished$",2,3);
}

public func OpenMenuCustomEntries( object pClonk )
{
	DebugLog("OpenMenuCustomEntries1","chooser");
	_inherited( pClonk );
}

/* Regelmenü */

protected func OpenRuleMenu(id dummy, int iSelection)
{
  var pClonk = GetCursor();
  // Menü aufmachen
  CreateMenu(GetID(), pClonk);
  // Regeln einfügen
  for(var i=0, idR, def, j, check ; idR = GetDefinition(i, Chooser_Cat) ; i++)
    if(DefinitionCall(idR, "IsChooseable") && !GetLength(FindObjects(Find_ID(idR))))
      {
      def = CreateObject(idR, 0,0, -1);
      if(aRules[i])
        {
        SetGraphics("Chosen", def, Rock, 1, GFXOV_MODE_Picture);
        SetObjDrawTransform(650,0,5000,0,650,5000, def, 1);
        }
      AddMenuItem("%s", "ChangeRuleConf", idR, pClonk, 0, i, 0, 4, def);
      if(i == iSelection)
        check = true;
      if(!check)
        j++;
      RemoveObject(def);
      }
  // Fertig
  AddMenuItem("$Finished$", "OpenMenu", Rock, pClonk,0,0,0,2,3);
  // Menüeintrag auswählen
  SelectMenuItem(j, pClonk);
}

protected func ChangeRuleConf(id dummy, int i)
{
  // Regel wurde ausgewählt
  if(!aRules[i])
    aRules[i] = true;
  // Regel wurde abgewählt
  else
    aRules[i] = false;
  // Geräusch
  Sound("Grab", 1,0,0,1);
  // Regelmenü wieder aufmachen
  OpenRuleMenu(0, i);
}

/* Sondermenü: Dunkelheit */

protected func OpenDarknessMenu(id dummy, int iSelection)
{
	ChooserSmallSelect();
	  var pClonk = GetCursor();
	  // Menü aufmachen
	  CreateMenu(GetID(), pClonk, 0,0,"$DarknessChose$",0, 1);
	  // Anzeige
	  AddMenuItem("%s", "OpenDarknessMenu", Rock, pClonk, iDarkCount, 0, " ");
	  // Zähler erhöhen
	  AddMenuItem("$MoreDarkness$", "ChangeDARKConf", Rock, pClonk, 0, +1, "$MoreDarkness$",2,1);
	  // Zähler senken
	  AddMenuItem("$LessDarkness$", "ChangeDARKConf", Rock, pClonk, 0, -1, "$LessDarkness$",2,2);
	  // Fertig
	  AddMenuItem("$Finished$", "OpenMenu", Rock, pClonk,0,0, "$Finished$",2,3);
	  // Letzten Eintrag auswählen
	  SelectMenuItem(iSelection, pClonk);
}

protected func ChangeDARKConf(id dummy, int iChange)
{
  // Stand verändern
  iDarkCount = BoundBy(iDarkCount+iChange, 0, 10);
  FadeDarkness(iDarkCount,100);
  // Geräusch
  Sound("Grab", 1,0,0,1);
  // Menü wieder öffnen
  var iSel = 1;
  if(iChange == -1) iSel = 2;
  OpenDarknessMenu(0, iSel);
}

/* Spielzielmenü */

protected func OpenGoalChooseMenu()
{
	ChooserSmallSelect();
	var pClonk = GetCursor(GetPlayerByIndex(g_iRoundPlr, C4PT_User));
	if (!pClonk || !GetLength(aGoals))
		return ScheduleCall(this, "OpenMenu", 1);

	CloseMenu(pClonk);

	if(GetLength(aGoals) == 1)
		return CreateGoal(aGoals[0], aTempGoalSave[0]);

	CreateMenu(GetID(), pClonk, 0, 0, "$$", 0, 1);

	var i;
	for(var goal in aGoals)
	{
		AddMenuItem("%s", "CreateGoal", goal, pClonk, 0, aTempGoalSave[i]);
		i++;
	}
	//Zufall
	AddMenuItem("$RandomGoal$", "OpenGoalRandomMenu", GetID(), pClonk, 0, pClonk, "$RandomGoal$");
}



protected func CreateGoal(id idGoal, int iScore)
{
	//Spielziel erstellen
	var goal = CreateObject(idGoal, 0,0, -1);
	//Alten Wert setzen
	SetWinScore(iScore, goal);
	//Array leeren, damit das Menü nicht nochmal kommt
	aGoals = CreateArray();
	//Normales Menü öffnen
	OpenMenu();
}

private func GoalIsCompatible()
{
  // Schon eines gefunden?
  if(pGoal) return(1);
  // Mehr als ein Spielziel wird nicht unterstützt
  if(GetLength(FindObjects(Find_Category(C4D_Goal()))) != 1) return();
  // Ist das Spielziel kompatibel mit diesem Objekt?
  if(!(FindObject2(Find_Category(C4D_Goal()))->~IsConfigurable())) return();
  // Gut
  pGoal = FindObject2(Find_Category(C4D_Goal()));
  return(1);
}

protected func OpenGoalMenu(id dummy, int iSelection)
{
	ChooserSmallSelect();
	if(!pGoal)
		return OpenMenu();
	var pClonk = GetCursor(GetPlayerByIndex(g_iRoundPlr, C4PT_User));
	if(pGoal->~ConfigMenu(pClonk))
		return 1;
	//return _inherited(dummy, iSelection, ...);
	  if(!pGoal) return(OpenMenu());
	  var pClonk = GetCursor();
	  // Menü aufmachen
	  CreateMenu(GetID(), pClonk, 0,0,"$GoalChoose$",0, 1);
	  // Anzeige
	  AddMenuItem("$Score$", "OpenGoalMenu", GetID(pGoal), pClonk, GetWinScore2(), 0, " ");
	  // Zähler erhöhen
	  AddMenuItem("$MoreWinScore$", "ChangeGoalConf", Rock, pClonk, 0, +1, "$MoreWinScore$",2,1);
	  // Zähler senken
	  AddMenuItem("$LessWinScore$", "ChangeGoalConf", Rock, pClonk, 0, -1, "$LessWinScore$",2,2);
	  // Fertig
	  AddMenuItem("$Finished$", "OpenMenu", Rock, pClonk,0,0,"$Finished$",2,3);
	  // Letzten Eintrag auswählen
	  SelectMenuItem(iSelection, pClonk);
}

protected func ChangeGoalConf(id dummy, int iChange)
{
  // Stand verändern
  SetWinScore(GetWinScore(pGoal)+iChange,pGoal);
  if(GetWinScore(pGoal) <= 0) SetWinScore(1,pGoal);
  // Geräusch
  Sound("Grab", 1,0,0,1);
  // Menü wieder öffnen
  var iSel = 1;
  if(iChange == -1) iSel = 2;
  OpenGoalMenu(0, iSel);
}

private func GetWinScore2()
{
  var pGoal = FindObject2(Find_Category(C4D_Goal()));
  return(GetWinScore(pGoal));
}

/* Sondermenü: KI */

protected func OpenAIMenu(id dummy, int iSelection)
{
  var pClonk = GetCursor();
  var pGoal = FindObject2(Find_Category(C4D_Goal()));
  // Menü aufmachen
  CreateMenu(GetID(), pClonk, 0,0,0,0, 1);
  // Kein Teamspielziel

  if(!(pGoal->~IsTeamGoal()))
  {
    // Zahl
    AddMenuItem(" ", "OpenAIMenu", __EF, pClonk, 0, 0, " ");
    // Einer mehr!
    AddMenuItem("$OneAIMore$", "AIConfig(0,+1,1)", Rock, pClonk, 0,0, "$OneAIMore$",2,1);
    // Einer weniger!
    AddMenuItem("$OneAILess$", "AIConfig(0,-1,2)", Rock, pClonk, 0,0, "$OneAILess$",2,2);
  }
  else // Teamspielziel
  {
    // Doller Text
    KIMessage(pClonk);
    // Dolles Menü
    for(var i=1 ; GetTeamName(i) ; i++)
    {
      AddMenuItem(Format("%s: $OneAIMore$", GetTeamName(i)), Format("AIConfig(%d,+1,%d)", i,i*2-2), Rock, pClonk, 0,0, "$OneAIMore$",2,1);
      AddMenuItem(Format("%s: $OneAILess$", GetTeamName(i)), Format("AIConfig(%d,-1,%d)", i,i*2-1), Rock, pClonk, 0,0, "$OneAILess$",2,2);
    }
    if(GetLength(aAI) > i)
      while(i < GetLength(aAI))
      {
        AddMenuItem(Format("%s: $OneAIMore$", Format("Team %d", i)), Format("AIConfig(%d,+1,%d)", i,i*2-2), Rock, pClonk, 0,0, "$OneAIMore$",2,1);
        AddMenuItem(Format("%s: $OneAILess$", Format("Team %d", i)), Format("AIConfig(%d,-1,%d)", i,i*2-1), Rock, pClonk, 0,0, "$OneAILess$",2,2);
        i++;
      }
    // Autoteams -> Neue Teams einstellbar
    if(GetTeamConfig(TEAM_AutoGenerateTeams))
      AddMenuItem("$NewTeam$", Format("AIConfig(%d,+1,%d)", i, i*2), HZCK, pClonk, 0,0, "$NewTeam$");
    // Faire Teams
    AddMenuItem("$FairTeams$", "FairTeams", MEL2, pClonk, 0,0, "$FairTeamsInfo$");
  }
  // Fertig
  AddMenuItem("$Finished$", "OpenMenu", Rock, pClonk,0,0,"$Finished$",2,3);
  // Letzten Eintrag auswählen
  SelectMenuItem(iSelection, pClonk);
}

protected func AIConfig(int iTeam, int iChange, int iSel)
{
  // Kein Team
  if(iChange == +1) aAI[iTeam]++;
  if(iChange == -1 && iTeam <= GetLength(aAI))
  {
    if(aAI[iTeam] > 0)
      aAI[iTeam]--;
    if(iTeam == GetLength(aAI) && aAI[iTeam] <= 0)
    {
      for(var i=0, newArray = CreateArray() ; i < GetLength(aAI)-1 ; i++)
        newArray[i] = aAI[i];
      aAI = newArray;
    }
  }
  // Menü wieder öffnen (verzögert)
  ScheduleCall(this(), "OpenAIMenu", 2, 0, 0, iSel);
}

private func KIMessage(object pClonk)
{
  var msg = "@", j=0;
  for(var i in aAI)
  {
    if(i)
      msg = Format("%sTeam %d: %d|", msg, j, i);
    j++;
  }
  // Nachrichtn ausgeben
  PlayerMessage(pClonk->GetOwner(), msg, pClonk);
}

protected func FairTeams()
{
  // Rausfinden, welches Team am stärksten ist
  var maxTeam = Max(GetTeamCount(), GetLength(aAI)-1)+1;
  for(var i=1,str,mstr ; i < maxTeam ; i++)
  {
    str = GetTeamStrength(i);
    if(str > mstr)
      mstr = str;
  }
  // Andere Teams auffüllen
  for(i=1 ; i < maxTeam ; i++)
    while(GetTeamStrength(i) < mstr)
      aAI[i]++;
  // Menü
  OpenAIMenu();
}

protected func GetTeamCount()
{
  var i=1;
  while(GetTeamName(i))
    i++;
  return(i-1);
}

private func GetTeamStrength(int iTeam)
{
  if(!iTeam) return();

  for(var i=0,j,str ; i < GetPlayerCount() ; i++)
  {
    j = GetPlayerByIndex(i);
    if(GetPlayerTeam(j) == iTeam)
    {
      // KI
      if(GetPlayerType(j) == C4PT_Script) str += GetCrewCount(j);
      // Normaler Spieler
      else str++;
    }
  }
  if(iTeam < GetLength(aAI)) str += aAI[iTeam]; 
  return(str);
}

/* Konfiguration abschließen */

protected func ConfigurationFinished()
{
  // KI-Spieler erzeugen
  //var i=0;
  //for(var ai in aAI)
  //{
  //  if(ai)
  //    AddAI(i);
  //  i++;
  //}
  ScheduleCall(this(), "ConfigurationFinished2", 5);
}

/* Konfiguration abschließen */

protected func ConfigurationFinished2()
{
	Sound("BigSelect");
	Death = true;
	//Regeln erzeugen
	var i = 0, j = 0, pCrew, tmp, log, def;
	if (IsStandardSetting())
		log = "$StdRules$";
	else
		log = "$Rules$";
	for(var check in aRules)
	{
		if(check)
		{
			def = GetDefinition(i, Chooser_Cat);
			var k = Max(1,aRulesNr[i]);
			// aRulesNr Objekte erzeugen! Für MoreGore, etc.
			while( k )
			{
				k--;
				CreateObject(def, 10,10, -1);
			}
			if(j)
				log = Format("%s, <c %x>%s</c>", log, GetRuleColor(def), GetName(0, def));
			else
				log = Format("%s<c %x>%s</c>", log, GetRuleColor(def), GetName(0, def));
			j++;
		}
		i++;
	}
	//Dunkelheit erzeugen
	log = Format("%s, %s x%d", log, GetName(0, DARK), iDarkCount);

	//ein schneller GameCall für Einstellungen
	GameCallEx("ChooserFinished");

	//Spieler freilassen
	for(i = 0 ; i < GetPlayerCount() ; i++)
	{
		for(j = 0 ; pCrew = GetCrew(GetPlayerByIndex(i), j) ; j++)
		{
			tmp = Contained(pCrew);
			if(GetID(tmp) != TIM1) continue;
			RemoveObject(tmp, 1);
			pCrew->~Recruitment(GetOwner(pCrew));
		}
		for(var rule in FindObjects(Find_Category(Chooser_Cat), Find_Exclude(this)))
			rule->~InitializePlayer(GetPlayerByIndex(i));
	}
	//Selber entfernen
	RemoveObject();
}

/* Generelles */
private func Eastern(object P)
{
	var rand = Random(17);
	if(!rand) SetName(GetPlayerName(GetPlayerByIndex(Random(GetPlayerCount()))), P);
	if(rand == 1) SetName("$Emptiness$", P);
	if(rand == 2) SetName("$YourBrain$", P);
	if(rand == 3) SetName("$Nothing$", P);
	if(rand == 4) SetName("$Freezer$", P);
	if(rand == 5) SetName("$Space$", P);
	if(rand == 6) SetName("$YourGun$", P);
	if(rand == 7) SetName("$YourWallet$", P);
	if(rand == 8) SetName("$YourCookieJar$", P);
	if(rand == 9) SetName("$ThisWindow$", P);
	if(rand == 10) SetName("$YourStomach$", P);
	if(rand == 11) SetName("$EverythingEmpty$", P);
	if(rand == 12) SetName("$Barrel$", P);
	if(rand == 13) SetName("$Beer$", P);
	if(rand == 14) SetName("$YourGlance$", P);
	if(rand == 15) SetName("$YourHarddrive$", P);
	if(rand == 16) SetName("$YourChat$", P);
}

protected func PreconficureRules()
{
	var a = GameCall("ChooserRuleConfig");
	if(!GetLength(a))
		return;

	for(var i=0, idR; idR = GetDefinition(i, Chooser_Cat) ; i++)
	 if(DefinitionCall(idR, "IsChooseable") && !GetLength(FindObjects(Find_ID(idR))))
		 if(GetArrayItemPosition(idR,a) > -1)
			 aRules[i] = true;
}

public func GetRuleSelected( id idRule, bool bAmount, bool bIndex )
{
	var i, def;
	for(var check in aRules)
	{
		if(check)
		{
			def = GetDefinition(i, Chooser_Cat);
			if(def == idRule )
			{
				if( bIndex ) return i;
				if( bAmount ) return aRulesNr[i];
				else return true;
			}
		}
		i++;
	}

	return false;
}
private func IsStandardSetting()
{
	var a = GameCall("ChooserRuleConfig"), i;
	for (var i = 0; i < GetLength(aRules); i++) {
		if (GetIndexOf(GetDefinition(i, Chooser_Cat), a) != -1)
		{
			if (!aRules[i]) //Regel im Standardsatz, aber nicht ausgewählt
				return false;
		}
		else if (aRules[i]) //Regel ausgewählt, aber nicht Standard
			return false;
	}
	return true;
}

private func GetRuleColor(id idDef)
{
	if (GetIndexOf(idDef, GameCall("ChooserRuleConfig")) == -1)
		return RGB(255);
	return RGB(255,255,255);
}

local aGoalsChecked;

protected func OpenGoalRandomMenu(id id, object pClonk)
{
	ChooserSmallSelect();
	CreateMenu(GetID(), pClonk, 0, 0, "$$", 0, C4MN_Style_Context);
	if (!aGoalsChecked)
		aGoalsChecked = [];
	var fChecked = false;
	for (var i = 0; i < GetLength(aGoals); i++)
	{
		if (!aGoals[i])
			continue;
		if (aGoalsChecked[i])
		{
			AddMenuItem("%s", "CheckRandomGoal", aGoals[i], pClonk, 0, pClonk, GetDesc(0, aGoals[i]));
			fChecked = true;
		}
		else
			AddMenuItem("<c 777777>%s</c>", "CheckRandomGoal", aGoals[i], pClonk, 0, pClonk, GetDesc(0, aGoals[i]));
	}
	if (fChecked)
		AddMenuItem("$GoalRandomChoose$", "GoalRandomChoose", GetID(), pClonk, 0, pClonk, "$GoalRandomChoose$", 2, 3);
	else
		AddMenuItem("<c 777777>$GoalRandomChoose$</c>", 0, GetID(), pClonk, 0, pClonk, "$GoalRandomChoose$", 2, 3);
	AddMenuItem("$Back$", "OpenGoalChooseMenu", 0, pClonk, 0, pClonk, "$Back$");
}


protected func CheckRandomGoal(id idGoal, object pClonk)
{
	var iIndex = GetIndexOf(idGoal, aGoals);
	aGoalsChecked[iIndex] = !aGoalsChecked[iIndex];
	OpenGoalRandomMenu(0, pClonk);
	SelectMenuItem(iIndex, pClonk);
	Sound("Grab", true, 0, 0, GetOwner(pClonk) + 1);
}

protected func GoalRandomChoose(id id, object pClonk)
{
	var array = [];
	for (var i = 0; i < GetLength(aGoals); i++)
		if (aGoalsChecked[i])
			array[GetLength(array)] = aGoals[i];
	var idGoal = array[Random(GetLength(array))];
	if (!idGoal)
		return OpenGoalRandomMenu(0, pClonk);
	return CreateGoal(idGoal, aTempGoalSave[GetIndexOf(idGoal, aGoals)]);
}

public func ChooserSmallSelect()
{
	Sound("LittleSelect");
}