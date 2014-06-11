/** \file
 * \brief iupexpander control
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "iup.h"
#include "iupcbs.h"
#include "iupglcontrols.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_register.h"

#include "iup_glcontrols.h"


#define IEXPAND_BUTTON_SIZE 16
#define IEXPAND_HANDLE_SIZE 20
#define IEXPAND_SPACING   3
#define IEXPAND_BACK_MARGIN  2

enum { IEXPANDER_LEFT, IEXPANDER_RIGHT, IEXPANDER_TOP, IEXPANDER_BOTTOM };
enum { IEXPANDER_CLOSE, IEXPANDER_OPEN };

struct _IcontrolData
{
  /* attributes */
  int position;
  int state;
  int barSize;

  int extra_buttons,
      extra_buttons_state[4];
};


static void iGLExpanderOpenCloseChild(Ihandle* ih, int refresh, int callcb, int state)
{
  Ihandle* gl_parent = (Ihandle*)iupAttribGet(ih, "GL_CANVAS");
  Ihandle *child = ih->firstchild;

  if (callcb)
  {
    IFni cb = (IFni)IupGetCallback(ih, "OPENCLOSE_CB");
    if (cb)
    {
      int ret = cb(ih, state);
      if (ret == IUP_IGNORE)
        return;
    }
  }

  ih->data->state = state;

  if (child)
  {
    if (ih->data->state == IEXPANDER_CLOSE)
      IupSetAttribute(child, "VISIBLE", "NO");
    else
      IupSetAttribute(child, "VISIBLE", "YES");

    if (refresh)
      IupRefreshChildren(gl_parent); /* this will recompute the layout of the glcanvasbox only */
  }

  IupSetAttribute(gl_parent, "REDRAW", NULL);

  if (callcb)
  {
    IFn cb = IupGetCallback(ih, "ACTION");
    if (cb)
      cb(ih);
  }
}

static int iGLExpanderGetBarSize(Ihandle* ih)
{
  int bar_size;
  if (ih->data->barSize == -1)
  {
    iupGLFontGetCharSize(ih, NULL, &bar_size); 

    if (bar_size < IEXPAND_HANDLE_SIZE)
      bar_size = IEXPAND_HANDLE_SIZE;

    if (ih->data->position == IEXPANDER_TOP)
    {
      char* image = iupAttribGetStr(ih, "IMAGE");
      if (image)
      {
        int image_h = 0;
        iupGLImageGetInfo(image, NULL, &image_h, NULL);
        bar_size = iupMAX(bar_size, image_h);
      }

      if (iupAttribGetStr(ih, "TITLE") || image)
        bar_size += 2 * IEXPAND_BACK_MARGIN;
    }
  }
  else
    bar_size = ih->data->barSize;

  return bar_size;
}

#if 0
static int iGLExpanderMOTION_CB(Ihandle* bar, int x, int y, char* status)
{
  Ihandle* ih = bar->parent;

  if (ih->data->position != IEXPANDER_TOP)
    return IUP_DEFAULT;

  if (y >= IEXPAND_SPACING + IEXPAND_BACK_MARGIN && y <= bar_size - IEXPAND_SPACING - IEXPAND_BACK_MARGIN)
  {
    int old_state[4];
    old_state[1] = ih->data->extra_buttons_state[1];
    old_state[2] = ih->data->extra_buttons_state[2];
    old_state[3] = ih->data->extra_buttons_state[3];

    if ((x >= bar->currentwidth - (IEXPAND_BUTTON_SIZE + IEXPAND_SPACING) - IEXPAND_BACK_MARGIN) &&
      (x < bar->currentwidth - IEXPAND_SPACING - IEXPAND_BACK_MARGIN))
    {
      if (ih->data->extra_buttons_state[1] == 0)
        ih->data->extra_buttons_state[1] = -1;  /* highlight if not pressed */
    }
    else
    {
      if (ih->data->extra_buttons_state[1] != 0)
        ih->data->extra_buttons_state[1] = 0;
    }

    if (ih->data->extra_buttons > 1)
    {
      if ((x >= bar->currentwidth - 2 * (IEXPAND_BUTTON_SIZE + IEXPAND_SPACING) - IEXPAND_BACK_MARGIN) &&
        (x < bar->currentwidth - (IEXPAND_BUTTON_SIZE + 2 * IEXPAND_SPACING) - IEXPAND_BACK_MARGIN))
      {
        if (ih->data->extra_buttons_state[2] == 0)
          ih->data->extra_buttons_state[2] = -1;  /* highlight if not pressed */
      }
      else
      {
        if (ih->data->extra_buttons_state[2] != 0)
          ih->data->extra_buttons_state[2] = 0;
      }
    }

    if (ih->data->extra_buttons == 3)
    {
      if ((x >= bar->currentwidth - 3 * (IEXPAND_BUTTON_SIZE + IEXPAND_SPACING) - IEXPAND_BACK_MARGIN) &&
        (x < bar->currentwidth - (2 * IEXPAND_BUTTON_SIZE + 3 * IEXPAND_SPACING) - IEXPAND_BACK_MARGIN))
      {
        if (ih->data->extra_buttons_state[3] == 0)
          ih->data->extra_buttons_state[3] = -1;  /* highlight if not pressed */
      }
      else
      {
        if (ih->data->extra_buttons_state[3] != 0)
          ih->data->extra_buttons_state[3] = 0;
      }
    }

    if (old_state[1] != ih->data->extra_buttons_state[1] ||
      old_state[2] != ih->data->extra_buttons_state[2] ||
      old_state[3] != ih->data->extra_buttons_state[3])
      IupUpdate(bar);
  }

  (void)status;
  return IUP_DEFAULT;
}

