/* 
 * File:   xsa-ng-render.h
 * Author: tpelton
 *
 * Created on August 15, 2008, 3:58 PM
 */

#ifndef _XSA_NG_RENDER_H
#define	_XSA_NG_RENDER_H

#include <GL/gl.h>
#include "xsa-ng-types.h"

void XSARenderInit();
void XSARenderShape(GLuint,double,double,double);
void XSARenderShapeDiamond(double x, double y, double z);

#endif	/* _XSA_NG_RENDER_H */
