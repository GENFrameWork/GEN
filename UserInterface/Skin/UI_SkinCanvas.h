/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       UI_SkinCanvas.h
* 
* @class      UI_SKINCANVAS
* @brief      User Interface Skin Canvas class
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

#include "XRect.h"
#include "XPath.h"

#include "GRP2DRebuildAreas.h"

#include "UI_Element.h"
#include "UI_Skin.h"



/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


enum UI_SKINCANVAS_TYPE
{
  UI_SKINCANVAS_TYPE_UNKNOWN          =  0  ,
  UI_SKINCANVAS_TYPE_FLAT                   ,
};


#define UI_SKINCANVAS_NAME_UNKNOWN					__L("")
#define UI_SKINCANVAS_NAME_FLAT							__L("FLAT")


#define UI_SKINCANVAS_PRESELECT_MAXEDGE		4
#define UI_SKINCANVAS_EDIT_MAXEDGE				10	




/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class GRPSCREEN;
class GRPVIEWPORT;
class GRP2DCANVAS;
class GRP2DPATH;
class UI_LAYOUT;
class UI_ELEMENT_TEXTBOX;
class UI_ELEMENT_PROGRESSBAR;
class UI_PROPERTY_SCROLLEABLE;


class UI_SKINCANVAS_REBUILDAREAS : public GRP2DREBUILDAREAS
{
	public:
																		  UI_SKINCANVAS_REBUILDAREAS							(GRPSCREEN* screen, int viewportindex = 0);
    virtual													 ~UI_SKINCANVAS_REBUILDAREAS							();

		bool															RebuildAllAreas													();    
		bool															RebuildAllAreas													(UI_LAYOUT* layout);
		bool															RebuildAllAreas													(UI_ELEMENT* element);

    bool                              CreateRebuildArea												(double x, double y, double width, double height, UI_ELEMENT* element);

    GRPBITMAP*												GetBitmap																(double x, double y, double width, double height);
    void															PutBitmapNoAlpha												(double x, double y, GRPBITMAP* bitmap);

    // Retarget GetBitmap/PutBitmapNoAlpha for modal offscreen composition (option B).
    void                              SetTargetCanvas                         (GRP2DCANVAS* newcanvas);
    GRP2DCANVAS*                      GetTargetCanvas                         ();
  
		GRP2DREBUILDAREA*									GetRebuildAreaByElement									(UI_ELEMENT* element);					

	private:

		void															Clean																		();

		void															MarkOverlappingAreasDirty							(GRP2DREBUILDAREA* area, GRPBITMAP* bitmap, int excludeindex);
		void															MarkElementSubtreeDirty									(UI_ELEMENT* element);

		GRPSCREEN*												screen;
		int																viewportindex;
		GRP2DCANVAS*											canvas;
};


// Plain last-known-bounds record for one progress-bar widget's element_progressrect/element_animation, keyed
// by the progress-bar element itself -- see the ProgressBounds_Find/Remember/HasChanged declarations and the
// GHOST-FILL FIX comment in Draw_ProgressBar() (UI_SkinCanvas.cpp). Deliberately a plain struct rather than
// reusing GRP2DREBUILDAREA: there is no bitmap or restore/discard lifecycle here, just four numbers per
// sub-element to compare against on the next tick.
class UI_PROGRESSBAR_LASTBOUNDS
{
	public:
																			UI_PROGRESSBAR_LASTBOUNDS	() : element(NULL), rectx(0.0), recty(0.0), rectwidth(0.0), rectheight(0.0),
																	                                     animx(0.0), animy(0.0), animwidth(0.0), animheight(0.0) { }

		UI_ELEMENT*								element;
		double										rectx;
		double										recty;
		double										rectwidth;
		double										rectheight;
		double										animx;
		double										animy;
		double										animwidth;
		double										animheight;
};


class UI_SKINCANVAS : public UI_SKIN, public UI_SKINCANVAS_REBUILDAREAS
{
  public:
																		  UI_SKINCANVAS														(GRPSCREEN* screen, int viewportindex = 0);
    virtual													 ~UI_SKINCANVAS														();

