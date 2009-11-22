#include "fortify.h"

#include <stdio.h>

#include <GL/gl.h>

#include "xsa-ng-render.h"
#include "xsa-ng-types.h"

GLuint SHAPE_LIST_DIAMOND;

#define SMALL 10
#define LARGE 30

GLuint XSABuildOpenGLShapeList(d_XSA3DPoint**);
void XSADrawGLLines3D(d_XSA3DPoint**);
d_XSA3DPoint* XSAGeneratePoint3D(double,double,double);
d_XSA3DPoint** XSADiamond();

void XSARenderInit() {
	d_XSA3DPoint** points = XSADiamond();
	SHAPE_LIST_DIAMOND = XSABuildOpenGLShapeList(points);
	for(int idx = 0;idx < 16;idx++) {
		free(points[idx]);
	}
	free(points);
}

void XSARenderShapeDiamond(double x, double y, double z) {
	XSARenderShape(SHAPE_LIST_DIAMOND,x,y,z);
}

void XSARenderShape(GLuint list,double x, double y, double z) {
	glPushMatrix();
		glTranslated(x,y,z);
		glRotatef(0.0,0.0,1.0,0.0);
    	glScaled(1.0,1.0,1.0);
		glCallList(SHAPE_LIST_DIAMOND);
 	glPopMatrix();
}

GLuint XSABuildOpenGLShapeList(d_XSA3DPoint** points) {
	GLuint list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	XSADrawGLLines3D(points);
	glEndList();
	return list;
}

void XSADrawGLLines3D(d_XSA3DPoint** points) {
  glBegin(GL_LINES);
	for(int idx = 0;idx < 16;idx++) {
		glVertex3d(points[idx]->x,points[idx]->y,points[idx]->z);
	}
  glEnd();
}

d_XSA3DPoint* XSAGeneratePoint3D(double x, double y, double z) {
	d_XSA3DPoint* point = (d_XSA3DPoint*) malloc(sizeof(d_XSA3DPoint));
	point->x = x;
	point->y = y;
	point->z = z;
	return point;
}

d_XSA3DPoint** XSADiamond() {
	double w = LARGE/2;
	double h = LARGE/2;
	double x = 0, y = 0, z = 0;
	d_XSA3DPoint** points = (d_XSA3DPoint**) malloc(sizeof(d_XSA3DPoint[16]));

	points[0] = XSAGeneratePoint3D(x,y+h,z);
	points[1] = XSAGeneratePoint3D(x,y,z+w);
	points[2] = XSAGeneratePoint3D(x,y+h,z);
	points[3] = XSAGeneratePoint3D(x+w,y,z);
	points[4] = XSAGeneratePoint3D(x,y+h,z);
	points[5] = XSAGeneratePoint3D(x,y,z-w);
	points[6] = XSAGeneratePoint3D(x,y+h,z);
	points[7] = XSAGeneratePoint3D(x-w,y,z);
	points[8] = XSAGeneratePoint3D(x,y-h,z);
	points[9] = XSAGeneratePoint3D(x,y,z+w);
	points[10] = XSAGeneratePoint3D(x,y-h,z);
	points[11] = XSAGeneratePoint3D(x+w,y,z);
	points[12] = XSAGeneratePoint3D(x,y-h,z);
	points[13] = XSAGeneratePoint3D(x,y,z-w);
	points[14] = XSAGeneratePoint3D(x,y-h,z);
	points[15] = XSAGeneratePoint3D(x-w,y,z);
	
	return points;
}
