#ifndef _XSA_NG_TYPES_H
#define	_XSA_NG_TYPES_H

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <XPLMNavigation.h>

#define XSA_NAUTICAL_MILE_METERS 1852

typedef enum XSANavType_t {
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
} XSANavType;

typedef struct NAVInfo_t NAVInfo;

struct NAVInfo_t {
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
};

typedef struct NodeNAVInfo_t NodeNAVInfo;

struct NodeNAVInfo_t {
	NAVInfo* node;
	NodeNAVInfo* next;
};

typedef struct d_XSA3DPoint_t d_XSA3DPoint;

struct d_XSA3DPoint_t {
  double x;
  double y;
  double z;
  double scale;
  double r;
  double g;
  double b;
};

typedef struct d_XSAWorldPoint_t d_XSAWorldPoint;

struct d_XSAWorldPoint_t {
  double lat;
  double lon;
  double height;
};

#endif	/* _XSA_NG_TYPES_H */
