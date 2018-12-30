/** \file
 * \brief Draw Functions
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

#include "iup.h"

#include "iup_attrib.h"
#include "iup_class.h"
#include "iup_str.h"
#include "iup_object.h"
#include "iup_image.h"
#include "iup_drvdraw.h"

#include "iuphaiku_drv.h"


struct _IdrawCanvas{
  Ihandle* ih;
  int w, h;

  /*
  GdkDrawable* wnd;
  GdkPixmap* pixmap;
  GdkGC *gc, *pixmap_gc;
  */
};

IdrawCanvas* iupdrvDrawCreateCanvas(Ihandle* ih)
{
  IdrawCanvas* dc = calloc(1, sizeof(IdrawCanvas));

  dc->ih = ih;

UNIMPLEMENTED

  return dc;
}

void iupdrvDrawKillCanvas(IdrawCanvas* dc)
{
UNIMPLEMENTED

  free(dc);
}

void iupdrvDrawUpdateSize(IdrawCanvas* dc)
{
UNIMPLEMENTED
}

void iupdrvDrawFlush(IdrawCanvas* dc)
{
UNIMPLEMENTED
}

void iupdrvDrawGetSize(IdrawCanvas* dc, int *w, int *h)
{
  if (w) *w = dc->w;
  if (h) *h = dc->h;
}

void iupdrvDrawParentBackground(IdrawCanvas* dc)
{
  unsigned char r=0, g=0, b=0;
  char* color = iupBaseNativeParentGetBgColorAttrib(dc->ih);
  iupStrToRGB(color, &r, &g, &b);
  iupdrvDrawRectangle(dc, 0, 0, dc->w-1, dc->h-1, r, g, b, IUP_DRAW_FILL);
}

void iupdrvDrawRectangle(IdrawCanvas* dc, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, int style)
{
	UNIMPLEMENTED
}

void iupdrvDrawLine(IdrawCanvas* dc, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, int style)
{
	UNIMPLEMENTED
}

void iupdrvDrawArc(IdrawCanvas* dc, int x1, int y1, int x2, int y2, double a1, double a2, unsigned char r, unsigned char g, unsigned char b, int style)
{
	UNIMPLEMENTED
}

void iupdrvDrawPolygon(IdrawCanvas* dc, int* points, int count, unsigned char r, unsigned char g, unsigned char b, int style)
{
	UNIMPLEMENTED
}

void iupdrvDrawSetClipRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
	UNIMPLEMENTED
}

void iupdrvDrawResetClip(IdrawCanvas* dc)
{
	UNIMPLEMENTED
}

void iupdrvDrawText(IdrawCanvas* dc, const char* text, int len, int x, int y, unsigned char r, unsigned char g, unsigned char b, const char* font)
{
	UNIMPLEMENTED
}

void iupdrvDrawImage(IdrawCanvas* dc, const char* name, int make_inactive, int x, int y)
{
	UNIMPLEMENTED
}

void iupdrvDrawSelectRect(IdrawCanvas* dc, int x, int y, int w, int h)
{
	UNIMPLEMENTED
}

void iupdrvDrawFocusRect(IdrawCanvas* dc, int x, int y, int w, int h)
{
	UNIMPLEMENTED
}
