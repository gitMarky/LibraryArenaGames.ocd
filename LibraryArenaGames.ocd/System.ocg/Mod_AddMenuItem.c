/**
	The default AddMenuItem cannot render icons for 3d graphics.@br
	@br
	This script uses a workaround to enable that - it renders an object instead!@br

	@author Marky
  */
global func AddMenuItem(string caption, string command, id symbol, int count, parameter, string info_caption, int extra, XPar1, XPar2)
{

	//DebugLog(Format("AddMenuItem %s %s", Format("caption: %v, command: %v, symbol: %v, count: %v, parameter: %v", caption, command, symbol, count, parameter), Format(", caption: %v, extra: %v, xpar1: %v, xpar2: %v", info_caption, extra, XPar1, XPar2)));
	var extraX = extra;
	var XPar1X = XPar1;

	if (symbol != nil && symbol->HasMeshGraphics() && extra == C4MN_ItemExtra_None || extra == C4MN_ItemExtra_BitOverrideValue)
	{
		extraX = extra | C4MN_ItemExtra_Object;
		XPar1X = GetMeshDummy(symbol);
		//DebugLog("Need new gfx for %v", symbol);
	}

	return _inherited(caption, command, symbol, count, parameter, info_caption, extraX, XPar1X, XPar2);
}

/**
	@author Marky
 */
global func HasMeshGraphics()
{
	if(!this) return false;
	return (this->GetMeshMaterial() != nil);
}


global func GetMeshDummy(id type)
{
	if (GetType(this) != C4V_C4Object)
	{
		FatalError("This function must be called from object context.");
	}

	var name = Format("%v", type);

	if (!(type->HasMeshGraphics()))
	{
		return nil;
	}

	var dummy = GetProperty(name, this.proplist_addmenuitem);
	if (!dummy)
	{
		dummy = CreateObject(Dummy, -GetX(), -GetY(), NO_OWNER);
		dummy->SetGraphics(nil, Dummy, 0, GFXOV_MODE_None);
		dummy->SetGraphics(nil, type, 1, GFXOV_MODE_Picture);

		if (!SetProperty(name, dummy, this.proplist_addmenuitem))
		{
			this.proplist_addmenuitem = {"Dummy" = nil};
			SetProperty(name, dummy, this.proplist_addmenuitem);
		}
	}

	return dummy;
}
