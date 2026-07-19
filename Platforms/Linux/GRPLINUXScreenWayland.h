/**-------------------------------------------------------------------------------------------------------------------
*
* @file       GRPLINUXScreenWayland.h
*
* @class      GRPLINUXSCREENWAYLAND
* @brief      LINUX Graphics Wayland Screen class
* @ingroup    PLATFORM_LINUX
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


#ifdef LINUX_WAYLAND_ACTIVE


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/


#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

#include "xdg-shell-client-protocol.h"

#ifdef LINUX_WAYLAND_XDGDECORATION_ACTIVE
#include "xdg-decoration-unstable-v1-client-protocol.h"
#endif

#include "XVector.h"

#include "GRPScreen.h"


#ifdef GRP_OPENGL_ACTIVE
class GRPLINUXBLITGLESWAYLAND;
#endif




/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

// xdg-shell gives no equivalent of X11's synchronous XGetWindowAttributes/XQueryPointer: the
// compositor announces the size it wants via a "configure" event and the client must ack it.
// Until the first configure arrives, GEN falls back to this size (matches GRPLINUXSCREENX11's
// default window request when no explicit size was set yet).
#define GRPLINUXSCREENWAYLAND_DEFAULT_WIDTH     640
#define GRPLINUXSCREENWAYLAND_DEFAULT_HEIGHT    480

// A wl_proxy (wl_pointer, wl_keyboard...) can only have ONE listener attached at a time --
// calling wl_*_add_listener() twice on the same object silently replaces the first listener
// instead of chaining it. Since GRPLINUXSCREENWAYLAND is the one binding wl_pointer/wl_keyboard
// from the seat (so ShowCursor()/HasFocus() have something to call directly, the same way
// GRPLINUXSCREENX11 manipulates Display*/Window directly), it must ALSO be the sole listener
// owner for both -- INPLINUXDEVICEKEYBOARDWAYLAND/INPLINUXDEVICEMOUSEWAYLAND cannot install
// their own. Instead they drain these small fixed-size FIFOs every Update(), which this class
// fills from its own listener callbacks. Capacity is generous for anything a single frame could
// plausibly produce; PushXxxEvent() drops the oldest entry rather than the newest if it is ever
// exceeded, so a slow frame loses old (already stale) input rather than the most recent state.
#define GRPLINUXSCREENWAYLAND_EVENTQUEUE_SIZE   64

typedef struct
{
  XDWORD   code;        // Linux evdev BTN_* code (matches event.xbutton.button's role on X11)
  bool     pressed;

} GRPLINUXSCREENWAYLAND_BUTTONEVENT;


typedef struct
{
  XDWORD   keysym;       // xkb_keysym_t -- numerically identical to the XK_* values Xlib uses
  bool     pressed;

} GRPLINUXSCREENWAYLAND_KEYEVENT;




/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class GRP2DCANVAS;

class GRPLINUXSCREENWAYLAND : public GRPSCREEN
{
  public:
                                          GRPLINUXSCREENWAYLAND             ();
    virtual                              ~GRPLINUXSCREENWAYLAND             ();

    bool                                  Create                            (bool show);

    bool                                  Update                            (GRP2DCANVAS* canvas);
    bool                                  UpdateTransparent                 (GRP2DCANVAS* canvas);
    bool                                  Delete                            ();

    bool                                  Resize                            (int width, int height);

    bool                                  Set_Position                      (int x, int y);

    bool                                  Show                              (bool active);
    bool                                  ShowCursor                        (bool active);

    bool                                  Minimize                          (bool active);
    bool                                  Maximize                          (bool active);

    void*                                 GetHandle                         ();

    bool                                  IsDesktop                         ();
    void                                  SetIsDesktop                      (bool isdesktop);

    bool                                  HasFocus                          ();

