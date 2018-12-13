
static const EFFECT_Disintegration_Name = "ArenaDisint";
static const EFFECT_Disintegration_BaseAlpha = 60;
static const EFFECT_Disintegration_Precision = 100;

/**
	Fades out and removes an object over a short amount of time.

	@par lifetime The object will fade to the color {@link global#FxArenaDisintColorObject}
                  over this amount of frames. Afterwards it fades out.
	@par dt After {@c lifetime} frames have passed the object will fade out over this amount
            of frames.
	@par dy The target changes its y position by {@c dy} over the course of {@c lifetime + dt} frames.
	@par additive If true, then the object will be drawn additive when the function is called.

	@related {@link global#IsDisintegrating}, {@link global#FxArenaDisintColorObject}, {@link global#FxArenaDisintColorParticle}
	@author Marky
  */
global func Disintegrate(int lifetime, int dt, int dy, bool additive)
{
	if(!this)
	{
		FatalError("Disintegrate() has to be called from object context!");
	}

	if (Contained()) return;
	if (IsDisintegrating()) return;

	if(!GetRDir()) SetRDir(RandomX(-7, 7));

	AddEffect(EFFECT_Disintegration_Name, this, 1, 1, nil, nil, lifetime, dt, dy, additive);
}

global func IsDisintegrating()
{
	if(!this)
	{
		FatalError("Disintegrate() has to be called from object context!");
	}

	if (GetEffect(EFFECT_Disintegration_Name, this)) return true;
	return false;
}

global func FxArenaDisintStart (object target, proplist effect, int temp, int lifetime, int dt, int dy, bool additive)
{
	if (!lifetime) lifetime = 50;
	if (!dt) dt = RGBA_MAX;

	target.Collectible = false;

	effect.lifetime = lifetime;
	effect.dt = dt;
	effect.dy = dy * EFFECT_Disintegration_Precision;
	effect.y = target->GetY(EFFECT_Disintegration_Precision);
	effect.r = GetRGBaValue(target->GetColor(), RGBA_RED);
	effect.g = GetRGBaValue(target->GetColor(), RGBA_GREEN);
	effect.b = GetRGBaValue(target->GetColor(), RGBA_BLUE);
	if (additive) target->SetObjectBlitMode(GFX_BLIT_Additive);
}

global func FxArenaDisintTimer(object target, proplist effect, int time)
{
	// collecting a disintegrating object cancels the effect
	// no undoing the color values for now.
	if(target->Contained())
	{
		return FX_Execute_Kill;
	}

	var lifetime = effect.lifetime;
	var dt = effect.dt;
	var dy = time * effect.dy / (lifetime + dt);
	var clr_mod, clr_dw;

	if(time <= lifetime)
	{
		var white =   (lifetime - time) * RGBA_MAX;
		var owner_r = (lifetime - time) * effect.r;
		var owner_g = (lifetime - time) * effect.g;
		var owner_b = (lifetime - time) * effect.b;

		var r = time * GetRGBaValue(FxArenaDisintColorObject(), RGBA_RED);
		var g = time * GetRGBaValue(FxArenaDisintColorObject(), RGBA_GREEN);
		var b = time * GetRGBaValue(FxArenaDisintColorObject(), RGBA_BLUE);

		clr_mod = RGBa((white + r) / lifetime, (white + g) / lifetime, (white + b) / lifetime, RGBA_MAX - BoundBy(EFFECT_Disintegration_BaseAlpha * time / lifetime, 0, EFFECT_Disintegration_BaseAlpha));
		clr_dw = RGBa((owner_r + r) / lifetime, (owner_g + g) / lifetime, (owner_b + b) / lifetime, RGBA_MAX);
	}
	else
	{	
		var diff = RGBA_MAX * (time - lifetime) / dt;

		clr_mod = SetRGBaValue(FxArenaDisintColorObject(), RGBA_MAX - diff, RGBA_ALPHA);
		clr_dw = FxArenaDisintColorObject();

		if(diff >= RGBA_MAX)
		{
			if (target) target->RemoveObject();
			return FX_Execute_Kill;
		}
	}

	for(var i = 1; i < 20; i++)
	{
		if (target->GetUnusedOverlayID(i) == i) continue;

		target->SetObjectBlitMode(GFX_BLIT_Additive, i);
	}

	target->SetClrModulation(clr_mod);
	target->SetColor(clr_dw);

	var ran = BoundBy(20 - 2 * Distance(target->GetDefWidth(), target->GetDefHeight()) / 3, 2, 20);

	if(!Random(ran))
	{
		var r = target->GetR();
		var x1, x2, y1, y2;
		x1 = target->GetDefOffset(0);
		x2 = target->GetDefOffset(0) + target->GetDefWidth();
		y1 = target->GetDefOffset(1);
		y2 = target->GetDefOffset(1) + target->GetDefHeight();

		var rx1, rx2, ry;

		rx1 = Cos(r, x1) - Sin(r, y2);
		rx2 = Cos(r, x2) - Sin(r, y1);

		ry = Cos(r, target->GetDefHeight() / 2) + Sin(r, target->GetDefWidth() / 2);

		target->CreateParticle("Magic", PV_Random(rx1, rx2), PV_Random(ry / 2, ry), 0, -2 + effect.dy / (2 * EFFECT_Disintegration_Precision), PV_Random(25, 40), Particles_Disintegrate(diff));
	}

	target->SetPosition(target->GetX(EFFECT_Disintegration_Precision), effect.y + dy, false, EFFECT_Disintegration_Precision);
	target->SetYDir();
}

/**
	When you {@link global#Disintegrate} an object it will fade to this color.
	@return int A color value.
 */
global func FxArenaDisintColorObject()
{
	return RGB(64, 128, 255);
}


/**
	When you {@link global#Disintegrate} an object it create particles of this color.
	@return int A color value.
 */
global func FxArenaDisintColorParticle()
{
	return RGB(64, 128, 255);
}


/**
	Defines what particles are used when you {@link global#Disintegrate} an object.

	@par alpha The current alpha value of the fading out object. Particles should have a similar
               alpha value.
	@return proplist A particle proplist.
 */
global func Particles_Disintegrate(int alpha)
{
	return {
			Prototype = Particles_Magic(),
			Size = PV_Random(1, 3),
			R = GetRGBaValue(FxArenaDisintColorParticle(), RGBA_RED),
			G = GetRGBaValue(FxArenaDisintColorParticle(), RGBA_GREEN),
			B = GetRGBaValue(FxArenaDisintColorParticle(), RGBA_BLUE),
			Alpha = PV_Linear(alpha, 0),
			};
}
