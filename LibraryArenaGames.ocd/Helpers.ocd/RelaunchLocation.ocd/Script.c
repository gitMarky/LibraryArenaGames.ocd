/**
	Relaunch Location
	
	Prototype for proplist with getter and setter functions.
	
	@author Marky
*/


/* --- Getters --- */

/**
	Getter.
	Note: overloads a global engine function.
	
	@return int The x position where the relaunch will happen.
 */
public func GetX()
{
	return this.x;
}

/**
	Getter.
	Note: overloads a global engine function.
	
	@return int The y position where the relaunch will happen.
 */
public func GetY()
{
	return this.y;
}


/**
	Getter.
	Note: overloads a global engine function.
	
	@return int Only members of this team are allowed to relaunch here.
 */
public func GetTeam()
{
	return this.team;
}


/* --- Setters --- */

/**
	Setter.
	
	@par value The x position where the relaunch will happen.
	
	@return proplist Returns the proplist, so that
	                 further functions can be called on it.
 */
public func SetX(int value)
{
	this.x = value;
	return this;
}


/**
	Setter.
	
	@par value The y position where the relaunch will happen.
	
	@return proplist Returns the proplist, so that
	                 further functions can be called on it.
 */
public func SetY(int value)
{
	this.y = value;
	return this;
}


/**
	Setter.
	
	@par value Only members of this team are allowed to relaunch here.
	           A value of {@code nil} means, that any team may relaunch
	           here.
	
	@return proplist Returns the proplist, so that
	                 further functions can be called on it.
 */
public func SetTeam(int value)
{
	this.team = value;
	return this;
}

/* --- Other --- */

/**
	Definition call constructor.
	
	@par x The x position. Is set via {@link Arena_RelaunchLocation#SetX} directly.
	@par y The y position. Is set via {@link Arena_RelaunchLocation#SetY} directly.
	
	@return proplist Returns the relaunch location.
 */
public func At(int x, int y)
{
	var location = new Arena_RelaunchLocation{};
	return location->SetX(x)->SetY(y);
}
