/* 
 * File:   xsa-nav.h
 * Author: tpelton
 *
 * Created on August 16, 2008, 9:07 AM
 */

#ifndef _XSA_NAV_H
#define	_XSA_NAV_H

#include <stdio.h>

#include "xsa-ng-types.h"

void XSALoadNavRef(XPLMNavRef ref,NAVInfo* nav);
void XSATouchNavDB(const d_XSAWorldPoint point);
NodeNAVInfo* XSAGetNavDBList();
double XSADistance(const d_XSAWorldPoint point1, const d_XSAWorldPoint point2, char unit);
void XSAPointOnBearingFromPoint(d_XSAWorldPoint* out,const d_XSAWorldPoint source,double bearing,double d);

/* test */
// void XSAAppendNavDBNode(navinfo_t* node);
// node_navinfo* XSAGetCurrentNavDBList();
// node_navinfo* XSAGetWorkingNavDBList();
// void XSAPromoteWorkingNavDBList();
// void XSAClearNavDBList(node_navinfo* list);

#endif	/* _XSA_NAV_H */