static int iGLExpanderCallExtraButtonCb(Ihandle* ih, int button, int pressed)
{
  int old_state = ih->data->extra_buttons_state[button];
  ih->data->extra_buttons_state[button] = pressed;

  /* redraw only if state changed */
  if (old_state != ih->data->extra_buttons_state[button])
    IupUpdate(ih->firstchild);

  if (!pressed)
    pressed = pressed;

  /* if pressed always call,
  if not pressed, call only if was pressed */
  if (pressed || old_state == 1)
  {
    IFnii cb = (IFnii)IupGetCallback(ih, "EXTRABUTTON_CB");
    if (cb)
      cb(ih, button, pressed);
  }

  return IUP_DEFAULT;
}

static int iGLExpanderBUTTON_CB(Ihandle* bar, int button, int pressed, int x, int y, char* status)
{
  Ihandle* ih = bar->parent;

  if (button != IUP_BUTTON1)
    return IUP_DEFAULT;

  if (ih->data->position == IEXPANDER_TOP && ih->data->extra_buttons != 0)
  {
    if (y >= IEXPAND_SPACING + IEXPAND_BACK_MARGIN && y <= height - IEXPAND_SPACING - IEXPAND_BACK_MARGIN)
    {
      if ((x >= width - (IEXPAND_BUTTON_SIZE + IEXPAND_SPACING) - IEXPAND_BACK_MARGIN) &&
        (x < width - IEXPAND_SPACING - IEXPAND_BACK_MARGIN))
        return iGLExpanderCallExtraButtonCb(ih, 1, pressed);

      if (ih->data->extra_buttons > 1)
      {
        if ((x >= width - 2 * (IEXPAND_BUTTON_SIZE + IEXPAND_SPACING) - IEXPAND_BACK_MARGIN) &&
          (x < width - (IEXPAND_BUTTON_SIZE + 2 * IEXPAND_SPACING) - IEXPAND_BACK_MARGIN))
          return iGLExpanderCallExtraButtonCb(ih, 2, pressed);
      }

      if (ih->data->extra_buttons == 3)
      {
        if ((x >= width - 3 * (IEXPAND_BUTTON_SIZE + IEXPAND_SPACING) - IEXPAND_BACK_MARGIN) &&
          (x < width - (2 * IEXPAND_BUTTON_SIZE + 3 * IEXPAND_SPACING) - IEXPAND_BACK_MARGIN))
          return iGLExpanderCallExtraButtonCb(ih, 3, pressed);
      }
    }
  }

  if (pressed)
  {
    /* Update the state: OPEN ==> collapsed, CLOSE ==> expanded */
    iGLExpanderOpenCloseChild(ih, 1, 1, ih->data->state == IEXPANDER_OPEN ? IEXPANDER_CLOSE : IEXPANDER_OPEN);
  }

  (void)x;
  (void)y;
  (void)status;
  return IUP_DEFAULT;
}
#endif

