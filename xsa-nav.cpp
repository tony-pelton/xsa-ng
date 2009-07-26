
#include <string.h>

#include "fortify.h"

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

#include <XPLMNavigation.h>
#include <XPLMUtilities.h>

#include "xsa-ng-types.h"
#include "xsa-nav.h"

double deg2rad(double);
double rad2deg(double);

d_XSAWorldPoint db_center;

node_navinfo* navlist = NULL;

void XSALoadNavRef(XPLMNavRef ref,navinfo_t* nav) {

	XPLMGetNavAidInfo(
		ref,
		&nav->xplmType,
		&nav->lat,
		&nav->lon,
		&nav->height,
		&nav->frequency,
		NULL,
		nav->id,
		nav->name,
		NULL);
	
	nav->xsaType = xsaNavTypeUnknown;

	switch(nav->xplmType) {
		case xplm_Nav_Airport :
			const static char heli[] = "[H]";
			if(strncmp(heli,nav->name,strlen(heli)) == 0) {
				nav->xsaType = xsaNavTypeHelipad;
				break;
			}
			const static char seap[] = "[S]";
			if(strncmp(heli,nav->name,strlen(seap)) == 0) {
				nav->xsaType = xsaNavTypeSeaport;
				break;
			}
			nav->xsaType = xsaNavTypeAirport;
			break;

		case xplm_Nav_VOR :
			const static char uss[] = "USS";
			if(strncmp(uss,nav->name,strlen(uss)) == 0) {
				nav->xsaType = xsaNavTypeShip;
				break;
			}
			nav->xsaType = xsaNavTypeVOR;
			break;

		case xplm_Nav_NDB :
			nav->xsaType = xsaNavTypeNDB;
			break;

		case xplm_Nav_Fix :
			nav->xsaType = xsaNavTypeFix;
			break;
			
		// case xplm_Nav_OuterMarker:
		// case xplm_Nav_Localizer:
		// case xplm_Nav_GlideSlope:
		// case xplm_Nav_DME:
		// case xplm_Nav_ILS:
		case xplm_Nav_MiddleMarker:
		case xplm_Nav_InnerMarker:
			nav->xsaType = xsaNavTypeNDB;
			break;
	}
}

