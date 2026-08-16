/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       UI_Element.h
* 
* @class      UI_ELEMENT
* @brief      User Interface Element class
* @ingroup    USERINTERFACE
* 
* @copyright  EndoraSoft. All rights reserved.
* 
* @cond
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
* documentation files(the "Software"), to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense, and/ or sell copies of the Software,
* and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of
* the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
* THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
* @endcond
* 
* --------------------------------------------------------------------------------------------------------------------*/

#pragma once

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XPublisher.h"
#include "XSubject.h"

#include "UI_Color.h"
#include "UI_BoundaryLine.h"



/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


enum UI_ELEMENT_TYPE
{
	UI_ELEMENT_TYPE_UNKNOWN												= 0	,	
	
	UI_ELEMENT_TYPE_TEXT															,	
	UI_ELEMENT_TYPE_TEXTBOX											  		,	
	UI_ELEMENT_TYPE_IMAGE															,	
	UI_ELEMENT_TYPE_ANIMATION													,		
	UI_ELEMENT_TYPE_OPTION														,	
	UI_ELEMENT_TYPE_MULTIOPTION												,									
	UI_ELEMENT_TYPE_BUTTON														,		
	UI_ELEMENT_TYPE_CHECKBOX													,		
	UI_ELEMENT_TYPE_EDITTEXT													,		
	UI_ELEMENT_TYPE_FORM															,
	UI_ELEMENT_TYPE_MENU															,				
	UI_ELEMENT_TYPE_LISTBOX														,			
	UI_ELEMENT_TYPE_SCROLL														,
	UI_ELEMENT_TYPE_PROGRESSBAR												,	
  UI_ELEMENT_TYPE_PROGRESSRADIAL										,	
	UI_ELEMENT_TYPE_PROGRESSIMAGE  
};				


enum UI_ELEMENT_TYPE_ALIGN
{
  UI_ELEMENT_TYPE_ALIGN_NOTDETERMINED	   = -1000000 ,
  UI_ELEMENT_TYPE_ALIGN_LEFT												,
  UI_ELEMENT_TYPE_ALIGN_RIGHT												,
  UI_ELEMENT_TYPE_ALIGN_UP													,
  UI_ELEMENT_TYPE_ALIGN_DOWN												,
  UI_ELEMENT_TYPE_ALIGN_CENTER											,
	UI_ELEMENT_TYPE_ALIGN_MAX													,
	UI_ELEMENT_TYPE_ALIGN_AUTO												,
};


enum UI_ELEMENT_TYPE_DIRECTION
{
	UI_ELEMENT_TYPE_DIRECTION_UNKWOWN							= 0	,
	UI_ELEMENT_TYPE_DIRECTION_HORIZONTAL							,
	UI_ELEMENT_TYPE_DIRECTION_VERTICAL								,
};


enum UI_ELEMENT_TRANSITION_STATE_SHOW	
{
	UI_ELEMENT_TRANSITION_STATE_SHOW_NONE					= 0 ,
	UI_ELEMENT_TRANSITION_STATE_SHOW_TOSHOW						,
	UI_ELEMENT_TRANSITION_STATE_SHOW_TOHIDE						,
};

// NOTE: reserved roles a layout element can take in a GEN custom Chromes (window caption) layout. Set via the
// "role" XML attribute (see UI_MANAGER::GetLayoutElement_Base); NONE means "just a regular element", the default
// for every element that is not part of a Chromes layout.
enum UI_ELEMENT_CHROMEROLE
{
	UI_ELEMENT_CHROMEROLE_NONE											= 0	,

	UI_ELEMENT_CHROMEROLE_CAPTION											,	// Draggable caption background/area
	UI_ELEMENT_CHROMEROLE_ICON												,	// Application icon
	UI_ELEMENT_CHROMEROLE_TITLE											,	// Window title text
	UI_ELEMENT_CHROMEROLE_MINIMIZE										,	// Minimize button
	UI_ELEMENT_CHROMEROLE_MAXIMIZE										,	// Maximize / restore button
	UI_ELEMENT_CHROMEROLE_CLOSE											,	// Close button
};

#define UI_ELEMENT_MARGIN_MAX										  4