		static void												GetScrollViewportSize										(UI_ELEMENT* element, double& width, double& height);

		GRPSCREEN*                        GetScreen																(); 
		GRP2DCANVAS*                      GetCanvas																();
    // Option B: while rebuilding the modal offscreen buffer, Draw_* must target that canvas.
    void                              SetCanvasOverride                       (GRP2DCANVAS* override_canvas); 
		
		bool															LoadFonts																();

		bool															GetFatherSize														(UI_ELEMENT* element, double& width, double& height);
		double														GetPositionWithoutDefine								(double position);
		bool															SetAroundFromSubElements								(UI_ELEMENT* element);
		bool															AddPositionSubElements									(UI_ELEMENT* element);
		bool															CalculePosition													(UI_ELEMENT* element, double fatherwidth, double fatherheight, bool adjustsizemargin = false);

    double														GetWidthString													(XCHAR* string, XDWORD sizefont = 12);  
    double														GetHeightString													(XCHAR* string, XDWORD sizefont = 12);  
				           
	  virtual bool                      CalculateBoundaryLine_Scroll						(UI_ELEMENT* element, bool adjustsizemargin = false);
	  virtual bool                      CalculateBoundaryLine_Text							(UI_ELEMENT* element, bool adjustsizemargin = false);
	  virtual bool                      CalculateBoundaryLine_TextBox						(UI_ELEMENT* element, bool adjustsizemargin = false);
	  virtual bool                      CalculateBoundaryLine_Image   					(UI_ELEMENT* element, bool adjustsizemargin = false);
		virtual bool                      CalculateBoundaryLine_Animation					(UI_ELEMENT* element, bool adjustsizemargin = false);
		virtual bool                      CalculateBoundaryLine_Option						(UI_ELEMENT* element, bool adjustsizemargin = false);
		virtual bool                      CalculateBoundaryLine_MultiOption				(UI_ELEMENT* element, bool adjustsizemargin = false);
	  virtual bool                      CalculateBoundaryLine_Button						(UI_ELEMENT* element, bool adjustsizemargin = false);
		virtual bool                      CalculateBoundaryLine_CheckBox					(UI_ELEMENT* element, bool adjustsizemargin = false);
		virtual bool                      CalculateBoundaryLine_EditText					(UI_ELEMENT* element, bool adjustsizemargin = false);	  
	  virtual bool                      CalculateBoundaryLine_Form							(UI_ELEMENT* element, bool adjustsizemargin = false);
		virtual bool                      CalculateBoundaryLine_Menu							(UI_ELEMENT* element, bool adjustsizemargin = false);
		virtual bool                      CalculateBoundaryLine_ListBox						(UI_ELEMENT* element, bool adjustsizemargin = false);
		virtual bool                      CalculateBoundaryLine_ProgressBar				(UI_ELEMENT* element, bool adjustsizemargin = false);
    virtual bool                      CalculateBoundaryLine_ProgressRadial		(UI_ELEMENT* element, bool adjustsizemargin = false);
		virtual bool                      CalculateBoundaryLine_ProgressImage			(UI_ELEMENT* element, bool adjustsizemargin = false);

		// ROOT-CAUSE FIX (2026-09, fourth pass): positions element_progressrect/element_animation/element_text
		// against element_progressbar's OWN box (already resolved by the caller) and applies the allocationtext-
		// driven shift (DOWN/UP/LEFT move the rect/animation to free up room for the caption -- see the switch in
		// CalculateBoundaryLine_ProgressBar()). Extracted out of that function so UI_MANAGER::RefreshFlexProgressBarTracks()
		// (UI_Manager.cpp) can call the SAME logic after it re-syncs element_progressrect to a flex-resolved father
		// position, instead of duplicating (and, as found live, forgetting half of) this shift -- see that
		// function's own banner and this method's .cpp comment for the defect this fixes.
		void                               ReapplyProgressBarAllocationLayout			(UI_ELEMENT_PROGRESSBAR* element_progressbar, bool adjustsizemargin = false);


