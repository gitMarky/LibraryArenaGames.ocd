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

	@par faction The faction.
	@par change The amount that the score should change.
	@par force_negative By default, negative changes are ignored.
                        Set this parameter to {@c true} if you want to
                        decrease the score.
 */
public func DoFactionScore(proplist faction, int change, bool force_negative)
{
	AssertArrayBounds(score_list_points, faction->GetID());

	if (force_negative)
	{
		score_list_points[faction->GetID()] += change;
	}
	else
	{
		score_list_points[faction->GetID()] += Max(0, change);
	}

	score_time_points[faction->GetID()] = -FrameCounter(); // Save as negative value, for picking the earliest value as maximum.

	this->OnFactionScoreChange(faction);
}


/**
	Increases the number of won rounds for a faction.

	@par faction The faction.
	@par change The amount that the score should increase by.
             	Negative numbers are ignored.
 */
public func DoRoundScore(proplist faction, int change)
{
	AssertArrayBounds(score_list_rounds, faction->GetID());

	score_list_rounds[faction->GetID()] += Max(0, change);
}


/**
	Gets the score of a faction, for the current round.

	@par faction The faction.

	@return int The score value.
 */
public func GetFactionScore(proplist faction)
{
	AssertArrayBounds(score_list_points, faction->GetID());

	return score_list_points[faction->GetID()];
}


/**
	Gets the frame when the score of the faction
	changed the last time, for the current round.

	@par faction The faction, by index.

	@return int The frame when the score changed
 */
public func GetFactionScoreFrame(proplist faction)
{
	AssertArrayBounds(score_time_points, faction->GetID());

	return score_time_points[faction->GetID()];
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
public func SetFactionScore(proplist faction, int value)
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
public func GetRoundScore(proplist faction)
{
	AssertArrayBounds(score_list_rounds, faction->GetID());

	return score_list_rounds[faction->GetID()];
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
	_inherited(factions, ...);
}


/**
	Sets the leading faction, for the current round.
	@par faction An array of faction indices, by index.
 */
public func SetLeadingFactionID(array faction)
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
public func GetLeadingFactionID()
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

	// Determine faction size

	var player = 0;
	for (var i = 0; i < GetPlayerCount(); i++)
	{
		var current = GetPlayerByIndex(i);

		if (current > player)
		{
			player = current;
		}
	}

	EnsureArraySize(GetFactionByPlayer(player)->GetID());

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


func InitializePlayer(int player)
{
	_inherited(player, ...);

	EnsureArraySize(GetFactionByPlayer(player)->GetID());
}


/* --- Callbacks from goal system --- */

func IsFulfilled() { return is_fulfilled; }


func Activate(int by_player)
{
	var message = GetDescription(by_player);

	MessageWindow(message, by_player);
}


func GetFactionScoreMessage(proplist faction)
{
	var color = faction->GetColor();
	var score = GetFactionScore(faction);
	if (color)
		return Format("<c %x>%d</c>", color, score);
	else
		return Format("%d", score);
}


func GetGoalDescription(proplist faction)
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
			return Format("$MsgAhead$",	 score.relative_score,  score.best_faction->GetName());
		}
		else if (score.relative_score < 0)
		{
			return Format("$MsgBehind$", -score.relative_score, score.best_faction->GetName());
		}
		else if (score.best_faction == faction)
		{
			return Format("$MsgYouAreBest$", score.relative_score);
		}
		else
		{
			return Format("$MsgEqual$", score.best_faction->GetName());
		}
	}
}


/**
	Determines the relative score of a faction,
	that is the faction score compared to the best score.
 */
func GetRelativeScore(proplist faction)
{
	var best_faction = nil, best_score = nil;
	for(var i = 0; i < GetFactionCount(); ++i)
	{
		var current_faction = GetFactionByIndex(i);
		var score = GetFactionScore(current_faction);
		if (current_faction->GetID() != faction->GetID() && ((score > best_score) || (best_faction == -1)))
		{
			best_faction = current_faction;
			best_score = score;
		}
	}

	var faction_score = GetFactionScore(faction);

	// special case if there is only one player in the game
	if(best_faction == nil)
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


/* --- Faction stuff --- */

func GetFactionCount()
{
	return Arena_FactionManager->GetInstance()->GetFactionCount();
}

func GetFaction(int faction_id)
{
	return Arena_FactionManager->GetInstance()->GetFaction(faction_id);
}

func GetFactionByIndex(int index)
{
	return Arena_FactionManager->GetInstance()->GetFactionByIndex(index);
}

func GetFactionByPlayer(int player)
{
	return Arena_FactionManager->GetInstance()->GetFactionByPlayer(player);
}

/* --- Event callbacks --- */

/**
	Callback when the score of a faction changes.

	@par faction The faction.
 */
func OnFactionScoreChange(proplist faction)
{
	SetLeadingFactionID(DetermineLeadingFactionID());

	if (score_list_points[faction->GetID()] >= win_score)
	{
		DoWinRound(GetLeadingFactionID());
	}
	_inherited(faction, ...);
}



/* --- Internals --- */


func DetermineLeadingFactionID()
{
	var best = DetermineHighestScoringFactionID();

	if (GetLength(best) > 1)
	{
		return DetermineEarliestScoringFactionID(best);
	}
	else
	{
		return best;
	}
}

func DetermineHighestScoringFactionID()
{
	// Get the best factions
	return GetMaxValueIndices(score_list_points);
}

func DetermineEarliestScoringFactionID(array factions)
{
	var times = PickArrayValues(score_time_points, factions);

	return GetMaxValueIndices(times);
}


func GetFactionSize(proplist faction)
{
	var size = 0;
	for (var i = 0; i < GetPlayerCount(); ++i)
	{
		if (faction->GetID() == GetFactionByPlayer(GetPlayerByIndex(i))->GetID())
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
