#include "fortify.h"

#include <stdio.h>
#include <stdbool.h>

#include <XPLMDefs.h>
#include <XPLMGraphics.h>
#include <XPLMDisplay.h>
#include <XPLMUtilities.h>
#include <XPLMProcessing.h>
#include <XPLMDataAccess.h>
#include <XPLMPlugin.h>
#include <XPLMNavigation.h>

#include "xsa-menu.h"
#include "xsa-nav.h"
#include "xsa-ng-types.h"
#include "xsa-ng-render.h"
#include "freeglut.h"

//static XPLMHotKeyID details_hot_key_id = NULL;
//static XPLMHotKeyID toggle_hot_key_id = NULL;

static struct {
    struct {
        float X; // plane
        float Y; // plane
        float Z; // plane
		float cX; // camera
		float cY; // camera
		float cZ; // camera
        double Lat;
        double Lon;
        double Elev;
        float Viz;
        float ViewHdg;
        float Psi;
        float RunningTime;
    } v;
    XPLMDataRef X;
    XPLMDataRef Y;
    XPLMDataRef Z;
    XPLMDataRef cX;
    XPLMDataRef cY;
    XPLMDataRef cZ;
    XPLMDataRef	Lat;
    XPLMDataRef	Lon;
    XPLMDataRef	Elev;
    XPLMDataRef Viz;
    XPLMDataRef ViewHdg;
    XPLMDataRef Psi;
    XPLMDataRef RunningTime;
} dr;

static int list_count = 0;
static bool debug_dump = false;

/*
 * prototypes
 */
static void XSADrawNav();
static void XSAUpdateState();
static int XSADraw(XPLMDrawingPhase,int,void*);
static void key(void*);

int XSADraw(XPLMDrawingPhase inPhase, int inIsBefore, void* inRefcon) {
    XSAUpdateState();
    d_XSAWorldPoint point;
    point.height = dr.v.Elev;
    point.lat = dr.v.Lat;
    point.lon = dr.v.Lon;
    XSATouchNavDB(point);
    if (draw_flags) {
        XSADrawNav();
//        if(draw_state) { XSADrawState(); }
    }
    debug_dump = false;
    return 1;
}

void XSAUpdateState() {
    dr.v.X = XPLMGetDataf(dr.X);
    dr.v.Y = XPLMGetDataf(dr.Y);
    dr.v.Z = XPLMGetDataf(dr.Z);
    // dr.v.cX = XPLMGetDataf(dr.cX);
    // dr.v.cY = XPLMGetDataf(dr.cY);
    // dr.v.cZ = XPLMGetDataf(dr.cZ);
    dr.v.Lat = XPLMGetDatad(dr.Lat);
    dr.v.Lon = XPLMGetDatad(dr.Lon);
    dr.v.Elev = XPLMGetDatad(dr.Elev);
    dr.v.Viz = XPLMGetDataf(dr.Viz);
    dr.v.ViewHdg = XPLMGetDataf(dr.ViewHdg);
    dr.v.Psi = XPLMGetDataf(dr.Psi);
    dr.v.RunningTime = XPLMGetDataf(dr.RunningTime);
    XPLMSetGraphicsState(0, 0, 0, 0, 0, 1, 1);
}

void XSADrawState() {
    char buf[512];
    strcpy(buf, "XSA-NG");
    switch (draw_flags) {
        case xsaNavTypeAirport:
            strcat(buf, " Airports");
            break;
        case xsaNavTypeVOR:
            strcat(buf, " Nav Aids");
            break;
        case xsaNavTypeFix:
            strcat(buf, " Fixes");
            break;
        case xsaNavTypeUSGSCivil:
            strcat(buf, " Civil USGS");
            break;
        case xsaNavTypeUSGSMunicipal:
            strcat(buf, " Municipal USGS");
            break;
        case xsaNavTypeUSGSTerrain:
            strcat(buf, " Terrain USGS");
            break;
    }
    if (detail_draw_flags != 0 && !(draw_flags == xsaNavTypeFix)) {
        strcat(buf, " (w/ Details)");
    }
    d_XSA3DPoint point;
    point.x = dr.v.X;
    point.y = dr.v.Y;
    point.z = dr.v.Z;
    XSADrawStringHUD(point,dr.v.ViewHdg,(const unsigned char*)buf);
}

