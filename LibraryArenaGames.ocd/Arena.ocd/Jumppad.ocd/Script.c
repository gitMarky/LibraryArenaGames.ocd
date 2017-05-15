
static const JUMPPAD_DEFAULT_COLOR = 0xff64dcff;
static const JUMPPAD_LAYER_LAMP = 1;
static const JUMPPAD_LAYER_LIGHT = 2;
static const JUMPPAD_LAYER_GLOW = 3;
static const JUMPPAD_LAYER_BASE = 4;

local strength;
local color;
local angle_base;
local active;

func Initialize()
{
	SetGraphics("Lamps", GetID(), JUMPPAD_LAYER_LAMP, GFXOV_MODE_ExtraGraphics);
	SetGraphics("Light", GetID(), JUMPPAD_LAYER_LIGHT, GFXOV_MODE_ExtraGraphics, nil, GFX_BLIT_Additive);
	SetGraphics("Shine", GetID(), JUMPPAD_LAYER_GLOW, GFXOV_MODE_ExtraGraphics, nil, GFX_BLIT_Additive);
	SetGraphics("BaseLarge", GetID(), JUMPPAD_LAYER_BASE, GFXOV_MODE_ExtraGraphics);
 	AddTimer(this.CheckBounce, 1);
 	AddTimer(this.ParticleEffect, 8);
 	SetObjDrawTransform(1000, 0, 0, 0, 1000, -4000, JUMPPAD_LAYER_GLOW);

	color = JUMPPAD_DEFAULT_COLOR;
	Activate();
}


func SetBaseR(int r)
{
	angle_base = r;
}

func DrawBase(int r)
{
	var w = 1000;
	var h = 1000;
	var full_size = 1000;

	var fsin = -Sin(r, 1000), fcos = Cos(r, 1000);

	var width  = +fcos * w / full_size;
	var height = +fcos * h / full_size;
	var xskew  = +fsin * h / full_size;
	var yskew  = -fsin * w / full_size;

	// set matrix values
	SetObjDrawTransform(width, xskew, nil, yskew, height, nil, JUMPPAD_LAYER_BASE);
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
}


func SetStrength(int strength)
{
	this.strength = strength;
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
	xdir_new = Sin(GetR(), strength);
	ydir_new = -Cos(GetR(), strength);
	// the object keeps a bit of its old velocity
	xdir_new += xdir_old / 10;
	ydir_new += ydir_old / 10;
	// puff!
	target->SetAction("Jump");
	target->SetSpeed(xdir_new, ydir_new);
	target->~OnBouncedByJumpPad(this);
	Sound("Structural::Jumppad"); // TODO: extract to function
}

func ParticleEffect()
{
	if (active)
	{
		// particle effect for jump pad "waves"
		var lifetime = 40;
		var range = 180; // 18 pixels
		var xdir = +Sin(GetR(), range / lifetime);
		var ydir = -Cos(GetR(), range / lifetime);
		CreateParticle("Arena_JumpPad", 0, 0, xdir, ydir, lifetime, Particles_JumpPad(GetR(), color, 3), 1);
	}
}


func Set(int strength, int angle_top, int angle_base)
{
	SetR(angle_top);
	SetStrength(strength);
	SetBaseR(angle_base-angle_top);
	
	return this;
}


func SaveScenarioObject(proplist props)
{
	if (!_inherited(props, ...))
		return false;
	
	if (GetR())
		props->AddCall("Set", this, "Set", GetStrength(), GetR(), angle_base);
	else
		props->AddCall("Set", this, "Set", GetStrength(), GetR());
	
	return true;
}