// --- Step 5: per-corner border radii (CSS ordering, clockwise from top-left) ---------------------------------------
enum UI_ELEMENT_BORDER_CORNER
{
  UI_ELEMENT_BORDER_CORNER_TL      = 0 ,
  UI_ELEMENT_BORDER_CORNER_TR            ,
  UI_ELEMENT_BORDER_CORNER_BR            ,
  UI_ELEMENT_BORDER_CORNER_BL            ,
  UI_ELEMENT_BORDER_CORNER_MAX
};




/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class XTIMER;
class UI_ELEMENT_SCROLL;


class UI_ELEMENT : public XSUBJECT 
{
  public:
																					UI_ELEMENT									();
    virtual															 ~UI_ELEMENT									();

		XSTRING*															GetName											();

		UI_ELEMENT_TYPE												GetType											();
		void																	SetType											(UI_ELEMENT_TYPE type);

		XSTRING*															GetTypeString								();

		// --- CSS class attribute (space-separated list, HTML/CSS semantics) ------------------------------------------
		// Retro-compatible: layouts that never set a "class" attribute keep an empty list and are matched only by
		// type / id selectors. Populated by the XML loader from the "class" attribute; consumed by UI_STYLESHEET to
		// resolve .class selectors during layout construction.
		XSTRING*															GetClassNamesRaw						();										// raw attribute string ("chip primary big")
		XVECTOR<XSTRING*>*										GetClassNames								();										// split view, one entry per class
		void																	SetClassNames								(XCHAR* rawlist);							// resets the split view from rawlist
		void																	SetClassNames								(XSTRING& rawlist);
		bool																	HasClass										(XCHAR* classname);						// case-insensitive membership test

		// --- Pseudo-class state re-resolution (owned by UI_ELEMENT) --------------------------------------------------
		// Populates `out` with the pseudo-class names implied by the element's current live state (ispreselect,
		// isselected, isactive). Strings are heap-allocated with GEN_NEW; the caller must delete them. Adding new
		// mappings here (e.g. :hover) is the extension point when motion tracking gets wired to the CSS layer.
		void																	GetActivePseudos						(XVECTOR<XSTRING*>& out);

		// Captures the current color / bckgrdcolor / roundrect as the visual baseline for later state flips.
		// UI_MANAGER calls it once at end-of-load, right after XML + stateless CSS have been applied.
		void																	SnapshotStyleVisual					();

		// Re-resolves the active stylesheet with the element's current pseudo-class set, then re-applies the
		// three baseline visual keys (color, bckgrdcolor, roundrect) on top of the snapshot. Called by the state
		// setters when their value changes; a no-op if no stylesheet is active or no snapshot exists.
		void																	ReapplyStyleVisual					();

		// One-time flag that marks this element as reachable by at least one pseudo-carrying rule in the current
		// stylesheet. UI_MANAGER sets it at load time via HasPseudoRulesFor(); if false, the state setters skip
		// the snapshot restore + re-resolve pair entirely (zero cost on elements the sheet never restyles).
		bool																	GetStyleHasStateRules				();
		void																	SetStyleHasStateRules				(bool has);

		UI_ELEMENT_CHROMEROLE									GetChromeRole								();
		void																	SetChromeRole								(UI_ELEMENT_CHROMEROLE chromerole);

		UI_ELEMENT*														GetFather										();	
		void																	SetFather										(UI_ELEMENT* father);	

		bool																	IsDetached									();	
		void																	SetIsDetached								(bool isdetached);	
		
		UI_COLOR*															GetColor										();
		UI_COLOR*															GetBackgroundColor					();

		// --- Authored-vs-default flags for the two base colours (step 6) ------------------------------------------
		// Track whether "color" and "bckgrdcolor" were actually authored (via XML attribute or CSS declaration),
		// as opposed to left at UI_COLOR's zero-initialised default. Consumers use these to implement the
		// CSS-natural semantics: Draw_Form prefers background_color when set (falls back to color for retro-compat
		// with pre-step-6 layouts), and CSS "color: inherit" walks up the parent chain looking for the first
		// ancestor with color_set == true.
		bool																	IsColorSet									();
		void																	SetColorSet									(bool value);

		bool																	IsBackgroundColorSet				();
		void																	SetBackgroundColorSet				(bool value);
		
		double																GetXPosition								();		
		bool																	SetXPosition								(double x_position);		

