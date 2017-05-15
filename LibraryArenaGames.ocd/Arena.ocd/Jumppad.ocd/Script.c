
static const JUMPPAD_DEFAULT_COLOR = 0xff64dcff;
static const JUMPPAD_LAYER_LAMP = 1;
static const JUMPPAD_LAYER_LIGHT = 2;
static const JUMPPAD_LAYER_GLOW = 3;
static const JUMPPAD_LAYER_BASE = 4;

local strength;
local color;
local base_r;
local active;
local size;


func Initialize()
{
	SetGraphics("Lamps", GetID(), JUMPPAD_LAYER_LAMP, GFXOV_MODE_ExtraGraphics);
	SetGraphics("Light", GetID(), JUMPPAD_LAYER_LIGHT, GFXOV_MODE_ExtraGraphics, nil, GFX_BLIT_Additive);
	SetGraphics("Shine", GetID(), JUMPPAD_LAYER_GLOW, GFXOV_MODE_ExtraGraphics, nil, GFX_BLIT_Additive);
	SetBaseGraphics("BaseLarge", GetID());
	
 	AddTimer(this.CheckBounce, 1);
 	AddTimer(this.ParticleEffect, 8);

	SetSize(1000);
	SetEffectColor(JUMPPAD_DEFAULT_COLOR);
	Activate();
}

func SetPadR(int r)
{
	SetR(r);
	DrawPad();
	DrawBase();
	return this;
}


func SetPadGraphics(string name, id type)
{
	SetGraphics(name, type);
	return this;
}


func SetBaseR(int r)
{
	base_r = r;
	DrawPad();
	DrawBase();
	return this;
}

func SetBaseGraphics(string name, id type)
{
	SetGraphics(name, type, JUMPPAD_LAYER_BASE, GFXOV_MODE_Action, "Base"); //ExtraGraphics);
	return this;
}

func SetSize(int size)
{
	this.size = size;
	DrawBase();
	DrawPad();
	return this;
}


func DrawPad()
{
	SetObjDrawTransform(size, nil, nil, nil, size, nil, 0);
 	SetObjDrawTransform(1000, 0, 0, 0, 1000, -4 * size, JUMPPAD_LAYER_GLOW);
}

func DrawBase()
{

	var fsin = +Sin(base_r, size), fcos = Cos(base_r, size);

	var width  = +fcos;
	var height = +fcos;
	var xskew  = +fsin;
	var yskew  = -fsin;
	
	var xoff = -5;
	var yoff = +50;
	
	var x_adjust = fcos * xoff - fsin * yoff;
	var y_adjust = fcos * yoff - fsin * xoff;
	
	// set matrix values
	SetObjDrawTransform(width,  xskew, x_adjust / 10,
	                    yskew, height, y_adjust / 10, JUMPPAD_LAYER_BASE);
}

func Activate()
{
	active = true;
	SetClrModulation(0xffffff, JUMPPAD_LAYER_LAMP);
	SetClrModulation(color, JUMPPAD_LAYER_LIGHT);
	SetClrModulation(color, JUMPPAD_LAYER_GLOW);
}


func Deactivate()
{
	active = false;
	SetClrModulation(SetRGBaValue(color, 120, RGBA_ALPHA), JUMPPAD_LAYER_LAMP);
	SetClrModulation(RGBa(255, 255, 255, 1), JUMPPAD_LAYER_LIGHT);
	SetClrModulation(RGBa(255, 255, 255, 1), JUMPPAD_LAYER_GLOW);
}


func SetEffectColor(int color)
{
	this.color = color;
	Activate();
	return this;
}


func SetStrength(int strength)
{
	this.strength = strength;
	return this;
}

func GetStrength()
{
	return this.strength;
}

func CheckBounce()
{
	if (active)
	{
		// bounce targets
		for (var target in GetBounceTargets())
		{
			Bounce(target);
		}
	}	
}

func GetBounceTargets()
{
	return FindObjects(
			GetBounceArea(),
			Find_Or(Find_OCF(OCF_Alive), Find_Func("IsBouncy")),
			Find_NoContainer());
}

func GetBounceArea()
{
	return Find_InRect(-8, -8, 16, 16);
}

func Bounce(object target)
{
	// bazillions of complificated calculiationons!
	var xdir_new, ydir_new, xdir_old, ydir_old;
	xdir_old = target->GetXDir();
	ydir_old = target->GetYDir();
	xdir_new = +Sin(GetR(), strength);
	ydir_new = -Cos(GetR(), strength);
	// the object keeps a bit of its old velocity
	xdir_new += xdir_old / 10;
	ydir_new += ydir_old / 10;
	// puff!
	target->SetAction("Jump");
	target->SetSpeed(xdir_new, ydir_new);
	target->~OnBouncedByJumpPad(this);
	this->OnBounce(target);
}

func OnBounce(object target)
{
	// does nothing at the moment
}

func ParticleEffect()
{
	if (active)
	{
		// particle effect for jump pad "waves"
		var lifetime = 40;
		var range = 180 * size / 1000; // 18 pixels
		var xdir = +Sin(GetR(), range / lifetime);
		var ydir = -Cos(GetR(), range / lifetime);
		CreateParticle("Arena_JumpPad", 0, 0, xdir, ydir, lifetime, Particles_JumpPad(GetR(), color, size), 1);
	}
}


func Particles_JumpPad(int angle, int particle_color, int particle_size)
{
	particle_size = particle_size ?? 7000;
	particle_color = particle_color ?? 0xffffffff;
	
	return
	{
		Size = 3 * particle_size / 1000,
		Alpha = PV_KeyFrames(0, 0, 0, 400, 255, 1000, 0),
		R = GetRGBaValue(color, RGBA_RED),
		G = GetRGBaValue(color, RGBA_GREEN),
		B = GetRGBaValue(color, RGBA_BLUE),
		Rotation = angle,
	};
}



func SaveScenarioObject(proplist props)
{
	if (!_inherited(props, ...))
		return false;
	
	if (GetR())
	{
		props->AddCall("Set", this, "Set", GetStrength(), GetR(), base_r);
	}
	else
	{
		props->AddCall("Set", this, "Set", GetStrength(), GetR());
	}
	
	return true;
}

local ActMap = {
Base = {
	Prototype = Action,
	Name = "Base",
	Procedure = DFA_FLOAT,
	X = 0, Y = 0,
	Wdt = 30, Hgt = 18,
}};
