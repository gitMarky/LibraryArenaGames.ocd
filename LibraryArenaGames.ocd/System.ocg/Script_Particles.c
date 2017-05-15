
global func Particles_JumpPad(int angle, int color, int size)
{
	size = size ?? 21;
	color = color ?? 0xffffffff;
	
	return
	{
		Size = size,
		Alpha = PV_KeyFrames(0, 0, 0, 400, 255, 1000, 0),
		R = GetRGBaValue(color, RGBA_RED),
		G = GetRGBaValue(color, RGBA_GREEN),
		B = GetRGBaValue(color, RGBA_BLUE),
		Rotation = angle,
	};
}
