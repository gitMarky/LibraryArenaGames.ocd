/**
 Constants that should be part of the engine, but are not.[br]
 [br]
 Concerning general stuff:
 <table>
 	<tr><th>Name</th>                     <th>Value</th> <th>Description</th></tr>
 	<tr><td>FRAME_Per_Second</td> <td>36</td> <td>The assumed default value for fps.</td></tr>
 </table>
 
 Concerning menus:
 <table>
 	<tr><th>Name</th>                     <th>Value</th> <th>Description</th></tr>
 	<tr><td>C4MN_ItemExtra_None</td>               <td>0</td> <td>normal.</td></tr>
 	<tr><td>C4MN_ItemExtra_Rank</td>               <td>1</td> <td>rank symbol. With symbol specified, the Rank.png component of that definition will be used. count indicates the rank</td></tr>
 	<tr><td>C4MN_ItemExtra_Picture</td>            <td>2</td> <td>picture facet, shifted to the right by XPar1 times the facet width. This is used to include multiple menu symbols in a single definition.</td></tr>
 	<tr><td>C4MN_ItemExtra_ObjectRank</td>         <td>3</td> <td>XPar1 specifies an object to be drawn with the rank symbol. If the object has no info section (and thus no rank), there will be an empty entry in context menus.</td></tr>
 	<tr><td>C4MN_ItemExtra_Object</td>             <td>4</td> <td>XPar1 specifies an object to be drawn.</td></tr>
 	<tr><td>C4MN_ItemExtra_BitOverrideValue</td> <td>128</td> <td>XPar1 specifies an object to be drawn.</td></tr>
 </table>
 
 Grabbability:
 <table>
 	<tr><th>Name</th>                     <th>Value</th> <th>Description</th></tr>
 	<tr><td>PROP_Touchable_None</td>        <td>0</td> <td>Object cannot be grabbed.</td></tr>
 	<tr><td>PROP_Touchable_Grab</td>        <td>1</td> <td>Object can be grabbed, but not pushed</td></tr>
 	<tr><td>PROP_Touchable_GrabAndPush</td> <td>2</td> <td>Object can be grabbed and pushed.</td></tr>
 </table>

 @author Marky
 @id constants
 @title Engine Constants
 @version 0.1.0
 */

static const FRAME_Per_Second = 				36; // I am suprised that this is not a constant

static const C4MN_ItemExtra_None = 		 		0;  //: normal
static const C4MN_ItemExtra_Rank = 		 		1;  //: rank symbol. With symbol specified, the Rank.png component of that definition will be used. count indicates the rank
static const C4MN_ItemExtra_Picture = 	 		2;  //: picture facet, shifted to the right by XPar1 times the facet width. This is used to include multiple menu symbols in a single definition.
static const C4MN_ItemExtra_ObjectRank = 		3;  //: XPar1 specifies an object to be drawn with the rank symbol. If the object has no info section (and thus no rank), there will be an empty entry in context menus.
static const C4MN_ItemExtra_Object = 	 		4;  //: XPar1 specifies an object to be drawn.
static const C4MN_ItemExtra_BitOverrideValue =	128;//: XPar1 specifies an object to be drawn.

static const PROP_Touchable_None = 0;
static const PROP_Touchable_Grab = 1;
static const PROP_Touchable_GrabAndPush = 2;

static const RGBA_MAX = 255;
