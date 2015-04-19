/**
 Throws a fatal error if the index is outside the array bounds.
 Use this if you want to prevent expanding arrays.
 @par a
 @par index
 @version 0.1.0
 */
global func AssertArrayBounds(array a, int index)
{
	if (index < 0 || index >= GetLength(a))
	{
		FatalError(Format("Parameter outside array bounds (0 to %d): %d", Max(1, GetLength(a)-1), index));
	}
}