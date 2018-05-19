/**
	Colors the string with the given color.
	
	@par text The string
	@par color The color, in dword format.
 */
global func ColorizeString(string text, int color)
{
	return Format("<c %x>%s</c>", MakeReadableColor(color), text);
}

global func MakeReadableColor(int color)
{
	// determine lightness
	// 50% red, 87% green, 27% blue (max 164 * 255)
	var r = color >> 16 & 255, g = color >> 8 & 255, b = color & 255;
	var lightness = r * 50 + g * 87 + b * 27;
	// more than 35/164 (*255) is OK; in this case discard alpha only
	if (lightness >= 8925)
		return color & 16777215;
	// make lighter
	return RGB(Min(r + 50, 255), Min(g + 50, 255), Min(b + 50, 255));
}
