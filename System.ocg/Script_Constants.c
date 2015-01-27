/*
 *  -- Constants --
 *
 *  Author: Marky
 */


// This is used for loading and saving player and crew data
static const PlrExtraDataPrefix = "OCClassic";

static const C4MN_ItemExtra_None = 		 		0; //: normal
static const C4MN_ItemExtra_Rank = 		 		1;//: rank symbol. With symbol specified, the Rank.png component of that definition will be used. count indicates the rank
static const C4MN_ItemExtra_Picture = 	 		2;//: picture facet, shifted to the right by XPar1 times the facet width. This is used to include multiple menu symbols in a single definition.
static const C4MN_ItemExtra_ObjectRank = 		3;//: XPar1 specifies an object to be drawn with the rank symbol. If the object has no info section (and thus no rank), there will be an empty entry in context menus.
static const C4MN_ItemExtra_Object = 	 		4;//: XPar1 specifies an object to be drawn.
static const C4MN_ItemExtra_BitOverrideValue =	128;//: XPar1 specifies an object to be drawn.

static const PROP_Touchable_None = 0;
static const PROP_Touchable_Grab = 1;
static const PROP_Touchable_GrabAndPush = 2;