static void iGLExpanderDrawTriangle(Ihandle *ih, int x, int y, const char* color, int active, int dir)
{
  int points[6];

  /* fix for smooth triangle */
  int delta = (IEXPAND_HANDLE_SIZE - 2*IEXPAND_SPACING)/2;

  switch(dir)
  {
  case IEXPANDER_LEFT:  /* arrow points left */
    x += IEXPAND_SPACING;  /* fix center */
    points[0] = x + IEXPAND_HANDLE_SIZE - IEXPAND_SPACING - delta;
    points[1] = y + IEXPAND_SPACING;
    points[2] = x + IEXPAND_HANDLE_SIZE - IEXPAND_SPACING - delta;
    points[3] = y + IEXPAND_HANDLE_SIZE - IEXPAND_SPACING;
    points[4] = x + IEXPAND_SPACING;
    points[5] = y + IEXPAND_HANDLE_SIZE/2;
    break;
  case IEXPANDER_TOP:    /* arrow points top */
    y += IEXPAND_SPACING;  /* fix center */
    points[0] = x + IEXPAND_SPACING;
    points[1] = y + IEXPAND_HANDLE_SIZE - IEXPAND_SPACING - (delta-1);
    points[2] = x + IEXPAND_HANDLE_SIZE - IEXPAND_SPACING;
    points[3] = y + IEXPAND_HANDLE_SIZE - IEXPAND_SPACING - (delta-1);
    points[4] = x + IEXPAND_HANDLE_SIZE/2;
    points[5] = y + IEXPAND_SPACING;
    break;
  case IEXPANDER_RIGHT:  /* arrow points right */
    x += IEXPAND_SPACING;  /* fix center */
    points[0] = x + IEXPAND_SPACING;
    points[1] = y + IEXPAND_SPACING;
    points[2] = x + IEXPAND_SPACING;
    points[3] = y + IEXPAND_HANDLE_SIZE - IEXPAND_SPACING;
    points[4] = x + IEXPAND_HANDLE_SIZE - IEXPAND_SPACING - delta;
    points[5] = y + IEXPAND_HANDLE_SIZE/2;
    break;
  case IEXPANDER_BOTTOM:  /* arrow points bottom */
    y += IEXPAND_SPACING;  /* fix center */
    points[0] = x + IEXPAND_SPACING;
    points[1] = y + IEXPAND_SPACING;
    points[2] = x + IEXPAND_HANDLE_SIZE - IEXPAND_SPACING;
    points[3] = y + IEXPAND_SPACING;
    points[4] = x + IEXPAND_HANDLE_SIZE/2;
    points[5] = y + IEXPAND_HANDLE_SIZE - IEXPAND_SPACING - (delta-1);
    break;
  }

  iupGLDrawPolygon(ih, points, 3, color, active);
  iupGLDrawPolyline(ih, points, 3, 1, color, active);
}

static void iGLExpanderDrawSmallTriangle(Ihandle *ih, int x, int y, const char* color, int active, int dir)
{
  int points[6];
  int size = IEXPAND_HANDLE_SIZE-2;
  int space = IEXPAND_SPACING+1;

  /* fix for smooth triangle */
  int delta = (size - 2*space)/2;

  switch(dir)
  {
  case IEXPANDER_RIGHT:  /* arrow points right */
    x += space-1;  /* fix center */
    y += 1;
    points[0] = x + space;
    points[1] = y + space;
    points[2] = x + space;
    points[3] = y + size - space;
    points[4] = x + size - space - delta;
    points[5] = y + size/2;
    break;
  case IEXPANDER_BOTTOM:  /* arrow points bottom */
    y += space;  /* fix center */
    points[0] = x + space;
    points[1] = y + space;
    points[2] = x + size - space;
    points[3] = y + space;
    points[4] = x + size/2;
    points[5] = y + size - space - (delta-1);
    break;
  }

  iupGLDrawPolygon(ih, points, 3, color, active);
  iupGLDrawPolyline(ih, points, 3, 1, color, active);
}

static void iGLExpanderDrawArrow(Ihandle *ih, int x, int y, const char* color, int active, int dir)
{
  iGLExpanderDrawTriangle(ih, x, y, color, active, dir);
}