void XSATouchNavDB(const d_XSAWorldPoint point) {

	static const char delim[] = "|";
	
	static FILE *f = NULL;
	static char file_dlm[1024];
	
	static bool init = true;	
	static bool nav_db_load = false;
	static bool usgs_db_load = false;
	
	static bool report = false;
	static int report_load_dbnav = 0;
	static int report_alloc_node = 0;
	static int report_alloc_link = 0;
	
	static XPLMNavRef navref_current = XPLM_NAV_NOT_FOUND;
	static node_navinfo* ptr_navlist = NULL;
	static navinfo_t navinfo;

	static int id = 0;

	if(init) {
		navlist = (node_navinfo*) malloc(sizeof(node_navinfo));
		memset(navlist,0,sizeof(node_navinfo));
		ptr_navlist = navlist;
		XPLMGetSystemPath(file_dlm);
		strcat(file_dlm,"xdata.dlm");
		f = fopen(file_dlm,"r");
		if(!f) {
			XPLMDebugString("xdata.dlm file not found.\n");
		}
		init = false;
		XPLMDebugString("init()\n");
	}
	
	if(XSADistance(point,db_center,'N') > 30.0) {
		XPLMDebugString("XSATouchNavDB : (re)set center\n");
		report = true;
		report_load_dbnav = 0;

		if(nav_db_load) {
			nav_db_load = false;
			navref_current = XPLM_NAV_NOT_FOUND;
		}
		if(usgs_db_load) {
			usgs_db_load = false;
		}
		if(!nav_db_load && !usgs_db_load) {
			report = true;
			id = 0;
			ptr_navlist = navlist;
			nav_db_load = true;

			if(f) {
				fseek(f,0,SEEK_SET);
				usgs_db_load = true;
			}

			db_center.lat = point.lat;
			db_center.lon = point.lon;
			XPLMDebugString("db_load start\n");
		}
	}

	int loop = 0;
	while( (nav_db_load || usgs_db_load) && loop < 100) {

		loop++;

//		XPLMDebugString("ptr_navlist->node\n");
		if(!ptr_navlist->node) {
			ptr_navlist->node = (navinfo_t*) malloc(sizeof(navinfo_t));
			memset(ptr_navlist->node,0,sizeof(navinfo_t));
			report_alloc_node++;
		}
//		XPLMDebugString("ptr_navlist->next\n");
		if(!ptr_navlist->next) {
			ptr_navlist->next = (node_navinfo*) malloc(sizeof(node_navinfo));
			memset(ptr_navlist->next,0,sizeof(node_navinfo));
			report_alloc_link++;
			ptr_navlist->next->node = NULL;
		}
		
		memset(&navinfo,0,sizeof(navinfo_t));		
		id++;		
		navinfo.dbid = id;
			
		if(nav_db_load) {
//			XPLMDebugString("nav_db_load\n");
			if(navref_current == XPLM_NAV_NOT_FOUND) {
				navref_current = XPLMGetFirstNavAid();
			} else {
				navref_current = XPLMGetNextNavAid(navref_current);				
			}
			
			if(navref_current == XPLM_NAV_NOT_FOUND) {
				nav_db_load = false;
				XPLMDebugString("nav_db_load done.\n");
				continue;
			}

			XSALoadNavRef(navref_current,&navinfo);
			navinfo.ref = navref_current;
			if(navinfo.xsaType == xsaNavTypeUnknown) {
				continue;
			}
			
			d_XSAWorldPoint nav_point;
			nav_point.lat = navinfo.lat;
			nav_point.lon = navinfo.lon;
			if(XSADistance(nav_point,db_center,'N') < 50.0) {
//				XPLMDebugString("found navdb point\n");
				memcpy(ptr_navlist->node,&navinfo,sizeof(navinfo_t));
				report_load_dbnav++;
				ptr_navlist = ptr_navlist->next;
				char s[1024];
				sprintf(s,"XSATouchNavDB() : loaded navdb name : %s\n",navinfo.name);
				XPLMDebugString(s);
			}

		} // nav_db_load

		if(!nav_db_load && usgs_db_load) {
			char s[1024];
			fgets(s,sizeof(s),f);
			if(feof(f)) {
				// close things up
				fseek(f,0,SEEK_SET);
				usgs_db_load = false;
				XPLMDebugString("nav_db_load done.\n");
				continue;
			}
			s[strlen(s)-1] = '\0';
			char* layer = strtok(s,delim);
			navinfo.lat = atof(strtok(NULL,delim));
			navinfo.lon = atof(strtok(NULL,delim));

			d_XSAWorldPoint nav_point;
			nav_point.lat = navinfo.lat;
			nav_point.lon = navinfo.lon;
			if(XSADistance(nav_point,db_center,'N') < 50.0) {				
				navinfo.height = atof(strtok(NULL,delim));
				navinfo.xsaType = xsaNavTypeUnknown;
				switch(atoi(strtok(NULL,delim))) {
					case 21:
						navinfo.xsaType = xsaNavTypeUSGSMunicipal;
						break;
					case 22:
						navinfo.xsaType = xsaNavTypeUSGSTerrain;
						break;
					case 23:
						navinfo.xsaType = xsaNavTypeUSGSCivil;
						break;
				}
				
				strcpy(navinfo.id,strtok(NULL,delim));
				
				strcpy(navinfo.name,strtok(NULL,delim));				

//				for(int idx = 0;idx < strlen(navinfo.name);idx++) {
//					if(navinfo.name[idx] < 0x20 || navinfo.name[idx] > 0x7E) {
//						memset(navinfo.name,0,sizeof(navinfo.name));
//						navinfo.name[0] = '\0';
//					}
//				}
				
				memcpy(ptr_navlist->node,&navinfo,sizeof(navinfo_t));
				report_load_dbnav++;
				ptr_navlist = ptr_navlist->next;

				char s[1024];
				sprintf(s,"XSATouchNavDB() : loaded usgs db name %s\n",navinfo.name);
				XPLMDebugString(s);				

			} // usgs in range
		} // usgs_db_load
	} // while
	
	if(!nav_db_load && !usgs_db_load && report) {
		// do report here
		char s[1024];
		sprintf(s,"loaded %i, nodes %i, links %i\n",report_load_dbnav,report_alloc_node,report_alloc_link);
		XPLMDebugString(s);
		report = false;
	}
}

node_navinfo* XSAGetNavDBList() {
	return navlist;
}

double XSADistance(const d_XSAWorldPoint point1,const d_XSAWorldPoint point2,char unit) {
	double lat1 = point1.lat;
	double lon1 = point1.lon;
	double lat2 = point2.lat;
	double lon2 = point2.lon;
	
	double theta, dist;
	theta = lon1 - lon2;
	dist = sin(deg2rad(lat1)) * sin(deg2rad(lat2)) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(deg2rad(theta));
	dist = acos(dist);
	dist = rad2deg(dist);
	dist = dist * 60 * 1.1515;
	switch(unit) {
	case 'M': // miles
		break;
	case 'K': // kilometers
		dist = dist * 1.609344;
		break;
	case 'N': // nautical miles
		dist = dist * 0.8684;
		break;
	}
	return (dist);
}

const double DEG2RADHALFPI = M_PI / 180;
double deg2rad(double deg) {
	return (deg * DEG2RADHALFPI);
}

const double RAD2DEGHALFPI = 180 / M_PI;
double rad2deg(double rad) {
	return (rad * RAD2DEGHALFPI);
}

/*
 * calculate destination point given start point, initial bearing (deg) and distance (km)
 *   see http://williams.best.vwh.net/avform.htm#LL
 */

// earth's mean radius in km
#define R 6371
void XSAPointOnBearingFromPoint(d_XSAWorldPoint* out,const d_XSAWorldPoint source,double bearing,double d) {
	double lat1 = deg2rad(source.lat);
	double lon1 = deg2rad(source.lon);
	double brng = deg2rad(bearing);
	
	double lat2 = asin(sin(lat1)*cos(d/R) + cos(lat1)*sin(d/R)*cos(brng));
	double lon2 = lon1 + atan2(sin(brng)*sin(d/R)*cos(lat1),cos(d/R)-sin(lat1)*sin(lat2));
	// lon2 = ((lon2+M_PI) % (2*M_PI)) - M_PI;  // normalise to -180...+180

	out->lat = rad2deg(lat2);
	out->lon = rad2deg(lon2);
}
