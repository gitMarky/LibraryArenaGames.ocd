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
	var factions = Max(1, this->~GetFactionCount());
	
	score_list_points = [];
	score_list_rounds = [];
	
	// fill the arrays. "<=" is correct, because the team numbers start at 1
	// in single player and team goals one goal will be a dummy, but I do not care :)
	for (var i = 0; i <= factions; i++)
	{
		PushBack(score_list_points, 0);
		PushBack(score_list_rounds, 0);
	}
	
	is_fulfilled = false;
	
	
	win_score = Max(1, this->~GetDefaultWinScore());
	
	if (RoundManager() != nil)
	{
		RoundManager()->RegisterRoundEndBlocker(this);
	}
	else
	{
		var round_number = 1;
		this->~DoSetup(round_number);
	}
	
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
