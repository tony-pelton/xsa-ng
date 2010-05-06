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
#include <XPLMMenus.h>

#include "xsa-nav.h"
#include "xsa-ng-types.h"
#include "xsa-ng-render.h"
#include "freeglut.h"

#define XSA_NAUTICAL_MILE_METERS 1852

#define DETAILS_DRAW_NAME 1
#define DETAILS_DRAW_ID 2
#define DETAILS_DRAW_DISTANCE 4

XPLMHotKeyID details_hot_key_id = NULL;
XPLMHotKeyID toggle_hot_key_id = NULL;

XPLMMenuID menu_root = NULL;
int menu_root_idx = 0;

int menu_drawname_idx = 0;
int menu_drawdistance_idx = 0;
int menu_drawid_idx = 0;
int menu_fix_idx = 0;
int menu_airport_idx = 0;
int menu_navaid_idx = 0;
int menu_civil_idx = 0;
int menu_terrain_idx = 0;
int menu_municipal_idx = 0;

XPLMDataRef dr_X = NULL;
XPLMDataRef dr_Y = NULL;
XPLMDataRef dr_Z = NULL;

XPLMDataRef	dr_Lat = NULL;
XPLMDataRef	dr_Lon = NULL;
XPLMDataRef	dr_Elev = NULL;

XPLMDataRef dr_Viz = NULL;

XPLMDataRef dr_ViewHdg = NULL;
XPLMDataRef dr_Psi = NULL;

XPLMDataRef dr_RunningTime = NULL;

d_XSA3DPoint plane_ogl;
d_XSAWorldPoint plane_coord;

float viz = 0.0;
float view_heading = 0.0;
float running_time_secs = 0.0;
float psi = 0.0;

int list_count = 0;

bool debug_dump = false;

int draw_flags = 0;
int detail_draw_flags = 0;

float last_toggle = 0.0;

void XSADrawNav();
void XSAUpdateState();
void XSADrawState();
int XSADraw(XPLMDrawingPhase inPhase,int inIsBefore,void* inRefcon);
void XSADrawString(d_XSA3DPoint translate,double scale,const unsigned char* string);
void toggleType();
void key(void* inRefCon);
void XSAMenuHandler(void*,void*);

int XSADraw(XPLMDrawingPhase inPhase,int inIsBefore,void* inRefcon) {
	XSAUpdateState();
	XSATouchNavDB(plane_coord);
	if(draw_flags) {
		XSADrawNav();
//		if(draw_state) { XSADrawState(); }
	}
	debug_dump = false;
	return 1;
}

void XSAUpdateState() {

	plane_ogl.x = XPLMGetDataf(dr_X);
	plane_ogl.y = XPLMGetDataf(dr_Y);
	plane_ogl.z = XPLMGetDataf(dr_Z);

	plane_coord.lat = XPLMGetDatad(dr_Lat);
	plane_coord.lon = XPLMGetDatad(dr_Lon);
	plane_coord.height = XPLMGetDatad(dr_Elev);
	
	viz = XPLMGetDataf(dr_Viz);

	view_heading = XPLMGetDataf(dr_ViewHdg);
	
	psi = XPLMGetDataf(dr_Psi);
	
	running_time_secs = XPLMGetDataf(dr_RunningTime);

	XPLMSetGraphicsState(0, 0, 0, 0, 0, 1, 1);
}

void XSADrawState() {
	char buf[512];
	
	strcpy(buf,"XSA-NG");
	
	switch(draw_flags) {
		case xsaNavTypeAirport:
			strcat(buf," Airports");
			break;
			
		case xsaNavTypeVOR:
			strcat(buf," Nav Aids");
			break;

		case xsaNavTypeFix:
			strcat(buf," Fixes");
			break;
			
		case xsaNavTypeUSGSCivil:
			strcat(buf," Civil USGS");
			break;			

		case xsaNavTypeUSGSMunicipal:
			strcat(buf," Municipal USGS");
			break;			

		case xsaNavTypeUSGSTerrain:
			strcat(buf," Terrain USGS");
			break;			
	}
	
	if(detail_draw_flags != 0 && !(draw_flags == xsaNavTypeFix)) {
		strcat(buf," (w/ Details)");
	}
	
	int length = glutStrokeLength(GLUT_STROKE_MONO_ROMAN,(unsigned char*)&buf);

	glColor3f(0.0,1.0,0.0); // green
	
	glPushMatrix();
		glTranslated(plane_ogl.x,plane_ogl.y,plane_ogl.z);
		glRotatef(-view_heading,0.0,1.0,0.0);
		glTranslated(0.0,0.1,-10.0);
		glScaled(0.001,0.001,0.001);
		glTranslated(-(length/2),0.0,0.0);
		glutStrokeString(GLUT_STROKE_MONO_ROMAN,(unsigned char*)buf);
	glPopMatrix();
}