		virtual bool                      SetElementPosition											(UI_ELEMENT* element, double x_position, double y_position);

		// HIDE-RESTORE FIX (2026-09): see UI_Skin.h/.cpp for the full rationale (Draw()'s own "invisible -> skip"
		// early return never gave Draw_Form() a chance to restore its persistent formbackdrops cache once an
		// element like a sidebar "nav-band" is hidden, so its last-painted pixels stayed on screen forever).
		// Overrides the no-op base: restores the element's own cached true backdrop (currently: "form"/"menu",
		// via formbackdrops -- the same cache Draw_Form()'s ALPHA-DARKENING FIX already restores from on every
		// ordinary visible redraw) when one exists, so hiding it erases it exactly as it would have if the
		// element had been allowed one last real redraw instead of being skipped.
		virtual bool                      RestoreOnHide														(UI_ELEMENT* element);

		// COMPOSITION-RESET (2026-09): drop EVERY persistent "true backdrop" cache plus the one-shot rebuild
		// areas. Required whenever the canvas under the widgets is fully rewritten (Layout_PutBackground,
		// virtual-keyboard modal show/hide, etc.): those caches were captured against the PREVIOUS composition,
		// and restoring them with PutBitmapNoAlpha after a fresh background paint produces opaque white/stale
		// rectangles over menus, progress tracks and chrome (reproduced in UI_Options when the virtual keyboard
		// appears). Next Draw_* will re-capture from the new background. Framework-wide -- not example-specific.
		void                               InvalidateCompositionCaches							();

		// Diagnostics / unit tests: total entries across every persistent backdrop cache (0 after a successful
		// InvalidateCompositionCaches). Does not include one-shot rebuild areas.
		XDWORD                             CompositionCacheCount									();

	  virtual bool                      Draw_Scroll															(UI_ELEMENT* element);
	  virtual bool                      Draw_Text																(UI_ELEMENT* element);
		virtual bool											Draw_TextBox														(UI_ELEMENT* element);
	  virtual bool                      Draw_Image  														(UI_ELEMENT* element);
		virtual bool                      Draw_Animation													(UI_ELEMENT* element);
		virtual bool                      Draw_Option															(UI_ELEMENT* element);
		virtual bool                      Draw_MultiOption												(UI_ELEMENT* element);
	  virtual bool                      Draw_Button															(UI_ELEMENT* element);
		virtual bool											Draw_CheckBox														(UI_ELEMENT* element);
		virtual bool											Draw_EditText														(UI_ELEMENT* element);		
	  virtual bool                      Draw_Form																(UI_ELEMENT* element);
		virtual bool											Draw_Menu																(UI_ELEMENT* element);
		virtual bool											Draw_ListBox														(UI_ELEMENT* element);
		virtual bool											Draw_ProgressBar												(UI_ELEMENT* element);
    virtual bool                      Draw_ProgressRadial											(UI_ELEMENT* element);
    virtual bool                      Draw_ProgressImage											(UI_ELEMENT* element);

  protected: 

		bool															GetFontSize															(XCHAR* text, XDWORD& width, XDWORD& height);
		bool															SetFontSize															(XDWORD size);

		bool															DrawBackgroundColor											(UI_ELEMENT* element, GRP2DCANVAS* canvas, double x_position, double y_position);

		// Shared box-model drawing helpers: generic across widget types (built against base UI_ELEMENT accessors
		// only), so any subclass' Draw_X can reuse them instead of duplicating per-corner-radius / box-shadow math.
		// Moved here (were file-local static helpers) so UI_SKINCANVAS_FLAT can reach box-model parity with the
		// default skin without a second copy of this logic.
		static void												AppendRoundRectPathPerCorner						(GRP2DPATH& path, double minx, double miny, double maxx, double maxy, double rTL, double rTR, double rBR, double rBL);
		static void												DrawElementBoxShadow										(GRP2DCANVAS* canvas, UI_ELEMENT* element, double x_position, double y_position);

