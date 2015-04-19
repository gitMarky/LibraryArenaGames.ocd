/**
 Include this library in rules that can be configured in
 {@link Environment_Configuration#MenuConfigureRules}.
 @author Marky
 @version 0.1.0
 */

#include Library_BasicRule

/**
 Marks the rule as configurable for {@link Environment_Configuration}.
 @return {@c true}
 */
public func GameConfigIsChoosable()
{
	return true;
}

/**
 Defines rules that this rule is in conflict with. If one of the rules
 is active, the other rule is deactivated.
 Interface for {@link Environment_Configuration#MenuConfigureRules}.
 @return array An array of ids. By default the array is empty.
 */
public func GameConfigConflictingRules()
{
	return [];
}


/**
 Defines rules that this rule requires. The rule can be chosen only
 if all the required rules are active.
 Interface for {@link Environment_Configuration#MenuConfigureRules}.
 @return array An array of ids. By default the array is empty.
 */
public func GameConfigRequiredRules()
{
	return [];
}