void XSADrawNav() {
    double max_viz = (dr.v.Viz + XSA_NAUTICAL_MILE_METERS);
    double max_viz_nm = max_viz / XSA_NAUTICAL_MILE_METERS;
    d_XSA3DPoint gl_point;
    navinfo_t stack_nav;

    list_count = 0;

    node_navinfo* list = XSAGetNavDBList();

    if (debug_dump) {
        XPLMDebugString("XSADrawNav() : fetched list\n");
        if (list) {
            XPLMDebugString("XSADrawNav() : list is NOT null\n");
        }
    }
    while (list->node != NULL) {
        list_count++;

        navinfo_t* ptr_nav = list->node;
        if (debug_dump) {
            char s[1024];
            sprintf(s, "XSADrawNav() : processing nav name %s type %i\n", ptr_nav->name, ptr_nav->xsaType);
            XPLMDebugString(s);
        }
        list = list->next;

        // reload lat/lon for moving targets ...
        if (ptr_nav->xsaType == xsaNavTypeShip) {
            XSALoadNavRef(ptr_nav->ref, ptr_nav);
        }

        d_XSAWorldPoint nav_point;
        nav_point.lat = ptr_nav->lat;
        nav_point.lon = ptr_nav->lon;
        d_XSAWorldPoint point;
        point.height = dr.v.Elev;
        point.lat = dr.v.Lat;
        point.lon = dr.v.Lon;
        double range = XSADistance(nav_point, point, 'N');

        if (range > max_viz_nm) {
            if (debug_dump) {
                char s[1024];
                sprintf(s, "XSADrawNav() : <> range for nav name %s type %i\n", ptr_nav->name, ptr_nav->xsaType);
                XPLMDebugString(s);
            }
            continue;
        }

        // clean on stack nav struct, in case ...
        memset(&stack_nav, 0, sizeof (navinfo_t));

        // glColor3f(0.0, 0.0, 1.0); // blue
        // glColor3f(0.0,1.0,0.0); // green
        switch (ptr_nav->xsaType) {
            case xsaNavTypeUSGSCivil:
                if (!((draw_flags & xsaNavTypeUSGSCivil) == xsaNavTypeUSGSCivil)) {
                    continue;
                }
                glColor3f(0.0, 1.0, 0.0); // green
                break;

            case xsaNavTypeUSGSMunicipal:
                if (!((draw_flags & xsaNavTypeUSGSMunicipal) == xsaNavTypeUSGSMunicipal)) {
                    continue;
                }
                glColor3f(0.0, 1.0, 0.0); // green
                break;

            case xsaNavTypeUSGSTerrain:
                if (!((draw_flags & xsaNavTypeUSGSTerrain) == xsaNavTypeUSGSTerrain)) {
                    continue;
                }
                glColor3f(0.0, 1.0, 0.0); // green
                break;

            case xsaNavTypeAirport:
                if (!((draw_flags & xsaNavTypeAirport) == xsaNavTypeAirport)) {
                    continue;
                }
                glColor3f(1.0, 0.0, 0.0); // red
                break;

            case xsaNavTypeVOR:
                if (!((draw_flags & xsaNavTypeVOR) == xsaNavTypeVOR)) {
                    continue;
                }
                glColor3f(1.0, 0.0, 1.0); // magenta
                break;

            case xsaNavTypeShip:
                if (!((draw_flags & xsaNavTypeVOR) == xsaNavTypeVOR)) {
                    continue;
                }
                glColor3f(1.0, 0.0, 1.0); // magenta
                break;

            case xsaNavTypeNDB:
                if (!((draw_flags & xsaNavTypeVOR) == xsaNavTypeVOR)) {
                    continue;
                }
                glColor3f(1.0, 0.0, 1.0); // magenta
                break;

            case xsaNavTypeFix:
                if (!((draw_flags & xsaNavTypeFix) == xsaNavTypeFix)) {
                    continue;
                }
                if (ptr_nav->height < point.height) {
                    memcpy(&stack_nav, ptr_nav, sizeof (navinfo_t));
                    stack_nav.height = point.height;
                    ptr_nav = &stack_nav;
                }
                glColor3f(0.0, 0.0, 1.0); // blue
                break;

            default:
                continue;
        }

        XPLMWorldToLocal(
                ptr_nav->lat,
                ptr_nav->lon,
                ptr_nav->height,
                &gl_point.x,
                &gl_point.y,
                &gl_point.z
                );

		const double clamp_range = 1.0;
		const double scale_reduce = 0.75;
		// set an initial scale for drawing points and text
		double scale = clamp_range;
		// if point is greater than 'clamp_range' away
		if(range > clamp_range) {
			// using the range of the point as a way to set a drawing scale ...
			// given the range of the point, take a fraction the distance to the point,
			// after the 'clamp_range' and add that to the 'clamp_range', as the scale
			scale = clamp_range + ((range - clamp_range)*scale_reduce);
		}

        XSARenderShapeDiamond(gl_point.x, gl_point.y + 16.0, gl_point.z,scale);

        if (ptr_nav->xsaType != xsaNavTypeFix && detail_draw_flags != 0) {
            // nav name,nav id,frequency,distance etc ...
            static char* buf = (char*) malloc(1024 * sizeof (unsigned char));
            memset(buf, 0, 1024 * sizeof (unsigned char));
            // scratch
            static char* bufbuf = (char*) malloc(5 * sizeof (char));

            if ((detail_draw_flags & DETAILS_DRAW_NAME) == DETAILS_DRAW_NAME) {
                strcat(buf, ptr_nav->name);
            }

            if ((detail_draw_flags & DETAILS_DRAW_ID) == DETAILS_DRAW_ID) {
                if (strlen(buf) > 0) {
                    strcat(buf, " ");
                }
                strcat(buf, ptr_nav->id);
            }

            if ((detail_draw_flags & DETAILS_DRAW_DISTANCE) == DETAILS_DRAW_DISTANCE) {
                if (strlen(buf) > 0) {
                    strcat(buf, " ");
                }
                strcat(buf, "(");
                memset(bufbuf, 0, 5 * sizeof (char));
                sprintf(bufbuf, "%.1f", range);
                strcat(buf, bufbuf);
                strcat(buf, ")");
            }

			const double scale_reduce_text = 0.10;
            gl_point.y = gl_point.y + (32.0*scale);
            XSADrawString(gl_point,
							dr.v.ViewHdg,
							scale*scale_reduce_text,
							(unsigned char*)buf);
        }
    }
}