		bool															PreDrawFunction													(UI_ELEMENT* element, GRP2DCANVAS* canvas, XRECT& clip_rect, double& x_position, double& y_position, XDWORD edge = 5);
		bool															PostDrawFunction												(UI_ELEMENT* element, GRP2DCANVAS* canvas, XRECT& clip_rect, double  x_position, double  y_position);

		bool															DrawScrollBars													(UI_ELEMENT* element, UI_PROPERTY_SCROLLEABLE* scrolleable, GRP2DCANVAS* canvas, double x_position, double y_position);
		bool															ResolveScrollPolicy											(UI_ELEMENT* element, UI_PROPERTY_SCROLLEABLE* scrolleable);
			
		double														TextBox_SizeLine												(UI_ELEMENT_TEXTBOX* element_textbox, GRP2DCANVAS* canvas, double x_position, double y_position, int nline, XSTRING& characterstr, XDWORD index_char, XVECTOR<UI_SKIN_TEXTBOX_PART*>& parts);
		bool															TextBox_GenerateLines										(UI_ELEMENT_TEXTBOX* element, GRP2DCANVAS* canvas, double x_position, double y_position, XVECTOR<UI_SKIN_TEXTBOX_PART*>& parts); 

		#ifdef USERINTERFACE_DEBUG
		bool															Debug_Draw															(UI_ELEMENT* element, double x_position, double y_position);	
		#endif

		XDWORD														fontsize;
    GRPSCREEN*												screen;
	  int																viewportindex;
		XPATH															fontpathfile;
    GRP2DCANVAS*                      canvas_override;

	private:

		void															Clean																		();		

		// Persistent "true backdrop" cache for an AUTO-sized progress bar's FULL natural box (rect + gap +
		// caption together) -- see the GHOST-FILL FIX comment in Draw_ProgressBar() (UI_SkinCanvas.cpp) for
		// the full rationale. Captured once (via the inherited GetBitmap()), the very first time a given
		// progress bar is ever drawn -- before its rect, animation or caption have painted anything, so it is
		// guaranteed the true backdrop -- and kept for the whole remaining life of the widget, so every later
		// real redraw can paint those REAL captured pixels back (via the inherited PutBitmapNoAlpha())
		// immediately before the rect/animation/caption are (re)painted, genuinely erasing whatever was there
		// (including ink left behind in a strip the rect no longer covers once CalculateBoundaryLine_
		// ProgressBar() shifts it to make room for the caption) instead of painting on top of it. Never a
		// synthetic/flat fill colour: only ever real, previously-captured pixels. Deliberately kept OUTSIDE
		// UI_SKINCANVAS_REBUILDAREAS::areas: that vector's whole lifecycle (create, restore-or-discard-next
		// -frame) is one-shot by design (see RebuildAllAreas()'s orphan-discard comment) and is not meant to
		// hold anything long-lived; reusing GRP2DREBUILDAREA here is just for its ready-made (xpos, ypos,
		// bitmap, extra_data-as-key) shape, not for its lifecycle.
		GRP2DREBUILDAREA*								ProgressBackdrop_Find										(UI_ELEMENT* element);
		bool													ProgressBackdrop_Capture								(UI_ELEMENT* element, double x, double y, double width, double height);

		XVECTOR<GRP2DREBUILDAREA*>				progressbackdrops;

		// Last-known element_progressrect/element_animation bounds for each progress-bar widget tracked in
		// progressbackdrops above -- see the GHOST-FILL FIX comment in Draw_ProgressBar() (UI_SkinCanvas.cpp).
		// Lets that fix restore the widget's cached backdrop ONLY on the tick where the rect (and/or animation)
		// actually moved, instead of on every tick the widget happens to be marked dirty -- this progress bar
		// can stay marked dirty for many ticks in a row after a single real value change (a separate, pre-
		// existing condition -- see that same comment), and restoring the backdrop on every one of those ticks
		// regardless of whether anything moved was confirmed live to erase the caption's own, already-correct
		// glyph far more often than it gets repainted.
		UI_PROGRESSBAR_LASTBOUNDS*				ProgressBounds_Find											(UI_ELEMENT* element);
		void													ProgressBounds_Remember								(UI_ELEMENT* element, double rectx, double recty, double rectwidth, double rectheight, double animx, double animy, double animwidth, double animheight);
		bool													ProgressBounds_HasChanged							(UI_ELEMENT* element, double rectx, double recty, double rectwidth, double rectheight, double animx, double animy, double animwidth, double animheight);

