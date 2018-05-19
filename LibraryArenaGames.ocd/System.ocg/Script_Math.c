/**
	Math functions

	@author Marky
 */
 

/**
	Determines the array indices with the highest value.
	
	@par values The array. {@code nil} values will be skipped.
	
	@return array An array with the indices. Always returns an array. 
	              In case the values array was empty, this array will
	              also be empty.
 */
global func GetMaxValueIndices(array values)
{
	var indices = [];

	var best_value = nil;
	if (GetLength(values) > 0)
	{
		for (var index = 0; index < GetLength(values); ++index)
		{
			var value = values[index];
			if (value == nil)
			{
				continue;
			}
			
			if (best_value == nil || value > best_value)
			{
				best_value = value;
				indices = [index];
			}
			else if (value == best_value)
			{
				PushBack(indices, index);
			}
		}
	}

	return indices;
}

/**
	Picks all values at given indices from an array.
	
	@par values The values array.
	@par indices These indices will have the
	             values, all other values will be {@code nil}.
	             
	@return array A new array.
 */
global func PickArrayValues(array values, array indices)
{
	var picked = [];
	for (var index in indices)
	{
		picked[index] = values[index];
	}
	return picked;
}
