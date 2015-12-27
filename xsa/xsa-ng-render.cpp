#include <stdio.h>
#include "freeglut.h"
#include "xsa-ng-render.h"

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

void XSARenderShapeDiamond(const d_XSA3DPoint* p_p) {
	XSARenderShape(SHAPE_LIST_DIAMOND,p_p);
}

void XSARenderShape(GLuint list,const d_XSA3DPoint* p_p) {
	glPushMatrix();
		glColor3f(p_p->r, p_p->g, p_p->b);
		glTranslated(p_p->x,p_p->y,p_p->z);
		glRotatef(0.0,0.0,1.0,0.0);
    	glScaled(p_p->scale,p_p->scale,p_p->scale);
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

void XSADrawString(d_XSA3DPoint translate,float rotation,double scale,const unsigned char* string) {
    int length = glutStrokeLength(GLUT_STROKE_MONO_ROMAN, string);
    glPushMatrix();
        glTranslated(translate.x, translate.y, translate.z);
        glScaled(scale, scale, scale);
        glRotatef(-rotation, 0.0, 1.0, 0.0);
        glTranslated(-(length / 2), 0.0, 0.0);
        glutStrokeString(GLUT_STROKE_MONO_ROMAN, string);
    glPopMatrix();
}

void XSADrawStringHUD(d_XSA3DPoint point,float rotation,const unsigned char* buf) {
    int length = glutStrokeLength(GLUT_STROKE_MONO_ROMAN,  buf);
    glColor3f(0.0, 1.0, 0.0); // green
    glPushMatrix();
        glTranslated(point.x, point.y, point.z);
        glRotatef(-rotation, 0.0, 1.0, 0.0);
        glTranslated(0.0, 0.1, -10.0);
        glScaled(0.001, 0.001, 0.001);
        glTranslated(-(length / 2), 0.0, 0.0);
        glutStrokeString(GLUT_STROKE_MONO_ROMAN, buf);
    glPopMatrix();
}
