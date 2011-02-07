#include <stdlib.h>

#include <XPLMMenus.h>

#include "xsa-menu.h"
#include "xsa-ng-types.h"

int draw_flags = 0;
int detail_draw_flags = 0;

static XPLMMenuID menu_root = NULL;
static int menu_root_idx = 0;
static int menu_drawname_idx = 0;
static int menu_drawdistance_idx = 0;
static int menu_drawid_idx = 0;
static int menu_fix_idx = 0;
static int menu_airport_idx = 0;
static int menu_navaid_idx = 0;
static int menu_civil_idx = 0;
static int menu_terrain_idx = 0;
static int menu_municipal_idx = 0;

static void menuItemSyncState();
static void menuItemSyncToggle(int,int*,int MASK);
static void XSAMenuHandler(void*,void*);

void menuItemSyncState() {
    menuItemSyncToggle(menu_drawname_idx, &detail_draw_flags, DETAILS_DRAW_NAME);
    menuItemSyncToggle(menu_drawdistance_idx, &detail_draw_flags, DETAILS_DRAW_DISTANCE);
    menuItemSyncToggle(menu_drawid_idx, &detail_draw_flags, DETAILS_DRAW_ID);
    menuItemSyncToggle(menu_airport_idx, &draw_flags, xsaNavTypeAirport);
    menuItemSyncToggle(menu_navaid_idx, &draw_flags, xsaNavTypeVOR);
    menuItemSyncToggle(menu_fix_idx, &draw_flags, xsaNavTypeFix);
    menuItemSyncToggle(menu_municipal_idx, &draw_flags, xsaNavTypeUSGSMunicipal);
    menuItemSyncToggle(menu_civil_idx, &draw_flags, xsaNavTypeUSGSCivil);
    menuItemSyncToggle(menu_terrain_idx, &draw_flags, xsaNavTypeUSGSTerrain);
}

/*
 * set menu item check state to match the flag variable and mask
 */
void menuItemSyncToggle(int menu_idx, int* pflags, int MASK) {
    int flags = *pflags;
    if ((flags & MASK) == MASK) {
        XPLMCheckMenuItem(menu_root, menu_idx, xplm_Menu_Checked);
    } else {
        XPLMCheckMenuItem(menu_root, menu_idx, xplm_Menu_Unchecked);
    }
}

void XSAMenuInit() {
    menu_root_idx = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "XSA-NG", NULL, 1);
    menu_root = XPLMCreateMenu("XSA-NG", XPLMFindPluginsMenu(), menu_root_idx, XSAMenuHandler, NULL);

    menu_drawname_idx = XPLMAppendMenuItem(menu_root, "Details : Draw Name", &menu_drawname_idx, 1);
    XPLMCheckMenuItem(menu_root, menu_drawname_idx, xplm_Menu_Unchecked);

    menu_drawdistance_idx = XPLMAppendMenuItem(menu_root, "Details : Draw Distance", &menu_drawdistance_idx, 1);
    XPLMCheckMenuItem(menu_root, menu_drawdistance_idx, xplm_Menu_Unchecked);

    menu_drawid_idx = XPLMAppendMenuItem(menu_root, "Details : Draw ID", &menu_drawid_idx, 1);
    XPLMCheckMenuItem(menu_root, menu_drawid_idx, xplm_Menu_Unchecked);

    XPLMAppendMenuSeparator(menu_root);

    menu_airport_idx = XPLMAppendMenuItem(menu_root, "Draw Airport", &menu_airport_idx, 1);
    menu_navaid_idx = XPLMAppendMenuItem(menu_root, "Draw NAVAID", &menu_navaid_idx, 1);
    menu_fix_idx = XPLMAppendMenuItem(menu_root, "Draw Fix", &menu_fix_idx, 1);
    menu_civil_idx = XPLMAppendMenuItem(menu_root, "Draw Civil", &menu_civil_idx, 1);
    menu_terrain_idx = XPLMAppendMenuItem(menu_root, "Draw Terrain", &menu_terrain_idx, 1);
    menu_municipal_idx = XPLMAppendMenuItem(menu_root, "Draw Municipal", &menu_municipal_idx, 1);
    
    menuItemSyncState();
}

void XSAMenuHandler(void* inMenuRef, void* inItemRef) {
    if (inItemRef == &menu_drawname_idx) {
        detail_draw_flags = detail_draw_flags ^ DETAILS_DRAW_NAME;
    }
    if (inItemRef == &menu_drawdistance_idx) {
        detail_draw_flags = detail_draw_flags ^ DETAILS_DRAW_DISTANCE;
    }
    if (inItemRef == &menu_drawid_idx) {
        detail_draw_flags = detail_draw_flags ^ DETAILS_DRAW_ID;
    }
    if (inItemRef == &menu_airport_idx) {
        draw_flags = draw_flags ^ xsaNavTypeAirport;
		draw_flags = draw_flags ^ xsaNavTypeHelipad;
		draw_flags = draw_flags ^ xsaNavTypeSeaport;
    }
    if (inItemRef == &menu_navaid_idx) {
        draw_flags = draw_flags ^ xsaNavTypeVOR;
		draw_flags = draw_flags ^ xsaNavTypeNDB;
		draw_flags = draw_flags ^ xsaNavTypeShip;
    }
    if (inItemRef == &menu_fix_idx) {
        draw_flags = draw_flags ^ xsaNavTypeFix;
    }
    if (inItemRef == &menu_municipal_idx) {
        draw_flags = draw_flags ^ xsaNavTypeUSGSMunicipal;
    }
    if (inItemRef == &menu_civil_idx) {
        draw_flags = draw_flags ^ xsaNavTypeUSGSCivil;
    }
    if (inItemRef == &menu_terrain_idx) {
        draw_flags = draw_flags ^ xsaNavTypeUSGSTerrain;
    }
    menuItemSyncState();
}