    // Pumps the wl_display connection (flush outgoing requests, non-blocking read of any new
    // incoming events, dispatch everything already queued). Wayland has no per-window/per-mask
    // selective dequeue like XCheckWindowEvent, so unlike X11 there is a single dispatch point
    // for the WHOLE connection, shared by this screen and by INPLINUXDEVICEKEYBOARDWAYLAND /
    // INPLINUXDEVICEMOUSEWAYLAND. Calling it more than once per frame is harmless (a call with
    // nothing new pending is a cheap no-op) -- Update() calls it, and so does the Update() of
    // both input devices, exactly mirroring how all three X11 counterparts independently call
    // XCheckWindowEvent()/XQueryPointer() every frame without coordinating with each other.
    bool                                  DispatchEvents                    ();

    struct wl_display*                    GetWLDisplay                      ();
    struct wl_surface*                    GetWLSurface                      ();
    struct wl_seat*                       GetWLSeat                         ();
    struct wl_pointer*                    GetWLPointer                      ();
    struct wl_keyboard*                   GetWLKeyboard                     ();
    struct xdg_toplevel*                  GetXDGToplevel                    ();

    XDWORD                                GetLastPointerSerial              ();
    XDWORD                                GetLastKeyboardSerial             ();

    // Consumed once per frame by INPLINUXDEVICEMOUSEWAYLAND::Update() / INPLINUXDEVICEKEYBOARDWAYLAND::Update()
    // -- see the GRPLINUXSCREENWAYLAND_EVENTQUEUE_SIZE comment above for why events are queued
    // here instead of each device listening directly.
    bool                                   PopButtonEvent                    (XDWORD& code, bool& pressed);
    bool                                   PopKeyEvent                       (XDWORD& keysym, bool& pressed);
    bool                                   GetPointerPosition                (int& x, int& y);
    int                                    GetAndResetScrollDelta            ();

    // LIVE compositor-driven window size (windowwidth/windowheight below), as opposed to
    // GetWidth()/GetHeight() (inherited from GRPPROPERTIES), which after this port represents ONLY
    // the canvas' fixed design size -- see the windowwidth/windowheight member comment for the full
    // rationale. Needed by GRPLINUXBLITGLESWAYLAND (a different class) to size the wl_egl_window /
    // report the EGL native surface size.
    int                                    GetWindowWidth                    ();
    int                                    GetWindowHeight                   ();

    #ifdef GRP_OPENGL_ACTIVE
    GRPLINUXBLITGLESWAYLAND*              GetBlitGLES                       ();
    #endif

  protected:

    struct wl_display*                    display;
    struct wl_registry*                   registry;
    struct wl_compositor*                 compositor;
    struct wl_shm*                        shm;
    struct xdg_wm_base*                   xdgwmbase;
    struct wl_seat*                       seat;

    struct wl_surface*                    surface;
    struct xdg_surface*                   xdgsurface;
    struct xdg_toplevel*                  xdgtoplevel;

    #ifdef LINUX_WAYLAND_XDGDECORATION_ACTIVE
    // Bound from the registry (interface "zxdg_decoration_manager_v1") like compositor/shm/
    // xdgwmbase/seat above -- persists for the lifetime of the connection, NULL if the compositor
    // never advertises it (an optional extension, see GEN_Main_WaylandProtocols.cmake and
    // Create_Surface() for the "no native chromes on this compositor" fallback that results).
    struct zxdg_decoration_manager_v1*    decorationmanager;

    // Per-toplevel, unlike decorationmanager: created fresh in Create_Surface() (needs an
    // xdg_toplevel to attach to) and destroyed in Delete() alongside it, not in the destructor.
    struct zxdg_toplevel_decoration_v1*   toplleveldecoration;
    #endif

    struct wl_pointer*                    pointer;
    struct wl_keyboard*                   keyboard;

    // xkbcommon keymap/state, built from the wl_keyboard::keymap event (a shared-memory fd
    // holding the keymap the compositor wants this client to use). Xlib needs nothing like this:
    // its keysym tables are compiled in. Owned here (not by INPLINUXDEVICEKEYBOARDWAYLAND) for
    // the same one-listener-per-proxy reason everything else pointer/keyboard-related lives here.
    struct xkb_context*                   xkbcontext;
    struct xkb_keymap*                    xkbkeymap;
    struct xkb_state*                     xkbstate;

