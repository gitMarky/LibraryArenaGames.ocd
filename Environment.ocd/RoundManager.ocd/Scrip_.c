/*-- Standard Szenarioscript --*/

#strict 2

static g_iCountDown;
static g_bRoundRestarted;

/* Regelwähler */

public func ChooserFinished()
{
	// Klassenwahlzeug ohne das Objekt initialisieren
	if(GetType(aVitality) != C4V_Array) aVitality = [];
	if(GetType(aAgility) != C4V_Array) aAgility = [];
	if(GetType(aAccuracy) != C4V_Array) aAccuracy = [];

	if(GetType(aVoice) != C4V_Array) aVoice = [];
	if(GetType(aBlood) != C4V_Array) aBlood = [];
	if(GetType(aGender) != C4V_Array) aGender = [];

	g_bChooserFinished = true;
	g_bRoundRestarted = false;

	DebugLog("ChooserFinished","chooser");
	for(var i=0 ; i < GetPlayerCount() ; i++)
	{
		DebugLog("ChooserFinishedRelaunch","chooser");
		RelaunchPlayer(i, GetHiRank(i));
	}
		//GameCallEx("InitializePlayer", i, LandscapeWidth()/2, LandscapeHeight()/2, 0, iTeam);

	/*
	for(var clonk in FindObjects(Find_OCF(OCF_CrewMember)))
	{
		while(Contents(0, clonk))
			RemoveObject(Contents(0, clonk));

		CreateEquipmentFor( clonk);
	}
*/

	// Bei InstaGIB werden alle Spawnpunkte entfernt und die Waffen aus den Spielern entfernt
	if(FindObject(IGIB))
	{
		for(var spwn in FindObjects(Find_ID(SPNP)))
			RemoveObject(spwn);
		for(var clonk in FindObjects(Find_OCF(OCF_CrewMember)))
			while(Contents(0, clonk))
				RemoveObject(Contents(0, clonk));
	}
	// Bei Keine-Munition werden Munispawns entfernt
	if(FindObject(NOAM))
	{
		for(var spwn in FindObjects(Find_ID(SPNP)))
			if(Contents(0, spwn)->~IsAmmoPacket())
				RemoveObject(spwn);
	}
	// Bei Waffenwahl werden alle Nicht-Munispawns entfernt
	var wp = FindObject(WPCH);
	if(wp)
	{
		for(var spwn in FindObjects(Find_ID(SPNP)))
			if(!(Contents(0, spwn)->~IsAmmoPacket()))
				RemoveObject(spwn);

		// und begrenzte Waren...
		wp->RemoveWare();
		wp->AddWare(PIWP);	// Pistole
		wp->AddWare(PGWP);	// Pumpgun
		wp->AddWare(GLWP);	// Granatenwerfer
		wp->AddWare(MIWP);	// Minigun
		wp->AddWare(GGWP);	// Partikelkanone
		wp->AddWare(BZWP);	// Bazooka
		wp->AddWare(KLAS);	// Laser
		wp->AddWare(KRFL);	// Waffenteile
		wp->AddWare(FLSH);	// Taschenlampe
		wp->AddWare(MINE);	// Lasermine
		//wp->SortWare();
	}

	SetLocalPlayList();

	//AddEffect("StartRound",0,50,36);
	g_iCountDown = 10;
	var helper = CreateObject(_LSS,0,0,-1);
	ScheduleCall(helper,"CountdownRound",36,g_iCountDown+1);
	//Schedule("CountdownRound()",36,g_iCountDown+1);
}

public func CountdownRound()
{
	for( var i=0; i<GetPlayerCount(C4PT_User); i++)
	{
		var iPlr = GetPlayerByIndex(i,C4PT_User);
		var pCrew = GetHiRank(iPlr);

		if( g_iCountDown > 0)
		{
			UA_Announcer(Format("an_count_%d",g_iCountDown),pCrew,true);
			PlayerMessage(iPlr,Format("@<c %x>%d...</c>",RGB(200,0,0),g_iCountDown));
		}
		else
		{
			PlayerMessage(iPlr,"");
		}
	}
	if( g_iCountDown <= 0)
	{
		RemoveAll(_LSS);
	}
	g_iCountDown = BoundBy(g_iCountDown-1,-1,10);
}

/* Relaunch */

protected func InitializePlayer(int iPlr, int iX, int iY, object pBase, int iTeam)
{
	for(var i=0, pCrew ; pCrew = GetCrew(iPlr, i) ; i++)
		RelaunchPlayer(iPlr, pCrew, 0, iTeam);
}