static void iGLExpanderDrawSmallArrow(Ihandle *ih, const char* color, int active, int dir, int y_offset)
{
  switch(dir)
  {
  case IEXPANDER_RIGHT:  /* arrow points right */
    iGLExpanderDrawSmallTriangle(ih, 1 + IEXPAND_BACK_MARGIN, 0 + IEXPAND_BACK_MARGIN + y_offset, color, active, dir);
    break;
  case IEXPANDER_BOTTOM:  /* arrow points bottom */
    iGLExpanderDrawSmallTriangle(ih, 0 + IEXPAND_BACK_MARGIN, 0 + IEXPAND_BACK_MARGIN + y_offset, color, active, dir);
    break;
  }
}

static void iGLExpanderDrawExtraButton(Ihandle* ih, int button, int x, int y, int height)
{
  char* image = iupAttribGetId(ih, "IMAGEEXTRA", button);
  int active = IupGetInt(ih, "ACTIVE");
  int img_height;

  if (!image)
    return;

  if (ih->data->extra_buttons_state[button] == 1)
  {
    char* impress = iupAttribGetId(ih, "IMAGEEXTRAPRESS", button);
    if (impress) image = impress;
  }
  else if (ih->data->extra_buttons_state[button] == -1)
  {
    char* imhighlight = iupAttribGetId(ih, "IMAGEEXTRAHIGHLIGHT", button);
    if (imhighlight) image = imhighlight;
  }

  iupGLImageGetInfo(image, NULL, &img_height, NULL);
  if (height > img_height)
    y += (height - img_height) / 2;

  iupGLDrawImage(ih, x, y, image, !active);
}

