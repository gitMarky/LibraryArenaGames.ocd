/**
 Gets the game configuration object. This object has to return {@c true} when
 {@c IsGameConfiguration()} is called on it.
 @return object The game configuration object, or {@c nil} if it does not exist.
 @version 0.1.0
 */
global func GameConfiguration()
{
	return FindObject(Find_Func("IsGameConfiguration"));
}