void XSADrawNav() {

	double max_viz = (viz+XSA_NAUTICAL_MILE_METERS);
	double max_viz_nm = max_viz/XSA_NAUTICAL_MILE_METERS;
	d_XSA3DPoint gl_point;
	navinfo_t stack_nav;

	list_count = 0;
	
	node_navinfo* list = XSAGetNavDBList();

	if(debug_dump) {
		XPLMDebugString("XSADrawNav() : fetched list\n");
		if(list) {
			XPLMDebugString("XSADrawNav() : list is NOT null\n");
		}
	}
	while(list->node != NULL) {
		list_count++;

		navinfo_t* ptr_nav = list->node;
		if(debug_dump) {
			char s[1024];
			sprintf(s,"XSADrawNav() : processing nav name %s type %i\n",ptr_nav->name,ptr_nav->xsaType);
			XPLMDebugString(s);
		}
		list = list->next;

		// reload lat/lon for moving targets ...
		if(ptr_nav->xsaType == xsaNavTypeShip) {
			XSALoadNavRef(ptr_nav->ref,ptr_nav);
		}

		d_XSAWorldPoint nav_point;
		nav_point.lat = ptr_nav->lat;
		nav_point.lon = ptr_nav->lon;
		double range = XSADistance(nav_point, plane_coord,'N');

		if(range > max_viz_nm) {
			if(debug_dump) {
				char s[1024];
				sprintf(s,"XSADrawNav() : <> range for nav name %s type %i\n",ptr_nav->name,ptr_nav->xsaType);
				XPLMDebugString(s);
			}
			continue;
		}

		// clean on stack nav struct, in case ...
		memset(&stack_nav,0,sizeof(navinfo_t));
		
		// glColor3f(0.0, 0.0, 1.0); // blue
		// glColor3f(0.0,1.0,0.0); // green		
		switch(ptr_nav->xsaType) {
			case xsaNavTypeUSGSCivil :
				if(!((draw_flags & xsaNavTypeUSGSCivil) == xsaNavTypeUSGSCivil)) {
					continue;
				}
				glColor3f(0.0,1.0,0.0); // green
				break;
				
			case xsaNavTypeUSGSMunicipal :
				if(!((draw_flags & xsaNavTypeUSGSMunicipal) == xsaNavTypeUSGSMunicipal)) {
					continue;
				}
				glColor3f(0.0,1.0,0.0); // green
				break;

			case xsaNavTypeUSGSTerrain :
				if(!((draw_flags & xsaNavTypeUSGSTerrain) == xsaNavTypeUSGSTerrain)) {
					continue;
				}
				glColor3f(0.0,1.0,0.0); // green
				break;

			case xsaNavTypeAirport :
				if(!((draw_flags & xsaNavTypeAirport) == xsaNavTypeAirport)) {
					continue;
				}
				glColor3f(1.0, 0.0, 0.0); // red
				break;
				
			case xsaNavTypeVOR :
				if(!((draw_flags & xsaNavTypeVOR) == xsaNavTypeVOR)) {
					continue;
				}
				glColor3f(1.0, 0.0, 1.0); // magenta
				break;
				
			case xsaNavTypeShip :
				if(!((draw_flags & xsaNavTypeVOR) == xsaNavTypeVOR)) {
					continue;
				}
				glColor3f(1.0, 0.0, 1.0); // magenta
				break;
				
			case xsaNavTypeNDB :
				if(!((draw_flags & xsaNavTypeVOR) == xsaNavTypeVOR)) {
					continue;
				}
				glColor3f(1.0, 0.0, 1.0); // magenta
				break;
				
			case xsaNavTypeFix :
				if(!((draw_flags & xsaNavTypeFix) == xsaNavTypeFix)) {
					continue;
				}
				if(ptr_nav->height < plane_coord.height) {
					memcpy(&stack_nav,ptr_nav,sizeof(navinfo_t));
					stack_nav.height = plane_coord.height;
					ptr_nav = &stack_nav;
				}
				glColor3f(0.0, 0.0, 1.0); // blue
				break;
				
			default :
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

		XSARenderShapeDiamond(gl_point.x,gl_point.y+16.0,gl_point.z);

		if(ptr_nav->xsaType != xsaNavTypeFix && detail_draw_flags != 0) {
			// nav name,nav id,frequency,distance etc ...
			static char* buf = (char*) malloc(1024*sizeof(unsigned char));
			memset(buf,0,1024*sizeof(unsigned char));
			// scratch
			static char* bufbuf = (char*) malloc(5*sizeof(char));
      
      if((detail_draw_flags & DETAILS_DRAW_NAME) == DETAILS_DRAW_NAME) {
          strcat(buf,ptr_nav->name);
      }
      
      if((detail_draw_flags & DETAILS_DRAW_ID) == DETAILS_DRAW_ID) {
          if(strlen(buf) > 0) { strcat(buf," "); }
          strcat(buf,ptr_nav->id);
      }
      
      if((detail_draw_flags & DETAILS_DRAW_DISTANCE) == DETAILS_DRAW_DISTANCE) {
          if(strlen(buf) > 0) { strcat(buf," "); }          
          strcat(buf,"(");
          memset(bufbuf,0,5*sizeof(char));
          sprintf(bufbuf,"%.1f",range);
          strcat(buf,bufbuf);
          strcat(buf,")");
      }

			double nav_scale_factor = range*0.15;
			if(nav_scale_factor < 0.08) { nav_scale_factor = 0.08; }
			gl_point.y = gl_point.y+32.0;
			XSADrawString(gl_point,nav_scale_factor,(unsigned char*)buf);			
		}
	}
}

void XSADrawString(d_XSA3DPoint translate,double scale,const unsigned char* string) {
	
	int length = glutStrokeLength(GLUT_STROKE_MONO_ROMAN,string);
		
	glPushMatrix();
	
		glTranslated(translate.x,translate.y,translate.z);
		glScaled(scale,scale,scale);
		glRotatef(-view_heading,0.0,1.0,0.0);
		glTranslated(-(length/2),0.0,0.0);
		
		glutStrokeString(GLUT_STROKE_MONO_ROMAN,string);

	glPopMatrix();
}

PLUGIN_API int XPluginStart(char* outName,char* outSig,char* outDesc) {
	strcpy(outName, "xsa-ng");
	strcpy(outSig, "com.dsrts.xsa-ng");
	strcpy(outDesc, "XSA-NG v1.1 (tpelton@gmail.com) built : "__DATE__);
	
	dr_X = XPLMFindDataRef("sim/flightmodel/position/local_x");
	dr_Y = XPLMFindDataRef("sim/flightmodel/position/local_y");
	dr_Z = XPLMFindDataRef("sim/flightmodel/position/local_z");

	dr_Lat = XPLMFindDataRef("sim/flightmodel/position/latitude");
	dr_Lon = XPLMFindDataRef("sim/flightmodel/position/longitude");
	dr_Elev = XPLMFindDataRef("sim/flightmodel/position/elevation");
	
	dr_Viz = XPLMFindDataRef("sim/graphics/view/visibility_effective_m");
	
	dr_ViewHdg = XPLMFindDataRef("sim/graphics/view/view_heading");
	dr_Psi = XPLMFindDataRef("sim/flightmodel/position/psi");
	
	dr_RunningTime = XPLMFindDataRef("sim/time/total_running_time_sec");
	
//	details_hot_key_id = XPLMRegisterHotKey(XPLM_VK_F12,xplm_DownFlag,"Toggle Details",key,&details_hot_key_id);
//	toggle_hot_key_id = XPLMRegisterHotKey(XPLM_VK_F11,xplm_DownFlag,"Toggle Type",key,&toggle_hot_key_id);
	XPLMRegisterDrawCallback(XSADraw,xplm_Phase_Objects,0,NULL);
	XSARenderInit();
	
	menu_root_idx = XPLMAppendMenuItem(XPLMFindPluginsMenu(),"XSA-NG",NULL,1);
	menu_root = XPLMCreateMenu("XSA-NG",XPLMFindPluginsMenu(),menu_root_idx,XSAMenuHandler,NULL);

  menu_drawname_idx = XPLMAppendMenuItem(menu_root,"Details : Draw Name",&menu_drawname_idx,1);
  XPLMCheckMenuItem(menu_root,menu_drawname_idx,xplm_Menu_Unchecked);
  
  menu_drawdistance_idx = XPLMAppendMenuItem(menu_root,"Details : Draw Distance",&menu_drawdistance_idx,1);
  XPLMCheckMenuItem(menu_root,menu_drawdistance_idx,xplm_Menu_Unchecked);
  
  menu_drawid_idx = XPLMAppendMenuItem(menu_root,"Details : Draw ID",&menu_drawid_idx,1);
	XPLMCheckMenuItem(menu_root,menu_drawid_idx,xplm_Menu_Unchecked);

	XPLMAppendMenuSeparator(menu_root);	

	menu_airport_idx = XPLMAppendMenuItem(menu_root,"Draw Airport",&menu_airport_idx,1);
	XPLMCheckMenuItem(menu_root,menu_airport_idx,xplm_Menu_Unchecked);
	menu_navaid_idx = XPLMAppendMenuItem(menu_root,"Draw NAVAID",&menu_navaid_idx,1);
	XPLMCheckMenuItem(menu_root,menu_navaid_idx,xplm_Menu_Unchecked);
	menu_fix_idx = XPLMAppendMenuItem(menu_root,"Draw Fix",&menu_fix_idx,1);
	XPLMCheckMenuItem(menu_root,menu_fix_idx,xplm_Menu_Unchecked);
	menu_civil_idx = XPLMAppendMenuItem(menu_root,"Draw Civil",&menu_civil_idx,1);
	XPLMCheckMenuItem(menu_root,menu_civil_idx,xplm_Menu_Unchecked);
	menu_terrain_idx = XPLMAppendMenuItem(menu_root,"Draw Terrain",&menu_terrain_idx,1);
	XPLMCheckMenuItem(menu_root,menu_terrain_idx,xplm_Menu_Unchecked);
	menu_municipal_idx = XPLMAppendMenuItem(menu_root,"Draw Municipal",&menu_municipal_idx,1);
	XPLMCheckMenuItem(menu_root,menu_municipal_idx,xplm_Menu_Unchecked);

	return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho,long inMessage,void* inParam) {
	if (inFromWho == XPLM_PLUGIN_XPLANE) {
		switch(inMessage) {
			case XPLM_MSG_PLANE_LOADED:
			if ((int)inParam == XPLM_PLUGIN_XPLANE) {
				// XPLMDebugString("plane loaded.\n");
				break;
			}
		}
	}
}

PLUGIN_API void	XPluginStop(void) {}
PLUGIN_API int XPluginEnable(void) { return 1; }
PLUGIN_API void XPluginDisable(void) {}

void menuHandlerMasks(int menu_idx,int* pflags,int MASK) {
    int flags = *pflags;
		if((flags & MASK) == MASK) {
			XPLMCheckMenuItem(menu_root,menu_idx,xplm_Menu_Unchecked);
		} else {
			XPLMCheckMenuItem(menu_root,menu_idx,xplm_Menu_Checked);
    }
    *pflags = flags ^ MASK;
}

void XSAMenuHandler(void* inMenuRef,void* inItemRef) {

	last_toggle = running_time_secs;
	
  if(inItemRef == &menu_drawname_idx) {
      menuHandlerMasks(menu_drawname_idx,&detail_draw_flags,DETAILS_DRAW_NAME);
	}
  
  if(inItemRef == &menu_drawdistance_idx) {
      menuHandlerMasks(menu_drawdistance_idx,&detail_draw_flags,DETAILS_DRAW_DISTANCE);
	}
  
  if(inItemRef == &menu_drawid_idx) {
      menuHandlerMasks(menu_drawid_idx,&detail_draw_flags,DETAILS_DRAW_ID);
	}
  
  if(inItemRef == &menu_airport_idx) {
      menuHandlerMasks(menu_airport_idx,&draw_flags,xsaNavTypeAirport);
	}
	
  if(inItemRef == &menu_navaid_idx) {
      menuHandlerMasks(menu_navaid_idx,&draw_flags,xsaNavTypeVOR);      
	}
	
  if(inItemRef == &menu_fix_idx) {
      menuHandlerMasks(menu_fix_idx,&draw_flags,xsaNavTypeFix);      
	}
	
  if(inItemRef == &menu_municipal_idx) {
      menuHandlerMasks(menu_municipal_idx,&draw_flags,xsaNavTypeUSGSMunicipal);      
	}
	
  if(inItemRef == &menu_civil_idx) {
      menuHandlerMasks(menu_civil_idx,&draw_flags,xsaNavTypeUSGSCivil);      
	}

  if(inItemRef == &menu_terrain_idx) {
      menuHandlerMasks(menu_terrain_idx,&draw_flags,xsaNavTypeUSGSTerrain);      
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

void toggleType() {
	last_toggle = running_time_secs;
	
	switch(draw_flags) {
		case 0:
        draw_flags = 0|xsaNavTypeAirport;
			break;
			
		case xsaNavTypeAirport:
			draw_flags = 0|xsaNavTypeVOR;
			break;

		case xsaNavTypeVOR:
			draw_flags = 0|xsaNavTypeFix;
			break;
			
		case xsaNavTypeFix:
			draw_flags = 0|xsaNavTypeUSGSCivil;
			break;
			
		case xsaNavTypeUSGSCivil:
			draw_flags = 0|xsaNavTypeUSGSMunicipal;
			break;
			
		case xsaNavTypeUSGSMunicipal:
			draw_flags = 0|xsaNavTypeUSGSTerrain;
			break;
			
		default:
			draw_flags = 0;
	}	
}

void dump() {
	char buf[512];
	sprintf(buf,"draw count : %d\n",list_count);
	XPLMDebugString(buf);
	// Fortify_ListAllMemory();
}