/* Relaunch */

public func RelaunchPlayer(int iPlr, object pCrew, object pKiller, int iTeam)
{
	// Kein ordentlicher Spieler?
	if(GetOwner(pCrew) == NO_OWNER || iPlr == NO_OWNER || !GetPlayerName(iPlr))
		return;
	DebugLog("Relaunching","scenario");

	// Hazard-Hilfen aus
	SetPlrExtraData(iPlr, "Hazard_NoHelpMsg", true);

	// Kein Team
	if(!iTeam) iTeam = GetPlayerTeam(iPlr);

	//Reject?
	//if(!bFirst)
	//	if(RejectRelaunch(iPlr,iTeam))
	//		return;

	// Clonk tot?
	if(!GetAlive(pCrew))
		pCrew = RelaunchClonk(iPlr, pCrew);
	else
		DoEnergy(100,pCrew);

	// Zufallsposition
	var iX, iY;
	RelaunchPosition(iX, iY, iTeam);


		while(Contents(0, pCrew))
			RemoveObject(Contents(0, pCrew));

		CreateEquipmentFor( pCrew );

	//CreateEquipmentFor(pCrew);
	CreateHUD(pCrew);

	var repack = false, oldtim = Contained(pCrew);
	if(GetID(oldtim) == TIM1 )
	{
		if(!FindObject(CHOS) || g_bChooserFinished) repack = true;
	}
	if(!oldtim || repack)
	{
		DebugLog("Created Spawnpoint","scenario");
		var tim = CreateObject(TIM2, LandscapeWidth()/2, LandscapeHeight()/2, -1);
		pCrew->Enter(tim);

		if(oldtim) RemoveObject(oldtim);
	}

	if(Contained(pCrew))
	{
		DebugLog("AlreadyContained %i","scenario",GetID(Contained(pCrew)));
		SetPosition(iX, iY, Contained(pCrew));
	}
	else
		SetPosition(iX, iY, pCrew);
	// Fertig gerelauncht

	// KI-Objekt erstellen
	if(GetPlayerType(iPlr) == C4PT_Script )
	{
		//Log("Relaunching AI for Player %d %s",iPlr,GetPlayerName());
		if(!FindObjectOwner(__AI,iPlr))
		{
			  var ai = CreateObject(__AI, AbsX(0),AbsY(0), -1);
			  ai->~SetOwner(iPlr);
			  /*// ID zusammensetzen
			  var newID = Format("%d", ObjectCount(__AI)), oneID;
			  while(GetLength(newID) < 4)
				newID = Format("0%s", newID);
			  oneID = C4Id(newID);
			  ai->Init(oneID);*/
		}
	}

	if(!MOD_ClassSelection() && (g_bChooserFinished || !FindObject(CHOS)))
		GameCallEx("OnClassSelection",pCrew,0);
}

public func RelaunchClonk(int iPlr, object pCursor)
{
	var pClonk = CreateObject(ID_Player, 10, 10, iPlr);
	if(pCursor)
		GrabObjectInfo(pCursor, pClonk);
	else
		MakeCrewMember(pClonk, iPlr);

	DoEnergy(+150, pClonk);
	SetCursor(iPlr, pClonk);
	SetPlrView(iPlr, pClonk);


	// Wegstecken
	var tim = CreateObject(TIM2, LandscapeWidth()/2, LandscapeHeight()/2, -1);
	pClonk->Enter(tim);
	PlayerMessage(iPlr, Format("@%s", GetName(pClonk)), tim);

	return(pClonk);
}



/*
	//Effektmanager erstellen
	CreateObject(EFMN);

	//Ohne Klassenwahl
	if(!FindObject(MCSL))
	{
		for(var i = 0; i < GetPlayerCount(); i++)
			for(var j = 0, pCrew; pCrew = GetCrew(GetPlayerByIndex(i), j) ; j++)
				GameCallEx("OnClassSelection",pCrew);
	}

}
*/
/* Regelvoreinstellung */

func ChooserRuleConfig()
{
	//Standardregelsatz: Belohnungssystem, Kein FriendlyFire, Waffen bleiben, Arena, Limitierte Ausrüstung
	//Standardregelsatz: /*Klassensystem,*/ Ansager, kein Friendly Fire, Teamkennzeichung, Waffen bleiben
	var array = [ /*R_M3,*/ R_M6, NOFF, R_M9, WPST ];
	return array;
}

func LocalPlayList()
{
	return ["UT_Course.ogg","UT_Seeker.ogg","UT_Strider.ogg"];
}