    // Serial of the last enter/button/key event received on this seat -- required by several
    // Wayland requests that have no X11 equivalent (e.g. wl_pointer_set_cursor(), which unlike
    // XCreatePixmapCursor()/XDefineCursor() needs the serial of the enter event it responds to).
    XDWORD                                lastpointerserial;
    XDWORD                                lastkeyboardserial;

    bool                                  configured;             // true after the first xdg_surface::configure + ack
    bool                                  haskeyboardfocus;       // pushed by wl_keyboard::enter/leave (no XGetInputFocus equivalent)
    bool                                  haspointerfocus;        // pushed by wl_pointer::enter/leave

    // LIVE compositor-driven window size, deliberately kept SEPARATE from the inherited
    // GRPPROPERTIES::width/height (GetWidth()/GetHeight()/SetSize()), which represent ONLY the
    // canvas' fixed design resolution (e.g. 1024x768), set once by the app and never touched again
    // by window-resize code -- the same invariant GRPLINUXSCREENX11 already preserves (by never
    // calling SetSize() from Resize()) and GRPWINDOWSSCREEN relies on. Prior to this port,
    // Resize()/XDGToplevel_Configure() called SetSize() directly, conflating the two and breaking
    // that invariant on Wayland only; SetSize() must never be called again from window-resize code
    // on this class, only from the app's own canvas-size setup. Updated by Resize() (locally
    // requested size) and XDGToplevel_Configure() (compositor-confirmed size), both clamped to the
    // GRPVIEWPORT_ID_MAIN viewport's max via ClampToViewportMax(). Read by
    // GRPLINUXBLITGLESWAYLAND::GetNativeWindow()/GetNativeWindowSize() (via GetWindowWidth()/
    // GetWindowHeight() below) to size the wl_egl_window / report the EGL native surface size, and
    // by this class's own Update() raster path as the "live size" to crop against.
    int                                   windowwidth;
    int                                   windowheight;

    int                                   pointerx;               // last known surface-local position, pushed by wl_pointer::motion
    int                                   pointery;
    int                                   scrolldelta;            // accumulated wl_pointer::axis (vertical) since the last GetAndResetScrollDelta()

    GRPLINUXSCREENWAYLAND_BUTTONEVENT     buttonqueue[GRPLINUXSCREENWAYLAND_EVENTQUEUE_SIZE];
    XDWORD                                buttonqueuehead;
    XDWORD                                buttonqueuetail;

    GRPLINUXSCREENWAYLAND_KEYEVENT        keyqueue[GRPLINUXSCREENWAYLAND_EVENTQUEUE_SIZE];
    XDWORD                                keyqueuehead;
    XDWORD                                keyqueuetail;

    bool                                  isdesktop;

    // Bound from wl_registry::global (interface "wl_output"), one entry per connected monitor,
    // in announcement order. There is no Xrandr-style synchronous "give me all outputs and their
    // geometry" call -- GetDesktopScreenSelected() indexes into this the same way
    // GRPLINUXSCREENX11::Create_Window() indexes into GetDesktopManager()->GetDesktopMonitors()->
    // GetMonitorsRects(), just populated by push events instead of an XRRGetScreenResources()
    // call. See GRPLINUXDesktopManager.cpp for the equivalent geometry/mode enumeration.
    XVECTOR<struct wl_output*>            outputs;

    #ifdef GRP_OPENGL_ACTIVE
    GRPLINUXBLITGLESWAYLAND*              blitgles;
    #endif

  private:

    bool                                  Create_Surface                    (bool show);

    bool                                  ConnectDisplay                    ();
    bool                                  BindGlobals                       ();

