#ifndef _XSA_NG_TYPES_H
#define	_XSA_NG_TYPES_H

#include <GL/gl.h>
#include <XPLMNavigation.h>

enum _xsaNavType {
	xsaNavTypeUnknown = 1, // 2^^0
	xsaNavTypeAirport = 2, // 2^^1
	xsaNavTypeVOR = 4, // 2^^2
	xsaNavTypeNDB = 8, // ...
	xsaNavTypeFix = 16,
	xsaNavTypeHelipad = 32,
	xsaNavTypeSeaport = 64,
	xsaNavTypeShip = 128,
	xsaNavTypeUSGSTerrain = 256,
	xsaNavTypeUSGSCivil = 512,
	xsaNavTypeUSGSMunicipal = 1024
};

typedef _xsaNavType XSANavType;

typedef struct {
	int dbid;
	XPLMNavRef ref;
	XSANavType xsaType;
	XPLMNavType xplmType;
	float lat;
	float lon;
	float height;
	int frequency;
	char id[32];
	char name[256];
} navinfo_t;

struct _node_navinfo;
typedef _node_navinfo node_navinfo;
struct _node_navinfo {
	navinfo_t* node;
	node_navinfo* next;
};

typedef struct {
  double x;
  double y;
  double z;
} d_XSA3DPoint;

typedef struct {
  double lat;
  double lon;
  double height;
} d_XSAWorldPoint;

#endif	/* _XSA_NG_TYPES_H */