static int iGLExpanderACTION_CB(Ihandle* ih)
{
  char *image = iupAttribGet(ih, "IMAGE");
  char* title = iupAttribGet(ih, "TITLE");
  int active = iupAttribGetInt(ih, "ACTIVE");
  int x1, y1, x2, y2;
  int bar_size = iGLExpanderGetBarSize(ih);
  char* fgcolor = iupAttribGetStr(ih, "FORECOLOR");
  char* bgcolor = iupAttribGetStr(ih, "BACKCOLOR");
  int highlight = iupAttribGetInt(ih, "HIGHLIGHT");

  /* calc bar position */
  if (ih->data->position == IEXPANDER_LEFT)
  {
    x1 = 0;
    x2 = bar_size;
    y1 = 0;
    y2 = ih->currentheight - 1;
  }
  else if (ih->data->position == IEXPANDER_RIGHT)
  {
    x1 = ih->currentwidth-1 - bar_size;
    x2 = ih->currentwidth-1;
    y1 = 0;
    y2 = ih->currentheight - 1;
  }
  else if (ih->data->position == IEXPANDER_BOTTOM)
  {
    x1 = 0;
    x2 = ih->currentwidth - 1;
    y1 = ih->currentheight-1 - bar_size;
    y2 = ih->currentheight-1;
  }
  else /* IEXPANDER_TOP */
  {
    x1 = 0;
    x2 = ih->currentwidth - 1;
    y1 = 0;
    y2 = bar_size;
  }

  /* draw bar box */
  iupGLDrawBox(ih, x1, x2, y1, y2, bgcolor, 1);

  if (ih->data->position == IEXPANDER_TOP && (title || image || ih->data->extra_buttons != 0))
  {
    int txt_offset = IEXPAND_HANDLE_SIZE;

    if (image)
    {
      int active = IupGetInt(ih, "ACTIVE");
      int img_width = 0, img_height = 0;
      int y_offset = 0;

      if (ih->data->state != IEXPANDER_CLOSE)
      {
        char* imopen = iupAttribGetStr(ih, "IMAGEOPEN");
        if (imopen) image = imopen;

        if (highlight)
        {
          char* imhighlight = iupAttribGetStr(ih, "IMAGEOPENHIGHLIGHT");
          if (imhighlight) image = imhighlight;
        }
      }
      else if (highlight)
      {
        char* imhighlight = iupAttribGetStr(ih, "IMAGEHIGHLIGHT");
        if (imhighlight) image = imhighlight;
      }

      iupGLImageGetInfo(image, &img_width, &img_height, NULL);
      if (bar_size > img_height)
        y_offset = (bar_size - img_height) / 2;

      iupGLDrawImage(ih, IEXPAND_BACK_MARGIN, y_offset, image, !active);

      txt_offset = iupMAX(txt_offset, img_width);
    }

    if (title)
    {
      /* left align everything */
      int height;
      iupGLFontGetMultiLineStringSize(ih, title, NULL, &height);
      iupGLDrawText(ih, txt_offset + IEXPAND_SPACING, (bar_size - height) / 2, title, fgcolor, active);

      if (!image)
      {
        int y_offset = 0;
        if (bar_size > IEXPAND_HANDLE_SIZE + 2 * IEXPAND_BACK_MARGIN)
          y_offset = (bar_size - IEXPAND_HANDLE_SIZE - 2 * IEXPAND_BACK_MARGIN) / 2;

        if (highlight)
        {
          char* hlcolor = iupAttribGetStr(ih, "HIGHCOLOR");
          if (hlcolor)
            fgcolor = hlcolor;
        }

        if (ih->data->state == IEXPANDER_CLOSE)
          iGLExpanderDrawSmallArrow(ih, fgcolor, active, IEXPANDER_RIGHT, y_offset);
        else
          iGLExpanderDrawSmallArrow(ih, fgcolor, active, IEXPANDER_BOTTOM, y_offset);
      }
    }

    if (ih->data->extra_buttons != 0)
    {
      int y = IEXPAND_SPACING + IEXPAND_BACK_MARGIN,
        height = bar_size - 2 * (IEXPAND_SPACING + IEXPAND_BACK_MARGIN);

      iGLExpanderDrawExtraButton(ih, 1, ih->currentwidth - (IEXPAND_BUTTON_SIZE + IEXPAND_SPACING) - IEXPAND_BACK_MARGIN, y, height);

      if (ih->data->extra_buttons > 1)
        iGLExpanderDrawExtraButton(ih, 2, ih->currentwidth - 2 * (IEXPAND_BUTTON_SIZE + IEXPAND_SPACING) - IEXPAND_BACK_MARGIN, y, height);

      if (ih->data->extra_buttons == 3)
        iGLExpanderDrawExtraButton(ih, 3, ih->currentwidth - 3 * (IEXPAND_BUTTON_SIZE + IEXPAND_SPACING) - IEXPAND_BACK_MARGIN, y, height);
    }
  }
  else
  {
    /* center align the arrow */
    int x = x1, 
        y = y1, 
        width = x2 - x1 + 1,
        height = y2 - y1 + 1;

    if (highlight)
    {
      char* hlcolor = iupAttribGetStr(ih, "HIGHCOLOR");
      if (hlcolor)
        fgcolor = hlcolor;
    }

    switch (ih->data->position)
    {
    case IEXPANDER_LEFT:
      x += 0;
      y += (height - IEXPAND_HANDLE_SIZE) / 2;
      if (ih->data->state == IEXPANDER_CLOSE)
        iGLExpanderDrawArrow(ih, x, y, fgcolor, active, IEXPANDER_RIGHT);
      else
        iGLExpanderDrawArrow(ih, x, y, fgcolor, active, IEXPANDER_LEFT);
      break;
    case IEXPANDER_TOP:
      x += (width - IEXPAND_HANDLE_SIZE) / 2;
      y += 0;
      if (ih->data->state == IEXPANDER_CLOSE)
        iGLExpanderDrawArrow(ih, x, y, fgcolor, active, IEXPANDER_BOTTOM);
      else
        iGLExpanderDrawArrow(ih, x, y, fgcolor, active, IEXPANDER_TOP);
      break;
    case IEXPANDER_RIGHT:
      x += 0;
      y += (height - IEXPAND_HANDLE_SIZE) / 2;
      if (ih->data->state == IEXPANDER_CLOSE)
        iGLExpanderDrawArrow(ih, x, y, fgcolor, active, IEXPANDER_LEFT);
      else
        iGLExpanderDrawArrow(ih, x, y, fgcolor, active, IEXPANDER_RIGHT);
      break;
    case IEXPANDER_BOTTOM:
      x += (width - IEXPAND_HANDLE_SIZE) / 2;
      y += 0;
      if (ih->data->state == IEXPANDER_CLOSE)
        iGLExpanderDrawArrow(ih, x, y, fgcolor, active, IEXPANDER_TOP);
      else
        iGLExpanderDrawArrow(ih, x, y, fgcolor, active, IEXPANDER_BOTTOM);
      break;
    }
  }

  return IUP_DEFAULT;
}

