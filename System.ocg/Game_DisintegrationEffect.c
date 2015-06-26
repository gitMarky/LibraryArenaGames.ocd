/* Disintegration */

static const EFFECT_Disintegration_Name = "ArenaDisint";
static const EFFECT_Disintegration_BaseAlpha = 60;
static const EFFECT_Disintegration_Color = -16711936; // RGB(0, 255, 0)
static const EFFECT_Disintegration_ParticleColor = -16723456; // RGB(0, 210, 0)

global func Disintegrate(int lifetime, int dt, int dy)
{
	if(!this)
	{
		FatalError("Disintegrate() has to be called from object context!");
	}

	if (Contained()) return;
	if (IsDisintegrating()) return;
	
	if(!GetRDir()) SetRDir(RandomX(-7, 7));

	AddEffect(EFFECT_Disintegration_Name, this, 1, 1, nil, 0, lifetime, dt, dy);

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

global func FxArenaDisintStart (object target, proplist effect, int temp, int lifetime, int dt, int dy)
{
	if (!lifetime) lifetime = 50;
	if (!dt) dt = RGBA_MAX;
	//if (!dy) dy = -5;
	effect.lifetime = lifetime;
	effect.dt = dt;
	effect.dy = dy;
	effect.y = target->GetY();
	effect.r = GetRGBaValue(target->GetColor(), RGBA_RED);
	effect.g = GetRGBaValue(target->GetColor(), RGBA_GREEN);
	effect.b = GetRGBaValue(target->GetColor(), RGBA_BLUE);
}

global func FxArenaDisintTimer(object target, proplist effect, int time)
{
	// aufgesammelte Objekte brechen ab
	// allerdings kann der Clonk keine disintegrierenden Objekte aufsammeln, also passt alles
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
		// fade owner color

		var r, g, b;

		r = BoundBy(((lifetime - time) * effect.r + time * RGBA_MAX) / lifetime, 0, RGBA_MAX);
		g = BoundBy(((lifetime - time) * effect.g + time * RGBA_MAX) / lifetime, 0, RGBA_MAX);
		b = BoundBy(((lifetime - time) * effect.b + time * RGBA_MAX) / lifetime, 0, RGBA_MAX);

		clr_dw = RGB(r, g, b);

		// fade overall color, from white to specific

		var white = (lifetime - time) * RGBA_MAX;
		r = time * GetRGBaValue(EFFECT_Disintegration_Color, RGBA_RED);
		g = time * GetRGBaValue(EFFECT_Disintegration_Color, RGBA_GREEN);
		b = time * GetRGBaValue(EFFECT_Disintegration_Color, RGBA_BLUE);

		clr_mod = RGBa((white + r) / lifetime, (white + g) / lifetime, (white + b) / lifetime, RGBA_MAX - BoundBy(EFFECT_Disintegration_BaseAlpha * time / lifetime, 0, EFFECT_Disintegration_BaseAlpha));
	}
	else
	{
		// fade owner color
	
		clr_dw = RGB(RGBA_MAX, RGBA_MAX, RGBA_MAX);
		
		// fade out overall color
	
		var diff = RGBA_MAX * (time - lifetime) / dt;
		var alpha_max = RGBA_MAX - EFFECT_Disintegration_BaseAlpha;

		target->SetObjectBlitMode(GFX_BLIT_Additive);

		clr_mod = SetRGBaValue(EFFECT_Disintegration_Color, alpha_max - diff, RGBA_ALPHA);

		if(diff >= alpha_max)
		{
			if (target) target->RemoveObject();
			return FX_Execute_Kill;
		}
	}

	for(var i = 1; i < 20; i++)
	{
		if (target->GetUnusedOverlayID(i) == i) continue;

		target->SetClrModulation(clr_mod, i);
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

		target->CreateParticle("Magic", PV_Random(rx1, rx2), PV_Random(ry / 2, ry), 0, -2 + effect.dy / 2, PV_Random(25, 40), Particles_Disintegrate(diff));
	}

	target->SetPosition(target->GetX(), effect.y + dy);
	target->SetYDir();
}

global func Particles_Disintegrate(int alpha)
{
	return {
			Prototype = Particles_Magic(),
			Size = PV_Random(1, 3),
			R = GetRGBaValue(EFFECT_Disintegration_ParticleColor, RGBA_RED),
			G = GetRGBaValue(EFFECT_Disintegration_ParticleColor, RGBA_GREEN),
			B = GetRGBaValue(EFFECT_Disintegration_ParticleColor, RGBA_BLUE),
			Alpha = PV_Linear(alpha, 0),
			};
}