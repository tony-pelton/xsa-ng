/* 
 * File:   xsa-ng-render.h
 * Author: tpelton
 *
 * Created on August 15, 2008, 3:58 PM
 */

#ifndef _XSA_NG_RENDER_H
#define	_XSA_NG_RENDER_H

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "xsa-ng-types.h"

void XSARenderInit();
void XSARenderShape(GLuint,const d_XSA3DPoint*);
void XSARenderShapeDiamond(const d_XSA3DPoint*);
void XSADrawString(d_XSA3DPoint,float,double,const unsigned char*);
void XSADrawStringHUD(d_XSA3DPoint,float,const unsigned char*);

#endif	/* _XSA_NG_RENDER_H */