/*****************************************************************************\
|* Attributes                                                                *|
\*****************************************************************************/


static char* iGLExpanderGetClientSizeAttrib(Ihandle* ih)
{
  int width = ih->currentwidth;
  int height = ih->currentheight;
  int bar_size = iGLExpanderGetBarSize(ih);

  if (ih->data->position == IEXPANDER_LEFT || ih->data->position == IEXPANDER_RIGHT)
    width -= bar_size;
  else
    height -= bar_size;

  if (width < 0) width = 0;
  if (height < 0) height = 0;
  return iupStrReturnIntInt(width, height, 'x');
}

static char* iGLExpanderGetClientOffsetAttrib(Ihandle* ih)
{
  int dx = 0, dy = 0;
  int bar_size = iGLExpanderGetBarSize(ih);

  if (ih->data->position == IEXPANDER_LEFT)
    dx += bar_size;
  else if (ih->data->position == IEXPANDER_TOP)
    dy += bar_size;

  return iupStrReturnIntInt(dx, dy, 'x');
}

static int iGLExpanderSetPositionAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "LEFT"))
    ih->data->position = IEXPANDER_LEFT;
  else if (iupStrEqualNoCase(value, "RIGHT"))
    ih->data->position = IEXPANDER_RIGHT;
  else if (iupStrEqualNoCase(value, "BOTTOM"))
    ih->data->position = IEXPANDER_BOTTOM;
  else  /* Default = TOP */
    ih->data->position = IEXPANDER_TOP;

  return 0;  /* do not store value in hash table */
}

static int iGLExpanderSetBarSizeAttrib(Ihandle* ih, const char* value)
{
  if (!value)
    ih->data->barSize = -1;
  else
    iupStrToInt(value, &ih->data->barSize);  /* must manually update layout */
  return 0; /* do not store value in hash table */
}

static char* iGLExpanderGetBarSizeAttrib(Ihandle* ih)
{
  int bar_size = iGLExpanderGetBarSize(ih);
  return iupStrReturnInt(bar_size);
}

static int iGLExpanderSetStateAttrib(Ihandle* ih, const char* value)
{
  int state;
  if (iupStrEqualNoCase(value, "OPEN"))
    state = IEXPANDER_OPEN;
  else
    state = IEXPANDER_CLOSE;

  iGLExpanderOpenCloseChild(ih, 1, 0, state);

  return 0; /* do not store value in hash table */
}

static char* iGLExpanderGetStateAttrib(Ihandle* ih)
{
  if (ih->data->state)
    return "OPEN";
  else
    return "CLOSE";
}

static int iGLExpanderSetExtraButtonsAttrib(Ihandle* ih, const char* value)
{
  if (!value)
    ih->data->extra_buttons = 0;
  else
  {
    iupStrToInt(value, &(ih->data->extra_buttons));
    if (ih->data->extra_buttons < 0)
      ih->data->extra_buttons = 0;
    else if (ih->data->extra_buttons > 3)
      ih->data->extra_buttons = 3;
  }
  return 0; /* do not store value in hash table */
}

static char* iGLExpanderGetExtraButtonsAttrib(Ihandle* ih)
{
  return iupStrReturnInt(ih->data->extra_buttons);
}

static int iGLExpanderSetMoveableAttrib(Ihandle* ih, const char* value)
{
  Ihandle* gl_parent = (Ihandle*)iupAttribGet(ih, "GL_CANVAS");
  /* only a direct child of the canvabox can be moved */
  if (iupStrBoolean(value) && ih->parent == gl_parent)
    return 1;
  else
  {
    iupAttribSet(ih, "MOVEABLE", "NO");
    return 0;
  }
}


/*****************************************************************************\
|* Methods                                                                   *|
\*****************************************************************************/