		XVECTOR<UI_PROGRESSBAR_LASTBOUNDS*>	progressbarlastbounds;

		// ALPHA-DARKENING FIX (2026-09): persistent "true backdrop" cache for a "form"/"menu" element's own box
		// (Draw_Form(), shared by Draw_Menu()) -- same rationale and same real capture/restore primitives
		// (GetBitmap()/PutBitmapNoAlpha()) as progressbackdrops above, generalized to any element whose own
		// fill/border/box-shadow painting uses a translucent (alpha < 255) color. Draw_Form() ALWAYS paints by
		// blending (AGG "over" compositing), never a plain overwrite, so repainting the same nominal translucent
		// color on top of an earlier repaint of itself silently compounds toward full opacity: confirmed live on
		// "ListBoxMenuID" (color="black,50") via canvas->GetBitmap() reads, alpha 127 -> 191 -> 242 across three
		// consecutive real redraws triggered by unrelated UI interaction elsewhere in the same layout (a global
		// Elements_SetToRedraw() call). Captured once, the very first time a given form/menu is drawn (before its
		// own shadow/fill/border/children have painted anything), and restored immediately before every later
		// real redraw so translucent blending always starts fresh from the true backdrop instead of compounding.
		GRP2DREBUILDAREA*								FormBackdrop_Find												(UI_ELEMENT* element);
		bool													FormBackdrop_Capture										(UI_ELEMENT* element, double x, double y, double width, double height);

		// ACCENT-BAR TRAIL FIX (2026-09): mirrors TextBackdrop_MatchesArea()/the RESIZE-RECAPTURE FIX in
		// Draw_Text() -- see the .cpp definition for the full root-cause writeup on why a form's own cached
		// backdrop box can legitimately differ in size from its CURRENT rebuild area, and why blindly restoring
		// it anyway (the only thing Draw_Form() did before this fix) leaves a stale, never-erased strip behind.
		bool													FormBackdrop_MatchesArea								(GRP2DREBUILDAREA* formbackdrop, double x, double y, double width, double height);
		bool													FormBackdrop_Delete										(UI_ELEMENT* element);

		// SIBLING-CONTAMINATION FIX (2026-09): mirrors OptionBackdrop_FindOverlapping() -- see its own header
		// comment. Used by Draw_Form()'s first-ever FormBackdrop capture to find an EARLIER-drawn sibling
		// form/menu element (e.g. "nav-<section>-hl", drawn just before "nav-<section>-bar" in the same frame)
		// whose own fresh paint already landed in this box, so it can be stripped back out (restored) before
		// this element bakes in a "true backdrop" snapshot forever.
		GRP2DREBUILDAREA*								FormBackdrop_FindOverlapping						(double x, double y, double width, double height);

		XVECTOR<GRP2DREBUILDAREA*>				formbackdrops;

		// ACCENT-BAR NEVER APPEARS FIX (2026-09): tracks which "form"/"menu" elements are currently hidden
		// (RestoreOnHide() ran for them and they have not been drawn visible again since). A form/menu whose
		// pointer is in this list is, by construction, about to paint genuinely NEW ink the next time it is
		// drawn visible -- exactly like its very first-ever draw, and unlike an already-visible element that
		// simply redraws because its OWN content changed (e.g. a live stat card). See the fuller root-cause
		// writeup at Draw_Form()'s OptionBackdrop_InvalidateOverlapping() call. Small linear list, same
		// small-cardinality reasoning as optionbackdrops/formbackdrops above (a typical layout has only a
		// handful of toggled-visibility forms, e.g. UI_System's 16 "nav-<section>-hl"/"-bar" elements).
		XVECTOR<UI_ELEMENT*>					formhiddentracked;

