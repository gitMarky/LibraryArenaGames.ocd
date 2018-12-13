/**
	Relaunch Location

	Prototype for proplist with getter and setter functions.

	@author Marky
*/

global func RelaunchLocation(int x, int y)
{
	var location = new Arena_RelaunchLocation{};
	return location->SetX(x)->SetY(y);
}

static const Arena_RelaunchLocation = new Global
{
	/* --- Internal properties --- */
	x = 0,
	y = 0,
	team = nil,

	/* --- Getters --- */

	/**
		Getter.
		Note: overloads a global engine function.

		@return int The x position where the relaunch will happen.
	 */
	GetX = func()
	{
		return this.x;
	},

	/**
		Getter.
		Note: overloads a global engine function.

		@return int The y position where the relaunch will happen.
	 */
	GetY = func()
	{
		return this.y;
	},


	/**
		Getter.
		Note: overloads a global engine function.

		@return int Only members of this team are allowed to relaunch here.
	 */
	GetTeam = func()
	{
		return this.team;
	},

	/* --- Setters --- */

	/**
		Setter.

		@par value The x position where the relaunch will happen.

		@return proplist Returns the proplist, so that
		                 further functions can be called on it.
	 */
	SetX = func(int value)
	{
		this.x = value;
		return this;
	},


	/**
		Setter.

		@par value The y position where the relaunch will happen.

		@return proplist Returns the proplist, so that
		                 further functions can be called on it.
	 */
	SetY = func(int value)
	{
		this.y = value;
		return this;
	},


	/**
		Setter.

		@par value Only members of this team are allowed to relaunch here.
		           A value of {@code nil} means, that any team may relaunch
		           here.

		@return proplist Returns the proplist, so that
		                 further functions can be called on it.
	 */
	SetTeam = func(int value)
	{
		this.team = value;
		return this;
	},
};
