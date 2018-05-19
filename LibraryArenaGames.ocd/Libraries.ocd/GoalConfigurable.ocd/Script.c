/**
	Library for a goal that is configurable in the {@link Environment_Configuration#MenuConfigureGoal}
	menu.
	
	@author Marky
	@credits Hazard Team
 */


#include Library_Goal

/* --- Properties --- */

local score_time_points;
local score_list_points;
local score_list_rounds;

local win_score;

local is_fulfilled;
local is_inverted;

local leading_faction;

local Name = "$Name$";

/* --- Interface --- */

/**
	Changes the score of a player's faction for the current round.

	@par player The player number.
	@par change The amount that the score should change.
	@par force_negative By default, negative changes are ignored.
                        Set this parameter to {@code true} if you want to
                        decrease the score.
 */
public func DoScore(int player, int change, bool force_negative)
{
	return DoFactionScore(GetFactionByPlayer(player), change, force_negative);
}


/**
	Changes the score of a faction for the current round.

	@par faction The faction, by index.
	@par change The amount that the score should change.
	@par force_negative By default, negative changes are ignored.
                        Set this parameter to {@c true} if you want to
                        decrease the score.
 */
public func DoFactionScore(int faction, int change, bool force_negative)
{
	AssertArrayBounds(score_list_points, faction);

	if (force_negative)
	{
		score_list_points[faction] += change;
	}
	else
	{
		score_list_points[faction] += Max(0, change);
	}
	
	score_time_points[faction] = -FrameCounter(); // Save as negative value, for picking the earliest value as maximum.
	
	this->OnFactionScoreChange(faction);
}


/**
	Increases the number of won rounds for a faction.

	@par faction The faction, by index.
	@par change The amount that the score should increase by.
             	Negative numbers are ignored.
 */
public func DoRoundScore(int faction, int change)
{
	AssertArrayBounds(score_list_rounds, faction);
	
	score_list_rounds[faction] += Max(0, change);
}


/**
	Gets the score of a faction, for the current round.

	@par faction The faction, by index.

	@return int The score value.
 */
public func GetFactionScore(int faction)
{
	AssertArrayBounds(score_list_points, faction);

	return score_list_points[faction];
}


/**
	Gets the frame when the score of the faction
	changed the last time, for the current round.

	@par faction The faction, by index.

	@return int The frame when the score changed
 */
public func GetFactionScoreFrame(int faction)
{
	AssertArrayBounds(score_time_points, faction);

	return score_time_points[faction];
}


/**
	Gets the score of a player's faction, for the current round.

	@par player The player number.

	@return int The score value.
 */
public func GetScore(int player)
{
	return GetFactionScore(GetFactionByPlayer(player));
}


/**
	Sets the score of a faction, for the current round.

	@par faction The faction, by index.
 */
public func SetFactionScore(int faction, int value)
{
	DoFactionScore(faction, value - GetFactionScore(faction), true);
}


/**
	Sets the score of a player's faction, for the current round.

	@par player The player number.
 */
public func SetScore(int player, int value)
{
	return SetFactionScore(GetFactionByPlayer(player), value);
}


/**
	Gets the number of rounds that a faction won.

	@par faction The faction, by index.
	@return int The number of rounds that the faction won.
 */
public func GetRoundScore(int faction)
{
	AssertArrayBounds(score_list_rounds, faction);

	return score_list_rounds[faction];
}


/**
	Gets the number of points that a faction has to score in order to win.

	@return int If {@link Library_Goal_Configurable#GetFactionScore} of a faction is
                at least this number, then that faction wins the round.
 */
public func GetWinScore()
{
	return win_score;
}


/**
	Sets the number of points that a faction has to score in order to win.

	@par score No matter the input value the score is at least 1. If
               {@link Library_Goal_Configurable#GetFactionScore} of a faction is
               at least this number, then that faction wins the round.
 */
public func SetWinScore(int score)
{
	win_score = Max(1, score);
}


/**
	Increases the win score by the specified amount.

	@par change The win score changes by this amount.
             	Can be a positive or negative value.
 */
public func DoWinScore(int change)
{
	SetWinScore(GetWinScore() + change);
}


/**
	Lets a list of factions win the current round.

	@par factions An array of factions that won the round.
 */
public func DoWinRound(array factions)
{
	if (RoundManager() == nil)
	{
		is_fulfilled = true;
	}
	else
	{
		RoundManager()->RemoveRoundEndBlocker(this);
	}
}


/**
	Sets the leading faction, for the current round.
	@par faction An array of faction indices, by index.
 */