    // Shared resolution logic for the GRPVIEWPORT_ID_MAIN viewport's max size, used by both
    // ClampToViewportMax() and ApplyMaxSizeHint() below: an EXPLICIT GRPVIEWPORT::SetMaxSize() on
    // an axis wins; otherwise that axis falls back to the viewport's own declared size
    // (GetWidth()/GetHeight()), same fallback GRPLINUXSCREENX11::ApplyWMNormalHints() and
    // GRPWINDOWSSCREEN::ApplyResizeLimits() use. maxw/maxh are left untouched (caller's
    // responsibility to treat <=0 as "no cap on that axis") if no viewport is configured yet.
    void                                  ResolveViewportMax                (float& maxw, float& maxh);

    // In/out clamp of a requested (w,h) pair down to the GRPVIEWPORT_ID_MAIN viewport's resolved
    // max, per axis independently (see ResolveViewportMax() above) -- the Wayland analogue of
    // GRPWINDOWSSCREEN::ApplyResizeLimits() clamping ptMaxTrackSize, applied here in-process since
    // xdg-shell gives clients no equivalent OS-level drag-limit hook for THIS (see
    // ApplyMaxSizeHint() below for the proactive xdg_toplevel_set_max_size() request that covers
    // that side). Leaves w/h untouched on an axis with no resolved max, or if no viewport is
    // configured yet.
    void                                  ClampToViewportMax                (int& w, int& h);

    // Proactive Wayland analogue of GRPLINUXSCREENX11::ApplyWMNormalHints()/
    // GRPWINDOWSSCREEN::ApplyResizeLimits(): resolves the GRPVIEWPORT_ID_MAIN viewport's max size
    // (see ResolveViewportMax() above) and, if xdgtoplevel already exists, calls the standard
    // xdg_toplevel_set_max_size() request so the compositor's own interactive resize grab AND its
    // own maximize logic both respect it -- unlike X11's XSetWMNormalHints (a property the WM reads
    // whenever it needs to), this is a real protocol request that must be reissued, so it is called
    // both once from Create_Surface() and again on every XDGToplevel_Configure() (idempotent/cheap
    // either way). Deliberately does NOT call xdg_toplevel_set_min_size() (no OS-enforced minimum,
    // same policy as X11/Windows) -- see the Update() raster-path guard and
    // GRPLINUXBLITGLESWAYLAND::ComputePresentationScale (GL path) for how content below the
    // viewport's min is hidden instead.
    void                                  ApplyMaxSizeHint                  ();

  public:

    // wl_*_listener callbacks below are public, not private: each one is read via address-of
    // from the file-scope "static const struct wl_..._listener" aggregate tables in the .cpp
    // (grplinuxscreenwayland_registry_listener and friends), which sit OUTSIDE this class --
    // the same way a C API callback trampoline needs external visibility. A private static
    // method's address cannot be taken from outside the class, even in a same-translation-unit
    // file-scope initializer, so these must live here instead of alongside the rest of the
    // implementation-only helpers below. Nothing outside this file calls them directly.

    static void                           Registry_Global                   (void* data, struct wl_registry* registry, XDWORD name, const char* interface, XDWORD version);
    static void                           Registry_GlobalRemove              (void* data, struct wl_registry* registry, XDWORD name);

    static void                           XDGWMBase_Ping                    (void* data, struct xdg_wm_base* xdgwmbase, XDWORD serial);

    static void                           XDGSurface_Configure              (void* data, struct xdg_surface* xdgsurface, XDWORD serial);

    static void                           XDGToplevel_Configure             (void* data, struct xdg_toplevel* xdgtoplevel, int width, int height, struct wl_array* states);
    static void                           XDGToplevel_Close                 (void* data, struct xdg_toplevel* xdgtoplevel);

    static void                           Seat_Capabilities                 (void* data, struct wl_seat* seat, XDWORD capabilities);
    static void                           Seat_Name                        (void* data, struct wl_seat* seat, const char* name);

