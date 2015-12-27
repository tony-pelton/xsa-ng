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

extern XSAMenu_t XSAMenu;

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
    if (XSAMenu.draw_flags) {
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
    switch (XSAMenu.draw_flags) {
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
    if (XSAMenu.detail_draw_flags != 0 && !(XSAMenu.draw_flags == xsaNavTypeFix)) {
        strcat(buf, " (w/ Details)");
    }
    d_XSA3DPoint point;
    point.x = dr.v.X;
    point.y = dr.v.Y;
    point.z = dr.v.Z;
    XSADrawStringHUD(point,dr.v.ViewHdg,(const unsigned char*)buf);
}

void XSADrawNav() {
    list_count = 0;
    double max_viz = (dr.v.Viz + XSA_NAUTICAL_MILE_METERS);
    double max_viz_nm = max_viz / XSA_NAUTICAL_MILE_METERS;

    node_navinfo* p_nin = XSAGetNavDBList();

    if (debug_dump) {
        XPLMDebugString("XSADrawNav() : fetched list\n");
        if (p_nin) {
            XPLMDebugString("XSADrawNav() : list is NOT null\n");
        }
    }
    while (p_nin->node != NULL) {

        list_count++;
        navinfo_t* p_ni = p_nin->node;
        p_nin = p_nin->next;

		// draw flag for this type on ?
		if(!((p_ni->xsaType & XSAMenu.draw_flags) == p_ni->xsaType)) { continue; }
		
        if (debug_dump) {
            char s[1024];
            sprintf(s, "XSADrawNav() : processing nav name %s type %i\n", p_ni->name, p_ni->xsaType);
            XPLMDebugString(s);
        }

        // reload lat/lon for moving targets ...
        if (p_ni->xsaType == xsaNavTypeShip) {
            XSALoadNavRef(p_ni->ref, p_ni);
        }

		// distance calculation from plane to point
        d_XSAWorldPoint nav_point;
        nav_point.lat = p_ni->lat;
        nav_point.lon = p_ni->lon;
        d_XSAWorldPoint vantage_point;
        vantage_point.lat = dr.v.Lat;
        vantage_point.lon = dr.v.Lon;
        double range = XSADistance(nav_point, vantage_point, 'N');
		// no need to draw point, if we can't see it given viz
        if (range > max_viz_nm) {
            if (debug_dump) {
                char s[1024];
                sprintf(s, "XSADrawNav() : <> range for nav name %s type %i\n", p_ni->name, p_ni->xsaType);
                XPLMDebugString(s);
            }
            continue;
        }

		// not memset()'ing this. i think this is ok, since i'm assigning all attributes a value
		d_XSA3DPoint gl_point;
        switch (p_ni->xsaType) {
            case xsaNavTypeUSGSCivil:
            case xsaNavTypeUSGSMunicipal:
            case xsaNavTypeUSGSTerrain:
				gl_point.r = 0.0;
				gl_point.g = 1.0;
				gl_point.b = 0.0;
                break;

            case xsaNavTypeAirport:
			case xsaNavTypeHelipad:
			case xsaNavTypeSeaport:
				gl_point.r = 1.0;
				gl_point.g = 0.0;
				gl_point.b = 0.0;				
                break;

            case xsaNavTypeVOR:
            case xsaNavTypeShip:
            case xsaNavTypeNDB:
				// magenta
				gl_point.r = 1.0;
				gl_point.g = 0.0;
				gl_point.b = 1.0;
                break;

            case xsaNavTypeFix:
				// fixes will render at the same height as the users plane
				p_ni->height = dr.v.Elev;
				gl_point.r = 0.0;
				gl_point.g = 0.0;
				gl_point.b = 1.0;
                break;

            default:
                continue;
        }
        XPLMWorldToLocal(
                p_ni->lat,
                p_ni->lon,
                p_ni->height,
                &gl_point.x,
                &gl_point.y,
                &gl_point.z
                );
		gl_point.y += 16.0;
		
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
		gl_point.scale = scale;

        XSARenderShapeDiamond(&gl_point);

        if (p_ni->xsaType != xsaNavTypeFix && XSAMenu.detail_draw_flags != 0) {
            // nav name,nav id,frequency,distance etc ...
            static char* buf = (char*) malloc(1024 * sizeof (unsigned char));
            memset(buf, 0, 1024 * sizeof (unsigned char));
            // scratch
            static char* bufbuf = (char*) malloc(5 * sizeof (char));

            if ((XSAMenu.detail_draw_flags & DETAILS_DRAW_NAME) == DETAILS_DRAW_NAME) {
                strcat(buf, p_ni->name);
            }

            if ((XSAMenu.detail_draw_flags & DETAILS_DRAW_ID) == DETAILS_DRAW_ID) {
                if (strlen(buf) > 0) {
                    strcat(buf, " ");
                }
                strcat(buf, p_ni->id);
            }

            if ((XSAMenu.detail_draw_flags & DETAILS_DRAW_DISTANCE) == DETAILS_DRAW_DISTANCE) {
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
	// XPLMDebugString("xsa-ng XPluginStart()\n");
    strcpy(outName, "xsa-ng");
    strcpy(outSig, "com.dsrts.xsa-ng");
    strcpy(outDesc, "XSA-NG (tpelton@gmail.com) built : "__DATE__);
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

PLUGIN_API void	XPluginStop(void) {
	// Fortify_DumpAllMemory();
	// XPLMDebugString("xsa-ng XPluginStop()\n");
	XSAMenuSave();
}

PLUGIN_API int XPluginEnable(void) {
	// XPLMDebugString("xsa-ng XPluginEnable()\n");
	return 1;
}

PLUGIN_API void XPluginDisable(void) {
	// XPLMDebugString("xsa-ng XPluginDisable()\n");
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, long inMessage, void* inParam) {
    if (inFromWho == XPLM_PLUGIN_XPLANE) {
        switch (inMessage) {
            case XPLM_MSG_PLANE_LOADED:
				// XPLMDebugString("xsa-ng plane loaded.\n");
				break;
				
			case XPLM_MSG_AIRPORT_LOADED:
				// XPLMDebugString("xsa-ng airport loaded.\n");
				break;
				
			case XPLM_MSG_SCENERY_LOADED:
				// XPLMDebugString("xsa-ng scenery loaded.\n");
				break;
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
}
