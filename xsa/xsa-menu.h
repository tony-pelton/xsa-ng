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

#define XSA_PREFS_COOKIE 1

struct XSAMenu_t {
	int cookie;
	int draw_flags;
	int detail_draw_flags;
};

void XSAMenuInit();
void XSAMenuSave();

#endif	/* _XSA_MENU_H */