static void iGLExpanderComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  int child_expand = 0,
      natural_w, natural_h;
  Ihandle *child = ih->firstchild;
  int bar_size = iGLExpanderGetBarSize(ih);

  /* bar */
  if (ih->data->position == IEXPANDER_LEFT || ih->data->position == IEXPANDER_RIGHT)
  {
    natural_w = bar_size;
    natural_h = IEXPAND_HANDLE_SIZE;
  }
  else
  {
    natural_w = IEXPAND_HANDLE_SIZE;
    natural_h = bar_size;

    if (ih->data->position == IEXPANDER_TOP)
    {
      /* if IMAGE is defined assume that will cover all the canvas area */
      char* value = iupAttribGetStr(ih, "IMAGE");
      if (value)
      {
        int image_w = 0;
        iupGLImageGetInfo(value, &image_w, NULL, NULL);
        natural_w = iupMAX(natural_w, image_w);
      }

      /* if TITLE and IMAGE are both defined then 
         IMAGE is only the handle */

      value = iupAttribGetStr(ih, "TITLE");
      if (value)
      {
        int title_size = 0;
        iupGLFontGetMultiLineStringSize(ih, value, &title_size, NULL);
        natural_w += title_size + IEXPAND_SPACING;
      }

      if (ih->data->extra_buttons != 0)
        natural_w += ih->data->extra_buttons * (IEXPAND_BUTTON_SIZE + IEXPAND_SPACING);

      natural_w += 2 * IEXPAND_BACK_MARGIN;
    }
  }

  if (child)
  {
    /* update child natural bar_size first */
    iupBaseComputeNaturalSize(child);

    if (ih->data->position == IEXPANDER_LEFT || ih->data->position == IEXPANDER_RIGHT)
    {
      if (ih->data->state == IEXPANDER_OPEN)  /* only open, not float */
        natural_w += child->naturalwidth;
      natural_h = iupMAX(natural_h, child->naturalheight);
    }
    else
    {
      natural_w = iupMAX(natural_w, child->naturalwidth);
      if (ih->data->state == IEXPANDER_OPEN)  /* only open, not float */
        natural_h += child->naturalheight;
    }

    if (ih->data->state == IEXPANDER_OPEN)
      child_expand = child->expand;
    else
    {
      if (ih->data->position == IEXPANDER_LEFT || ih->data->position == IEXPANDER_RIGHT)
        child_expand = child->expand & IUP_EXPAND_HEIGHT;  /* only vertical allowed */
      else
        child_expand = child->expand & IUP_EXPAND_WIDTH;  /* only horizontal allowed */
    }
  }

  *children_expand = child_expand;
  *w = natural_w;
  *h = natural_h;
}

static void iGLExpanderSetChildrenCurrentSizeMethod(Ihandle* ih, int shrink)
{
  Ihandle *child = ih->firstchild;
  int width = ih->currentwidth;
  int height = ih->currentheight;
  int bar_size = iGLExpanderGetBarSize(ih);

  if (ih->data->position == IEXPANDER_LEFT || ih->data->position == IEXPANDER_RIGHT)
  {
    if (ih->currentwidth < bar_size)
      ih->currentwidth = bar_size;

    width = ih->currentwidth - bar_size;
  }
  else /* IEXPANDER_TOP OR IEXPANDER_BOTTOM */
  {
    if (ih->currentheight < bar_size)
      ih->currentheight = bar_size;

    height = ih->currentheight - bar_size;
  }

  if (child)
  {
    if (ih->data->state == IEXPANDER_OPEN)
      iupBaseSetCurrentSize(child, width, height, shrink);
  }
}

static void iGLExpanderSetChildrenPositionMethod(Ihandle* ih, int x, int y)
{
  Ihandle *child = ih->firstchild;
  if (child)
  {
    int bar_size = iGLExpanderGetBarSize(ih);

    if (ih->data->position == IEXPANDER_LEFT)
      x += bar_size;
    else if (ih->data->position == IEXPANDER_TOP)
      y += bar_size;

    if (ih->data->state == IEXPANDER_OPEN)
      iupBaseSetPosition(child, x, y);
  }
}

static void iGLExpanderChildAddedMethod(Ihandle* ih, Ihandle* child)
{
  iGLExpanderOpenCloseChild(ih, 0, 0, ih->data->state);
  (void)child;
}