public func SetLeadingFaction(array faction)
{
	if (faction == leading_faction)
	{
		this->~OnLeadingFactionOngoing(faction);
	}
	else
	{
		this->~OnLeadingFactionChanged(leading_faction, faction);
	}
	leading_faction = faction;
}


/**
	Gets the leading faction, for the current round.
	
	@return array Returns an array of the best factions.
	              Usually this is length 1. 
 */
public func GetLeadingFaction()
{
	return leading_faction;
}


/* --- Engine callbacks --- */

func Initialize()
{
	var factions = Max(1, this->~GetFactionCount());
	
	leading_faction = [];
	
	score_time_points = [];
	score_list_points = [];
	score_list_rounds = [];
	
	// Fill the arrays. "<=" is correct, because the team numbers start at 1
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


func Destruction()
{
	var round_number;
	if (RoundManager() == nil)
	{
		round_number = 1;
	}
	else
	{
		round_number = RoundManager()->GetRoundNumber();
	}

	this->~DoCleanup(round_number);
}


/* --- Callbacks from goal system --- */

func IsFulfilled() { return is_fulfilled; }


func Activate(int by_player)
{
	var message = GetDescription(by_player);
	
	MessageWindow(message, by_player);
}


func GetFactionScoreMessage(int faction)
{
	var color = this->GetFactionColor(faction);
	var score = GetFactionScore(faction);
	if (color)
		return Format("<c %x>%d</c>", color, score);
	else
		return Format("%d", score);
}


func GetGoalDescription(int faction)
{
	if(IsFulfilled()) 
	{
		if (GetFactionScore(faction) >= GetWinScore())
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


/**
	Determines the relative score of a faction,
	that is the faction score compared to the best score.
 */
func GetRelativeScore(int faction)
{
	var not_initialized = -1;
	var best_faction = not_initialized, best_score = not_initialized;
	for(var i = 0; i < GetFactionCount(); ++i)
	{
		var current_faction = GetFactionByIndex(i);
		var score = GetFactionScore(current_faction);
		if (current_faction != faction && ((score > best_score) || (best_faction == -1)))
		{
			best_faction = current_faction;
			best_score = score;
		}
	}
	
	var faction_score = GetFactionScore(faction);
	
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


/* --- Callbacks from round manager --- */

// From round manager
func OnRoundStart(int round)
{
	this->DoSetup(round);
	_inherited(round);
}


/**
	Callback. This is called by {@link Environment_RoundManager#DoRoundStart}.
	Does nothing by default.
 */
public func DoSetup(int round)
{
}


/**
	Callback. This is called by {@link Library_Goal_Configurable#Destruction}.
	Does nothing by default.
 */
public func DoCleanup(int round)
{
}


/* --- Should be overloaded --- */

public func GetFactionCount()
{
	FatalError("Implement this in a derived object");
}

public func GetFactionByIndex(int index)
{
	FatalError("Implement this in a derived object");
}

public func GetFactionByPlayer(int player)
{
	FatalError("Implement this in a derived object");
}

public func GetFactionColor(int faction)
{
	FatalError("Implement this in a derived object");
}

func GetFactionName(int faction)
{
	return "$DefaultFactionName$";
}

/* --- Event callbacks --- */

/**
	Callback when the score of a faction changes.
	
	@par faction The faction, by index.
 */
func OnFactionScoreChange(int faction)
{
	SetLeadingFaction(DetermineLeadingFaction());
	
	if (score_list_points[faction] >= win_score)
	{
		DoWinRound(GetLeadingFaction());
	}
	_inherited(faction, ...);
}



/* --- Internals --- */


func DetermineLeadingFaction()
{
	var best = DetermineHighestScoringFaction();

	if (GetLength(best) > 1)
	{
		return DetermineEarliestScoringFaction(best);
	}
	else
	{
		return best;
	}
}

func DetermineHighestScoringFaction()
{
	// Get the best factions
	return GetMaxValueIndices(score_list_points);
}

func DetermineEarliestScoringFaction(array factions)
{
	var times = PickArrayValues(score_time_points, factions);

	return GetMaxValueIndices(times);
}


func GetFactionSize(int faction)
{
	var size = 0;
	for (var i = 0; i < GetPlayerCount(); ++i)
	{
		if (faction == GetFactionByPlayer(GetPlayerByIndex(i)))
		{
			++size;
		}
	}
	return size;
}


func EnsureArraySize(int factions)
{
	for (var i = 0; i <= factions && GetLength(score_list_points) <= factions; ++i)
	{
		PushBack(score_list_points, 0);
		PushBack(score_list_rounds, 0);
	}
}