    // wl_pointer_listener callbacks (installed once, on bind, in Seat_Capabilities()).
    static void                           Pointer_Enter                     (void* data, struct wl_pointer* pointer, XDWORD serial, struct wl_surface* surface, wl_fixed_t sx, wl_fixed_t sy);
    static void                           Pointer_Leave                     (void* data, struct wl_pointer* pointer, XDWORD serial, struct wl_surface* surface);
    static void                           Pointer_Motion                    (void* data, struct wl_pointer* pointer, XDWORD time, wl_fixed_t sx, wl_fixed_t sy);
    static void                           Pointer_Button                    (void* data, struct wl_pointer* pointer, XDWORD serial, XDWORD time, XDWORD button, XDWORD state);
    static void                           Pointer_Axis                      (void* data, struct wl_pointer* pointer, XDWORD time, XDWORD axis, wl_fixed_t value);
    static void                           Pointer_Frame                     (void* data, struct wl_pointer* pointer);
    static void                           Pointer_AxisSource                (void* data, struct wl_pointer* pointer, XDWORD axissource);
    static void                           Pointer_AxisStop                  (void* data, struct wl_pointer* pointer, XDWORD time, XDWORD axis);
    static void                           Pointer_AxisDiscrete               (void* data, struct wl_pointer* pointer, XDWORD axis, int discrete);

    // wl_keyboard_listener callbacks (installed once, on bind, in Seat_Capabilities()).
    static void                           Keyboard_Keymap                   (void* data, struct wl_keyboard* keyboard, XDWORD format, int fd, XDWORD size);
    static void                           Keyboard_Enter                    (void* data, struct wl_keyboard* keyboard, XDWORD serial, struct wl_surface* surface, struct wl_array* keys);
    static void                           Keyboard_Leave                    (void* data, struct wl_keyboard* keyboard, XDWORD serial, struct wl_surface* surface);
    static void                           Keyboard_Key                      (void* data, struct wl_keyboard* keyboard, XDWORD serial, XDWORD time, XDWORD key, XDWORD state);
    static void                           Keyboard_Modifiers                (void* data, struct wl_keyboard* keyboard, XDWORD serial, XDWORD modsdepressed, XDWORD modslatched, XDWORD modslocked, XDWORD group);
    static void                           Keyboard_RepeatInfo                (void* data, struct wl_keyboard* keyboard, int rate, int delay);

    // wl_buffer_listener callback for the one-shot SHM buffers CreateSHMBuffer() creates (used
    // only by the software fallback path in Update(), when GRP_OPENGL_ACTIVE's blitgles path is
    // unavailable). Destroys the wl_buffer proxy once the compositor confirms it is done reading
    // from it -- see Buffer_Release() in the .cpp for why this cannot simply be destroyed
    // synchronously right after wl_surface_commit().
    static void                           Buffer_Release                     (void* data, struct wl_buffer* buffer);

    #ifdef LINUX_WAYLAND_XDGDECORATION_ACTIVE
    // zxdg_toplevel_decoration_v1_listener callback -- the compositor's answer to the mode
    // Create_Surface() requested via zxdg_toplevel_decoration_v1_set_mode(). Purely informational
    // (GEN has nothing finer-grained to react with -- unlike X11's per-button MWM hints, this
    // protocol is a single client-side/server-side switch, no per-decoration-piece control): logs
    // the outcome so a mismatch between requested and granted mode is visible, not silently eaten.
    static void                           Decoration_Configure               (void* data, struct zxdg_toplevel_decoration_v1* decoration, XDWORD mode);
    #endif

  private:

    void                                  PushButtonEvent                   (XDWORD code, bool pressed);
    void                                  PushKeyEvent                      (XDWORD keysym, bool pressed);

    bool                                  CreateSHMBuffer                   (int width, int height, XBYTE* buffer, struct wl_buffer** outbuffer, void** outshmdata);

    XBYTE*                                ScaleBufferNearestLetterbox       (XBYTE* src, int srcw, int srch, int dstw, int dsth);

    void                                  Clean                             ();
};




/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/




#endif