static int iGLExpanderCreateMethod(Ihandle* ih, void** params)
{
  ih->data = iupALLOCCTRLDATA();

  ih->data->position = IEXPANDER_TOP;
  ih->data->state = IEXPANDER_OPEN;
  ih->data->barSize = -1;

  /* Setting callbacks */
  IupSetCallback(ih, "GL_ACTION", (Icallback)iGLExpanderACTION_CB);
//  IupSetCallback(ih, "GL_BUTTON_CB", (Icallback)iGLExpanderBUTTON_CB);
//  IupSetCallback(ih, "GL_MOTION_CB", (Icallback)iGLExpanderMOTION_CB);
  IupSetCallback(ih, "GL_LEAVEWINDOW_CB", iupGLSubCanvasRestoreRedraw);
  IupSetCallback(ih, "GL_ENTERWINDOW_CB", iupGLSubCanvasRestoreRedraw);

  if (params)
  {
    Ihandle** iparams = (Ihandle**)params;
    if (*iparams)
      IupAppend(ih, *iparams);
  }

  return IUP_NOERROR;
}

Iclass* iupGLExpanderNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("glsubcanvas"));

  ic->name   = "glexpander";
  ic->format = "h";   /* one ihandle */
  ic->nativetype = IUP_TYPEVOID;
  ic->childtype  = IUP_CHILDMANY+1;  /* one child */
  ic->is_interactive = 1;

  /* Class functions */
  ic->New     = iupGLExpanderNewClass;
  ic->Create  = iGLExpanderCreateMethod;
  ic->Map     = iupBaseTypeVoidMapMethod;
  ic->ChildAdded = iGLExpanderChildAddedMethod;

  ic->ComputeNaturalSize     = iGLExpanderComputeNaturalSizeMethod;
  ic->SetChildrenCurrentSize = iGLExpanderSetChildrenCurrentSizeMethod;
  ic->SetChildrenPosition    = iGLExpanderSetChildrenPositionMethod;

  /* Callbacks */
  iupClassRegisterCallback(ic, "ACTION", "");
  iupClassRegisterCallback(ic, "OPENCLOSE_CB", "i");
  iupClassRegisterCallback(ic, "EXTRABUTTON_CB", "ii");
  iupClassRegisterCallback(ic, "MOVE_CB", "ii");

  /* Base Container */
  iupClassRegisterAttribute(ic, "EXPAND", iupBaseContainerGetExpandAttrib, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLIENTSIZE", iGLExpanderGetClientSizeAttrib, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLIENTOFFSET", iGLExpanderGetClientOffsetAttrib, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_READONLY|IUPAF_NO_INHERIT);

  /* Visual */
  /* NOTICE: avoid defining inheritable attributes for containers */

  iupClassRegisterAttribute(ic, "BARPOSITION", NULL, iGLExpanderSetPositionAttrib, IUPAF_SAMEASSYSTEM, "TOP", IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BARSIZE", iGLExpanderGetBarSizeAttrib, iGLExpanderSetBarSizeAttrib, IUPAF_SAMEASSYSTEM, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "STATE", iGLExpanderGetStateAttrib, iGLExpanderSetStateAttrib, IUPAF_SAMEASSYSTEM, "OPEN", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "EXTRABUTTONS", iGLExpanderGetExtraButtonsAttrib, iGLExpanderSetExtraButtonsAttrib, IUPAF_SAMEASSYSTEM, NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MOVEABLE", NULL, iGLExpanderSetMoveableAttrib, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "FORECOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "255 255 255", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "50 100 150", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HIGHCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "200 225 245", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLE", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLEOFFSET", NULL, NULL, IUPAF_SAMEASSYSTEM, "5", IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "IMAGE", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEHIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEOPEN", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEOPENHIGHLIGHT", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "IMAGEEXTRA1", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEEXTRAPRESS1", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEEXTRAHIGHLIGHT1", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEEXTRA2", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEEXTRAPRESS2", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEEXTRAHIGHLIGHT2", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEEXTRA3", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEEXTRAPRESS3", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEEXTRAHIGHLIGHT3", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  return ic;
}

Ihandle* IupGLExpander(Ihandle* child)
{
  void *children[2];
  children[0] = (void*)child;
  children[1] = NULL;
  return IupCreatev("glexpander", children);
}
