/* 
 * File:   xsa-menu.h
 * Author: tpelton
 *
 * Created on October 3, 2010, 10:36 AM
 */

#ifndef _XSA_MENU_H
#define	_XSA_MENU_H

#include <ctype.h>

#define DETAILS_DRAW_NAME 1
#define DETAILS_DRAW_ID 2
#define DETAILS_DRAW_DISTANCE 4

extern int draw_flags;
extern int detail_draw_flags;

void XSAMenuInit();

#endif	/* _XSA_MENU_H */