PLUGIN_API int XPluginStart(char* outName, char* outSig, char* outDesc) {
    strcpy(outName, "xsa-ng");
    strcpy(outSig, "com.dsrts.xsa-ng");
    strcpy(outDesc, "XSA-NG v1.1 (tpelton@gmail.com) built : "__DATE__);
    dr.X = XPLMFindDataRef("sim/flightmodel/position/local_x");
    dr.Y = XPLMFindDataRef("sim/flightmodel/position/local_y");
    dr.Z = XPLMFindDataRef("sim/flightmodel/position/local_z");
    dr.cX = XPLMFindDataRef("sim/graphics/view/view_x");
    dr.cY = XPLMFindDataRef("sim/graphics/view/view_y");
    dr.cZ = XPLMFindDataRef("sim/graphics/view/view_z");
    dr.Lat = XPLMFindDataRef("sim/flightmodel/position/latitude");
    dr.Lon = XPLMFindDataRef("sim/flightmodel/position/longitude");
    dr.Elev = XPLMFindDataRef("sim/flightmodel/position/elevation");
    dr.Viz = XPLMFindDataRef("sim/graphics/view/visibility_effective_m");
    dr.ViewHdg = XPLMFindDataRef("sim/graphics/view/view_heading");
    dr.Psi = XPLMFindDataRef("sim/flightmodel/position/psi");
    dr.RunningTime = XPLMFindDataRef("sim/time/total_running_time_sec");

    //	details_hot_key_id = XPLMRegisterHotKey(XPLM_VK_F12,xplm_DownFlag,"Toggle Details",key,&details_hot_key_id);
    //	toggle_hot_key_id = XPLMRegisterHotKey(XPLM_VK_F11,xplm_DownFlag,"Toggle Type",key,&toggle_hot_key_id);

    XPLMRegisterDrawCallback(XSADraw, xplm_Phase_Objects, 0, NULL);

    XSARenderInit();
    XSAMenuInit();

    return 1;
}

PLUGIN_API void	XPluginStop(void) {}

PLUGIN_API int XPluginEnable(void) { return 1; }

PLUGIN_API void XPluginDisable(void) {}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, long inMessage, void* inParam) {
    if (inFromWho == XPLM_PLUGIN_XPLANE) {
        switch (inMessage) {
            case XPLM_MSG_PLANE_LOADED:
                if ((int) inParam == XPLM_PLUGIN_XPLANE) {
                    // XPLMDebugString("plane loaded.\n");
                    break;
                }
        }
    }
}

/********************
 * utilities
 *******************/
void key(void* inRefCon) {
	/*
	if(inRefCon == &details_hot_key_id) {
		toggleDetail();
	}
	 */
    /*
	if(inRefCon == &toggle_hot_key_id) {
		toggleType();
    }
    */

}

void dump() {
	char buf[512];
	sprintf(buf,"draw count : %d\n",list_count);
	XPLMDebugString(buf);
	// Fortify_ListAllMemory();
}