func SetLocalPlayList()
{
	var list, str, done;
	for( str in LocalPlayList() )
	{
		if(!done)
		{
			done = true;
			list = str;
		}
		else
		{
			list = Format("%s;%s",list,str);
		}
	}
	SetPlayList(list);
}

public func RelaunchPosition(& iX, & iY, int iTeam)
{
	var wipf = PlaceAnimal(WIPF);
	iX = GetX(wipf);
	iY = GetY(wipf);
	RemoveObject(wipf);
}

/* Bei Clonkausrüstung */

public func CreateEquipmentFor(object pClonk)
{
	//Falls kein Clonk, nicht ausführen
	if(!pClonk)
		return;

	//Falls Clonk bereits ausgerüstet, nicht nochmal ausrüsten
	if(Contents(0,pClonk))
		return;

	var ammoIDs = [	ID_Ammo_Pistol,
					ID_Ammo_Shock,
					ID_Ammo_Missile,
					ID_Ammo_Grenade,
					ID_Ammo_Fuel,
					ID_Ammo_Bio,
					ID_Ammo_Ripper,
					ID_Ammo_Flak,
					ID_Ammo_Sniper,
					ID_Ammo_Pulse,
					ID_Ammo_Light ];

	var ammoID;
	for(ammoID in ammoIDs) SetAmmo(ammoID,0,pClonk);

	  // Lecker Waffen
	  if(!FindObject(IGIB) && g_bChooserFinished)
	  {
		  var melee = gWeaponPos01;
		  var gun = gWeaponPos02;

		  var obj;
		  if(gun)
		  {
			  obj = CreateContents(gun, pClonk);
			  //obj ->DoAmmo(obj ->GetFMData(FM_AmmoID), obj ->GetFMData(FM_AmmoLoad));
		  }
		  if(melee)
		  {
			  obj = CreateContents(melee, pClonk);
			  //obj ->DoAmmo(obj ->GetFMData(FM_AmmoID), obj ->GetFMData(FM_AmmoLoad));
		  }
	  }
}

/* Relaunch ablehnen */

public func RejectRelaunch(int iPlr, int iTeam)
{
	return false;
}

/* Spieler zuschauen lassen */

public func ForceObservation(int iPlr)
{
	if(GetCursor(iPlr)) SetPlrViewRange(0, GetCursor(iPlr));
	EliminatePlayer(iPlr);
}

/* Runde auswerten */

public func OnGameOver()
{
	//RewardEvaluation();
}

/* Spieler verlässt die Runde */

protected func RemovePlayer(iPlr)
{
	var pCursor = GetCursor(iPlr);
	if(!pCursor) return;
	if(pCursor->~GetRealCursor()) pCursor = pCursor->~GetRealCursor();
	if(!pCursor->~IsFakeDeath()) return;
	pCursor->Kill();
}


/* Zusatzfunktionen */

public func OnClassSelection( object pCrew, int iClass)
{
	if(!iClass) SelectNoClass( pCrew );
}

public func OnWeaponChoice()
{}



/* Runde wird neu gestartet */



static g_iRoundNr, g_iRoundPlr;

global func RestartRound()
{
	g_iRoundNr +=1;
	g_iRoundPlr +=1;

	if( g_iRoundPlr >= GetPlayerCount(C4PT_User)) g_iRoundPlr = 0;

	// unnötige Sachen löschen
	GameCallEx("OnRestartRound",-1);

	for(var i=0 ; i < GetPlayerCount() ; i++)
		GameCallEx("OnRestartRound",GetPlayerByIndex(i),GetPlayerTeam(GetPlayerByIndex(i)));

}




public func OnRestartRound( int iPlr, int iTeam )
{
	g_bRoundRestarted = true;
	if( iPlr == -1 )
	{
		// nicht benötigte Objekte löschen
		var aRemove = FindObjects(Find_Or( Find_Func("RemoveOnRestart"),
										   Find_Func("IsChooseable") ));
		//var aRemove = FindObjects(Find_Not( Find_ID( UBOT ) ));
		var pRemove;
		for( pRemove in aRemove ) RemoveObject(pRemove); //,1);

		GameCall("Initialize");
		CreateObject(CHOS,0,0,-1);
	}
	else
	{
		for(var i=0, pCrew ; pCrew = GetCrew(iPlr, i) ; i++)
		{
			while(Contents(0,pCrew)) RemoveObject(Contents(0,pCrew));
		}

		GameCallEx("InitializePlayer", iPlr, LandscapeWidth()/2, LandscapeHeight()/2, 0, iTeam);
	}

}