		double																GetYPosition								();		
		bool																	SetYPosition								(double y_position);		

		XDWORD																GetZLevel										();		
		bool																	SetZLevel										(XDWORD z_level);		
	
		UI_ELEMENT_TYPE_DIRECTION							GetDirection								();
		bool																	SetDirection								(UI_ELEMENT_TYPE_DIRECTION direction);

		double																GetXPositionWithScroll			();		
		bool																	SetXPositionWithScroll			(double x_positionwithscroll);		

		double																GetYPositionWithScroll			();		
		bool																	SetYPositionWithScroll			(double y_positionwithscroll);		

		UI_BOUNDARYLINE*											GetBoundaryLine							();
		void																	SetBoundaryLine							(UI_BOUNDARYLINE& boundaryline);

		double																GetLeftX										();
		double																GetRightX										();
		double																GetTopY											();
		double																GetBottomY									();
		
		double																GetMargin                   (UI_ELEMENT_TYPE_ALIGN position);
		void																	SetMargin                   (UI_ELEMENT_TYPE_ALIGN position, double value);

		// --- CSS box-model additions (step 4) --------------------------------------------------------------------------
		// Padding: inner inset applied by CalculePosition when placing this element's children. Indexed by the same
		// UI_ELEMENT_TYPE_ALIGN enum values used by margin (LEFT / RIGHT / UP / DOWN). Default 0 on every side, so
		// layouts that never set it retain their historical geometry.
		double																GetPadding                  (UI_ELEMENT_TYPE_ALIGN position);
		void																	SetPadding                  (UI_ELEMENT_TYPE_ALIGN position, double value);

		// Border width in pixels for the element's stroke. -1 = "unset, use the skin's historical default" (which
		// today is 1.0 for containers such as UI_ELEMENT_FORM); 0 = draw no border at all; any positive value = draw
		// with that thickness. Value 0 is honoured explicitly so authors can suppress the default 1-px card outline
		// from CSS with "border-width: 0;".
		double																GetBorderWidth              ();
		void																	SetBorderWidth              (double borderwidth);

		// Border color (base-level equivalent of the existing per-type "linecolor"). When unset, containers fall
		// back to their historical linecolor member so pre-step-5 layouts render unchanged. Query IsBorderColorSet()
		// before consuming, to distinguish an authored value (including a transparent one) from the default state.
		UI_COLOR*															GetBorderColor              ();
		bool																	IsBorderColorSet            ();
		void																	SetBorderColorFromString    (XCHAR* string);
		void																	SetBorderColorFromString    (XSTRING& string);

		// Per-corner border radius (CSS convention: TL, TR, BR, BL). A corner with radius < 0 falls back to the
		// element's uniform roundrect; if roundrect is also 0, the corner is drawn square. Consumers should read
		// through GetEffectiveBorderRadius() so the fallback chain is applied consistently.
		double																GetBorderRadius             (UI_ELEMENT_BORDER_CORNER corner);
		void																	SetBorderRadius             (UI_ELEMENT_BORDER_CORNER corner, double value);
		double																GetEffectiveBorderRadius    (UI_ELEMENT_BORDER_CORNER corner);
		bool																	HasAnyPerCornerRadius       ();

		// --- Box-shadow (step 7) ---------------------------------------------------------------------------------------
		// CSS-like drop shadow drawn behind the element (before its fill and stroke) at (offset_x, offset_y),
		// tinted with shadow_color. Blur is accepted from the parser but not rendered yet: this rebanada ships
		// hard shadow only, and stack-blur rendering is deferred to a follow-up step so this one stays small.
		// The skin expands the element's rebuild-area to include the shadow footprint (see PreDrawFunction),
		// so save/restore cycles at repaint time do not leave ghost pixels outside the element rect.
		bool																	IsBoxShadowSet              ();
		void																	SetBoxShadowSet             (bool value);
		double																GetShadowOffsetX            ();
		void																	SetShadowOffsetX            (double value);
		double																GetShadowOffsetY            ();
		void																	SetShadowOffsetY            (double value);
		double																GetShadowBlur               ();
		void																	SetShadowBlur               (double value);
		UI_COLOR*															GetShadowColor              ();

		bool																	IsActive										();
		void																	SetActive										(bool isactive);