		// RADIAL CAPTION GHOSTING FIX (2026-09): persistent "true backdrop" cache for a progressradial widget's
		// own box (Draw_ProgressRadial()) -- same rationale, same real capture/restore primitives (GetBitmap()/
		// PutBitmapNoAlpha()) as progressbackdrops/formbackdrops above. Unlike the LINEAR progress bar (whose
		// caption lives OUTSIDE the rect's own rebuild area and needed its own dedicated fix in Draw_ProgressBar()),
		// the radial caption is centered INSIDE the widget's own box, so a single per-widget capture/restore here
		// is enough to cover the ring, the value arc AND the caption together. The generic per-tick rebuild-area
		// system (UI_SKINCANVAS_REBUILDAREAS) does NOT cover this on its own: a progressradial is normally dirty
		// for exactly one frame per value change and idle in between, so its area is ORPHAN-DISCARDED (deleted
		// WITHOUT restoring -- see RebuildAllAreas()'s orphan-discard comment) the very next frame; the NEXT real
		// value change then creates a brand-new area that captures whatever is CURRENTLY on screen -- which
		// already has the PREVIOUS caption's ink baked in -- and the new caption is alpha-blended straight on top
		// of it. Confirmed live via canvas->GetBitmap() reads and a temporary PreDrawFunction/RebuildAllAreas
		// trace: progressbar1's rebuild area is created fresh on every real value-change tick and is ALWAYS
		// orphan-discarded (never restored) the following frame, so the ring+caption backdrop is never actually
		// reset to the true pixels beneath it. Captured once, the very first time a given progressradial is ever
		// drawn (before its own track/arc/caps/caption have painted anything), and restored immediately before
		// every later real redraw.
		GRP2DREBUILDAREA*								RadialBackdrop_Find											(UI_ELEMENT* element);
		bool													RadialBackdrop_Capture									(UI_ELEMENT* element, double x, double y, double width, double height);

		XVECTOR<GRP2DREBUILDAREA*>				radialbackdrops;

		// TEXT CAPTION GHOSTING FIX (2026-09): persistent "true backdrop" cache for a plain text element's own
		// box (Draw_Text()) -- same rationale, same real capture/restore primitives (GetBitmap()/
		// PutBitmapNoAlpha()) as progressbackdrops/formbackdrops/radialbackdrops above, generalised here to
		// EVERY text element (not just captions nested inside a progress widget) since Draw_Text() paints glyph
		// ink purely by alpha-blending and is just as exposed to the same orphan-discard gap in the generic
		// per-tick rebuild-area system (UI_SKINCANVAS_REBUILDAREAS): a text element whose value changes via a
		// live "#[...]" placeholder is typically dirty for exactly one frame per value change and idle in
		// between, so its area is ORPHAN-DISCARDED (deleted WITHOUT restoring) the very next frame; the NEXT
		// real value change then blends its new glyph straight onto whatever is on screen -- still showing the
		// PREVIOUS glyph's ink. Confirmed live on UI_System's "uptime_seconds_value": converges from a clean
		// digit into a solid, near-opaque block within a few seconds of ticking. Captured once, the first time a
		// given text element is ever drawn, and restored immediately before every later real redraw -- EXCEPT
		// when the element's own box has changed size since the capture (an auto-width caption whose string grew/
		// shrank, e.g. "9" -> "10"), in which case the OLD, undersized entry is first restored into its own old
		// position (erasing this element's own leftover ink there) and only then discarded and re-captured fresh
		// at the new size (see TextBackdrop_MatchesArea() and the RESIZE-RECAPTURE FIX comment in Draw_Text()) --
		// a naive re-capture-without-restoring-first was tried and diagnosed live to bake the stale digit in
		// permanently, since the "current screen" at the resize instant still held the old, un-erased glyph.
		GRP2DREBUILDAREA*								TextBackdrop_Find											(UI_ELEMENT* element);
		bool													TextBackdrop_MatchesArea								(GRP2DREBUILDAREA* textbackdrop, GRP2DREBUILDAREA* ownarea);
		bool													TextBackdrop_Delete										(UI_ELEMENT* element);
		bool													TextBackdrop_Capture										(UI_ELEMENT* element, double x, double y, double width, double height);

