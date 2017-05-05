/**
 Library for a goal that is configurable in the {@link Environment_Configuration#MenuConfigureGoal}
 menu.
 @author Marky
 @credits Hazard Team
 @version 0.1.0
 */


#include Library_Goal

local score_list_points;
local score_list_rounds;

local win_score;

local is_fulfilled;

local is_inverted;

/**
 Changes the score of a faction for the current round.
 @par faction A player or team, by index.
 @par change The amount that the score should change.
 @par force_negative By default, negative changes are ignored.
                     Set this parameter to {@c true} if you want to
                     decrease the score.
 @version 0.1.0
 */
public func DoScore(int faction, int change, bool force_negative)
{
	AssertArrayBounds(score_list_points, faction);

	if (!force_negative)
	{
		score_list_points[faction] += Max(0, change);
	}
	else
	{
		score_list_points[faction] += change;
	}
	
	if (score_list_points[faction] >= win_score)
	{
		DoWinRound(faction);
	}
}

/**
 Increases the number of won rounds for a faction.
 @par faction A player or team, by index.
 @par change The amount that the score should increase by.
             Negative numbers are ignored.
 @version 0.1.0
 */
public func DoRoundScore(int faction, int change)
{
	AssertArrayBounds(score_list_rounds, faction);
	
	score_list_rounds[faction] += Max(0, change);
}

/**
 Gets the score of a faction, for the current round.
 @par faction A player or team, by index.
 @return int The score value.
 @version 0.1.0
 */
public func GetScore(int faction)
{
	AssertArrayBounds(score_list_points, faction);

	return score_list_points[faction];
}

/**
 Sets the score of a faction, for the current round.
 @par faction A player or team, by index.
 @version 0.1.0
 */
public func SetScore(int faction, int value)
{
	DoScore(faction, value - GetScore(faction), true);
}

/**
 Gets the number of rounds that a faction won.
 @par faction A player or team, by index.
 @return int The number of rounds that the faction won.
 @version 0.1.0
 */
public func GetRoundScore(int faction)
{
	AssertArrayBounds(score_list_rounds, faction);

	return score_list_rounds[faction];
}

/**
 Gets the number of points that a faction has to score in order to win.
 @return int If {@link Library_ConfigurableGoal#GetScore} of a faction is
             at least this number, then that faction wins the round.
 @version 0.1.0
 */
public func GetWinScore()
{
	return win_score;
}

/**
 Sets the number of points that a faction has to score in order to win.
 @par score No matter the input value the score is at least 1. If
            {@link Library_ConfigurableGoal#GetScore} of a faction is
            at least this number, then that faction wins the round.
 @version 0.1.0
 */
public func SetWinScore(int score)
{
	win_score = Max(1, score);
}

/**
 Increases the win score by the specified amount.
 @par change The win score changes by this amount.
             Can be a positive or negative value.
 @version 0.1.0
 */
public func DoWinScore(int change)
{
	SetWinScore(GetWinScore() + change);
}

/**
 Lets a faction win the current round.
 @par faction A player or team, by index.
 @version 0.1.0
 */
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
	is_inverted = false;
	
	
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
	this->DoSetup(round);
}

/**
 Callback. This is called by {@link Environment_RoundManager#DoRoundStart}.
 Does nothing by default.
 @version 0.1.0
 */
public func DoSetup(int round)
{
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

private func GetScoreMessage(int faction)
{
	var color = this->GetFactionColor(faction);
	var score = GetScore(faction);
	if (color)
		return Format("<c %x>%d</c>", color, score);
	else
		return Format("%d", score);
}

public func GetFactionCount()
{
	FatalError("Implement this in a derived object");
}

public func GetFactionByIndex(int index)
{
	FatalError("Implement this in a derived object");
}

public func GetFactionColor(int faction)
{
	FatalError("Implement this in a derived object");
}

private func GetFactionName(int faction)
{
	return "$DefaultFactionName$";
}


/**
 Determines the relative score of a faction,
 that is the faction score compared to the best score.
 */
private func GetRelativeScore(int faction)
{
	var not_initialized = -1;
	var best_faction = not_initialized, best_score = not_initialized;
	for(var i = 0; i < GetFactionCount(); ++i)
	{
		var current_faction = GetFactionByIndex(i);
		var score = GetScore(current_faction);
		if (current_faction != faction && ((score > best_score) || (best_faction == -1)))
		{
			best_faction = current_faction;
			best_score = score;
		}
	}
	
	var faction_score = GetScore(faction);
	
	// special case if there is only one player in the game
	if(best_faction == not_initialized)
	{
		best_faction = faction;
		best_score = faction_score;
	}
	
	return {
		best_faction = best_faction,
		best_score = best_score,
		relative_score = faction_score - best_score
	};
}


public func Activate(int by_player)
{
	var message = GetDescription(by_player);
	
	MessageWindow(message, by_player);
}


private func EnsureArraySize(int factions)
{
	for (var i = 0; i <= factions && GetLength(score_list_points) <= factions; i++)
	{
		PushBack(score_list_points, 0);
		PushBack(score_list_rounds, 0);
	}
}


public func GetGoalDescription(int faction)
{
	if(IsFulfilled()) 
	{
		if (GetScore(faction) >= GetWinScore())
		{
			return "$MsgVictory$";
		}
		else
		{
			return "$MsgLost$";
		}
	} 
	else 
	{
		var score = GetRelativeScore(faction);
		if (score.relative_score > 0)
		{
			return Format("$MsgAhead$",	 score.relative_score,  GetFactionName(score.best_faction));
		}
		else if (score.relative_score < 0)
		{
			return Format("$MsgBehind$", -score.relative_score, GetFactionName(score.best_faction));
		}
		else if (score.best_faction == faction)
		{
			return Format("$MsgYouAreBest$", score.relative_score);
		}
		else
		{
			return Format("$MsgEqual$", GetFactionName(score.best_faction));
		}
	}
}


local Name = "$Name$";
