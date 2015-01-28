
global func AssertArrayBounds(array a, int index)
{
	if (index < 0 || index >= GetLength(a))
	{
		FatalError(Format("Parameter outside array bounds (0 to %d): %d", GetLength(a), index));
	}
}