		// STALE CAPTION BACKGROUND FIX (2026-09): discards every cached TextBackdrop entry whose box overlaps the
		// given rectangle, WITHOUT restoring it first (see the .cpp definition for the full root-cause writeup on
		// why a restore here would be actively harmful). Called from Draw_Form(), right next to the pre-existing
		// OptionBackdrop_InvalidateOverlapping() call, for the exact same reason: a form about to paint real ink
		// of its own (e.g. a "nav-<section>-hl" selection wash) makes any text element's cached "true backdrop"
		// over that same region stale, even though nothing about the text element itself changed.
		bool													TextBackdrop_InvalidateOverlapping					(double x, double y, double width, double height);

		XVECTOR<GRP2DREBUILDAREA*>				textbackdrops;

		// HOVER-WASH GHOSTING FIX (2026-09): persistent "true backdrop" cache for an option-family element's own
		// box (Draw_Option(), shared by Draw_Button()/Draw_CheckBox()/Draw_MultiOption() via delegation) -- same
		// rationale, same real capture/restore primitives (GetBitmap()/PutBitmapNoAlpha()) as progressbackdrops/
		// formbackdrops/radialbackdrops/textbackdrops above. DrawBackgroundColor() (called unconditionally by
		// Draw_Option() on every real redraw) paints the pre-select/selected background wash purely by ALPHA-
		// BLENDING onto whatever is already on the canvas, with NO backdrop cache of its own -- structurally
		// identical to the pre-fix state of the other three widget types above. UI_System's sidebar nav buttons
		// (class="nav-hit") use a CSS ":hover"/":preselect" transition (dashboard.css) that marks the button
		// MustReDraw() on EVERY tween frame while the mouse is over a row, and again while it fades out after the
		// mouse leaves -- each such frame blends the current wash color on top of whatever the LAST frame already
		// painted there instead of on top of the true backdrop, and the generic per-tick rebuild-area system
		// (UI_SKINCANVAS_REBUILDAREAS) does not help either: once the tween finishes the element goes idle and its
		// area is ORPHAN-DISCARDED (deleted WITHOUT restoring -- see RebuildAllAreas()'s own orphan-discard
		// comment), so the wash's last-painted pixels are never erased. Confirmed live on UI_System's sidebar:
		// hovering "Memoria" then moving on to "Red" (and beyond) leaves "Memoria" permanently highlighted --
		// exactly Abraham's reported "no se borra... se van seleccionando todas". Captured once, the first time a
		// given option-family element is ever drawn (before its own background wash/selection ring/animation/
		// caption have painted anything), and restored immediately before every later real redraw.
		//
		// UNLIKE formbackdrops/radialbackdrops/textbackdrops above, this cache must NOT be kept forever: an
		// option-family element's own box can OVERLAP a sibling's independently-changing visuals -- concretely,
		// UI_System's "nav-<section>-btn" is deliberately painted with a fully transparent base background
		// (alpha 0) directly ON TOP of its row's separate "nav-<section>-hl"/"-bar" selection-band elements (see
		// UI_System.cpp's UserInterface_SelectSection(), and dashboard.xml's own comments on why hl/bar/btn are
		// kept as top-level siblings instead of nesting btn's content inside hl), specifically so the blue
		// selection band shows through whenever the button itself has nothing to paint. A one-time-forever
		// capture taken while that band happened to be visible (e.g. "Resumen" is the default-active section at
		// startup, so nav-resumen-btn's very first draw captures the blue band as its "true" backdrop) would
		// permanently re-paint that stale band every time the button is later hovered, even long after the row
		// was correctly deselected by RestoreOnHide() -- confirmed live: hovering "Resumen" after clicking away
		// to a different section reintroduced a ghost blue band that UserInterface_SelectSection() had already
		// correctly cleared. Fix: forget (OptionBackdrop_Delete()) the captured anchor every time this element
		// finishes a redraw with nothing of its own left on screen -- background-color alpha 0 AND neither
		// selected nor preselected (see the caller in Draw_Option(), right after the selection/preselect ring
		// block) -- which UI_ELEMENT::UpdateTransition() guarantees eventually happens exactly once at the tail
		// of every hover fade (it snaps to the exact target value, never leaving the tween short of it). This
		// keeps the anchor alive only WITHIN a single active hover/selection transition (still preventing the
		// originally reported multi-frame compounding), while guaranteeing every NEW transition re-captures from
		// whatever is truly on screen at that moment -- correctly picking up any sibling change that happened
		// while this element was idle, instead of trusting a possibly ancient snapshot.
		//
		// MID-EPISODE STALENESS GAP (2026-09): the idle-discard above only re-validates the cache at the START
		// of a NEW hover/selection episode for THIS element -- it does not protect against a sibling changing
		// WHILE this element's own episode is still active (never idle in between), which is exactly what
		// happens on a plain click: the mouse is already hovering "nav-cpu-btn" (an ongoing, non-idle episode,
		// so no re-capture happens) at the moment the click also flips "nav-cpu-hl"/"nav-cpu-bar" to visible
		// (see UI_System.cpp's UserInterface_SelectSection()) -- both siblings occupy this button's own box.
		// "nav-cpu-hl" paints its new selection band, then, still the SAME tick (this button is drawn AFTER the
		// band in z-order, see dashboard.xml), Draw_Option() restores this button's OWN cached backdrop --
		// captured earlier, BEFORE the click, with no band -- straight over the just-painted band, and nothing
		// ever repaints it afterwards (the band element's own MustReDraw() already cleared). Confirmed live via
		// OPTBACKDROP-RESTORE tracing: this is Abraham's second reported symptom, "cuando se selecciona la
		// opcion no se hace" -- the click's own visual effect is wiped by the very button that was clicked.
		// Fix: Draw_Form() calls OptionBackdrop_InvalidateOverlapping() right before it paints any real ink of
		// its own (see the ALPHA-DARKENING FIX block), passing the exact box it is about to repaint. Any cached
		// OptionBackdrop entry whose box overlaps that rectangle is discarded there and then -- geometric, not
		// name-based, so it also protects any future option-family element sharing a box with a form/menu this
		// way, not just today's nav-<section>-btn/-hl/-bar triple. The discarded element simply falls back to
		// its normal "no cache yet" path (OptionBackdrop_Find() returns NULL) on its own next redraw, which
		// re-captures fresh from whatever the form just painted -- exactly the same, already-proven-correct
		// first-capture logic used at the start of every episode, just re-triggered mid-episode this time.
		GRP2DREBUILDAREA*								OptionBackdrop_Find											(UI_ELEMENT* element);
		bool													OptionBackdrop_Delete										(UI_ELEMENT* element);
		bool													OptionBackdrop_Capture									(UI_ELEMENT* element, double x, double y, double width, double height);
		// ACCENT-BAR NEVER APPEARS FIX (2026-09): "force" (default false, see UI_SkinCanvas.cpp) is passed true
		// only when the caller is a form/menu being drawn for the very first time in its life (e.g. UI_System's
		// "nav-<section>-bar", invisible until first selected). The REGRESSION FIX "busy" guard below is correct
		// for an ONGOING episode (an unrelated neighbour's incidental overlap must not discard a still-valid,
		// currently-painted cache) but wrong here: an option's cached backdrop cannot possibly already reflect
		// ink from an element that, by definition, has never painted anything before this exact call, no matter
		// how "busy" that option currently is. force=true skips the busy check entirely so the stale cache is
		// always discarded in that one specific case.
		bool													OptionBackdrop_InvalidateOverlapping					(double x, double y, double width, double height, bool force = false);
		GRP2DREBUILDAREA*								OptionBackdrop_FindOverlapping							(double x, double y, double width, double height);

		XVECTOR<GRP2DREBUILDAREA*>				optionbackdrops;
};




/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/