		bool																	IsPreSelect									();
		void																	SetPreSelect								(bool ispreselect);

		bool																	IsSelected									();
		void																	SetSelected									(bool isselected);		
		
		bool																	IsVisible										();
		void																	SetVisible									(bool isvisible);		

		XDWORD																GetRoundRect								();
    void																	SetRoundRect								(XDWORD roundrect);

		XDWORD																IsBlinking									();
		bool																	SetBlink										(XDWORD timeblink);
		bool																	GetStateBlink								();
		bool																	SwitchStateBlink					  ();
		XTIMER*																GetTimerBlink								();	

		UI_ELEMENT_TRANSITION_STATE_SHOW			GetTransitionStateShow			();
		void																	SetTransitionStateShow			(UI_ELEMENT_TRANSITION_STATE_SHOW	 transitionstateshow);

		bool																	HasScroll										();
		void																	SetHasScroll								(bool hasscroll);

		XSTRING*															GetExtra										();	

		bool																	MustReDraw									();
		void																	SetMustReDraw								(bool mustredraw);
			
		XVECTOR<UI_ELEMENT*>*									GetComposeElements					();
		bool																	DeleteAllComposeElements		();

		bool																	SetToRedraw									(bool recursive);

  private:

		void																	Clean												();

		XSTRING																name;
		UI_ELEMENT_TYPE												type;
		XSTRING																type_string;
		XSTRING																class_names_raw;											// full "class" attribute as authored
		XVECTOR<XSTRING*>											class_names;													// owned split view (one XSTRING* per class)
		UI_ELEMENT_CHROMEROLE									chromerole;
		
		UI_ELEMENT*														father;	
		bool																	isdetached;
		
		UI_COLOR															color;
		UI_COLOR															backgroundcolor;
		bool																	color_set;                                 // step 6: true iff "color" was authored
		bool																	background_color_set;                      // step 6: true iff "bckgrdcolor" was authored

		// Step 7: box-shadow. box_shadow_set flags whether the shadow layer must be drawn at all; when false
		// the other fields are irrelevant and the skin skips the extra draw call and the rebuild-area expansion.
		bool																	box_shadow_set;
		double																shadow_offset_x;
		double																shadow_offset_y;
		double																shadow_blur;                               // parsed and stored; ignored by render for now
		UI_COLOR															shadow_color;

		double																x_position;
		double																y_position;

		XDWORD																z_level;

		UI_ELEMENT_TYPE_DIRECTION							direction;

		double																x_positionwithscroll;
		double																y_positionwithscroll;

		UI_BOUNDARYLINE												boundaryline;

		double																margin[UI_ELEMENT_MARGIN_MAX];
		double																padding[UI_ELEMENT_MARGIN_MAX];                // step 4: CSS padding (LEFT/RIGHT/UP/DOWN)
		double																border_width;                                  // step 4: -1 = unset, 0 = no stroke, >0 = px

		UI_COLOR															border_color;                                  // step 5: authored border colour (see border_color_set)
		bool																	border_color_set;                              // step 5: true iff border_color was authored
		double																border_radius[UI_ELEMENT_BORDER_CORNER_MAX];   // step 5: per-corner radius; -1 = fallback to roundrect

		bool																	isactive;
		bool																	ispreselect;		
		bool																	isselected;
		bool																	isvisible;

		XDWORD																roundrect;	

		XDWORD																blink_time;
		bool																	blink_state;
		XTIMER*																blink_xtimer;	
		XDWORD																blink_nchanges;		

		UI_ELEMENT_TRANSITION_STATE_SHOW			transitionstateshow;

		bool																	hasscroll;

		bool																	mustredraw;

		XSTRING																extra;
		
		XVECTOR<UI_ELEMENT*>									compose_elements;

		// --- Baseline visual snapshot for pseudo-class state re-resolution -------------------------------------------
		// Captured once at end-of-load via SnapshotStyleVisual(); restored by ReapplyStyleVisual() before
		// layering state-active CSS rules on top. Only used when style_has_state_rules is true.
		UI_COLOR															snapshot_color;
		UI_COLOR															snapshot_backgroundcolor;
		XDWORD																snapshot_roundrect;
		bool																	snapshot_taken;
		bool																	style_has_state_rules;
};




/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/





