/*--
	Capture The Flag
	Author: Maikel
	
	Capture the flag of the opposing faction and bring it to your base to gain points.
--*/


#include Library_Goal

local score_list_points;
local score_list_rounds;

local win_score;

local is_fulfilled;


public func DoScore(int faction, int change)
{
	AssertArrayBounds(score_list_points, faction);

	score_list_points[faction] += Max(0, change);
	
	if (score_list_points[faction] >= win_score)
	{
		DoWinRound(faction);
	}

	return;
}

public func DoRoundScore(int faction, int change)
{
	AssertArrayBounds(score_list_rounds, faction);
	
	score_list_rounds[faction] += Max(0, change);
	return;
}

public func GetScore(int faction)
{
	AssertArrayBounds(score_list_points, faction);

	return score_list_points[faction];
}

public func GetRoundScore(int faction)
{
	AssertArrayBounds(score_list_rounds, faction);

	return score_list_rounds[faction];
}

public func GetWinScore()
{
	return win_score;
}

public func SetWinScore(int score)
{
	win_score = Max(1, score);
}

public func DoWinScore(int change)
{
	SetWinScore(GetWinScore() + change);
}

public func DoWinRound(int faction)
{
	if (RoundManager() != nil)
	{
		RoundManager()->RemoveRoundEndBlocker(this);
	}
	else
	{
		is_fulfilled = true;
	}
}

protected func Initialize()
{
	score_list_points = [];
	score_list_rounds = [];
	
	is_fulfilled = false;
	
	var round_number = 1;
	
	if (RoundManager() != nil)
	{
		RoundManager()->RegisterRoundEndBlocker(this);
		round_number = RoundManager()->GetRoundNumber();
	}

	this->~DoSetup(round_number);

	return _inherited(...);
}

public func IsFulfilled() { return is_fulfilled; }

public func OnRoundStart(int round)
{
	this->~DoSetup(round);
}

public func Destruction()
{
	var round_number = 1;
	
	if (RoundManager() != nil)
	{
		round_number = RoundManager()->GetRoundNumber();
	}

	this->~DoCleanup(round_number);
}

local Name = "$Name$";
