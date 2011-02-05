/* 
 * File:   freeglut.h
 * Author: tpelton
 *
 * Created on August 18, 2008, 10:40 PM
 */

#ifndef _FREEGLUT_H
#define	_FREEGLUT_H

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#   define  GLUT_STROKE_ROMAN               ((void *)0x0000)
#   define  GLUT_STROKE_MONO_ROMAN          ((void *)0x0001)

#ifdef __WIN32__
#   define FGAPIENTRY __stdcall
#else
#define FGAPIENTRY
#endif

#ifdef __WIN32__
#   define FGAPI __declspec(dllexport)
#else
#define FGAPI
#endif

typedef struct tagSFG_StrokeVertex SFG_StrokeVertex;
struct tagSFG_StrokeVertex
{
    GLfloat         X, Y;
};

typedef struct tagSFG_StrokeStrip SFG_StrokeStrip;
struct tagSFG_StrokeStrip
{
    int             Number;
    const SFG_StrokeVertex* Vertices;
};

typedef struct tagSFG_StrokeChar SFG_StrokeChar;
struct tagSFG_StrokeChar
{
    GLfloat         Right;
    int             Number;
    const SFG_StrokeStrip* Strips;
};

typedef struct tagSFG_StrokeFont SFG_StrokeFont;
struct tagSFG_StrokeFont
{
    char*           Name;                       /* The source font name      */
    int             Quantity;                   /* Number of chars in font   */
    GLfloat         Height;                     /* Height of the characters  */
    const SFG_StrokeChar** Characters;          /* The characters mapping    */
};


FGAPI FGAPIENTRY void glutStrokeCharacter( void* font, int character );

FGAPI FGAPIENTRY int glutStrokeWidth( void* font, int character );

FGAPI FGAPIENTRY int glutStrokeLength( void* font, const unsigned char* string );

FGAPI FGAPIENTRY void glutStrokeString( void* fontID, const unsigned char *string );

#endif	/* _FREEGLUT_H */
