/** \file
 * \brief Toggle Control
 *
 * See Copyright Notice in "iup.h"
 */

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

#include <Button.h>
#include <CheckBox.h>
#include <RadioButton.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_layout.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_image.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_image.h"
#include "iup_key.h"
#include "iup_toggle.h"

#include "iuphaiku_drv.h"

#define IUP_TOGGLE_BOX 16

void iupdrvToggleAddCheckBox(int *x, int *y, const char* str)
{
  /* has margins too */
  (*x) += 2+IUP_TOGGLE_BOX+2;
  if ((*y) < 2+IUP_TOGGLE_BOX+2) (*y) = 2+IUP_TOGGLE_BOX+2; /* minimum height */
  else (*y) += 2+2;

  if (str && str[0]) /* add spacing between check box and text */
    (*x) += 8;
}

static int beToggleGetCheck(Ihandle* ih)
{
	BControl* control = (BControl*)ih->handle;
    return control->Value();
}

static void beToggleUpdateImage(Ihandle* ih, int active, int check)
{
  char* name;
  BButton* button = (BButton*)ih->handle;

  BBitmap* bitmap = (BBitmap*)iupImageGetImage(iupAttribGet(ih, "IMAGE"), ih, check);
  button->SetIcon(bitmap);

#if 0
  name = iupAttribGet(ih, "IMINACTIVE");
  if (name)
  	pic = getPicture(ih, name, 0);
  else
    pic = getPicture(ih, iupAttribGet(ih, "IMAGE"), 1);
  button->SetDisabledOff(pic);
  button->SetDisabledOn(pic);

  name = iupAttribGet(ih, "IMPRESS");
  if (name)
  	pic = getPicture(ih, name, 0);
  else
    pic = getPicture(ih, iupAttribGet(ih, "IMAGE"), 0, true);
  button->SetEnabledOn(pic);
#endif
}


/*************************************************************************/


static int gtkToggleSetValueAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value,"NOTDEF"))
	UNIMPLEMENTED
  else 
  {
	int check;
    BControl* checkbox = (BCheckBox*)ih->handle;

    if (iupStrEqualNoCase(value,"TOGGLE")) {
	  check = checkbox->Value();
	  if (check == 0) check = 1;
	  else check = 0;
	} else
      check = iupStrBoolean(value);

	checkbox->SetValue(check);
  }

  return 0;
}

static char* gtkToggleGetValueAttrib(Ihandle* ih)
{
  int check = beToggleGetCheck(ih);
  if (check == -1)
    return "NOTDEF";
  else if (check == 1)
    return "ON";
  else
    return "OFF";
}

static int beToggleSetTitleAttrib(Ihandle* ih, const char* value)
{
  BControl* checkbox = (BCheckBox*)ih->handle;
  checkbox->SetLabel(value);
  return 1;
}

static int gtkToggleSetAlignmentAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
}

static int gtkToggleSetPaddingAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
    return 1; /* store until not mapped, when mapped will be set again */
}

static int gtkToggleSetFgColorAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
}

/*
static int gtkToggleSetStandardFontAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
}
*/

static int beToggleSetImageAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->type == IUP_TOGGLE_IMAGE)
  {
    if (value != iupAttribGet(ih, "IMAGE"))
      iupAttribSet(ih, "IMAGE", (char*)value);
    beToggleUpdateImage(ih, iupdrvIsActive(ih), beToggleGetCheck(ih));
    return 1;
  }
  else
    return 0;
}

static int beToggleSetImInactiveAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->type == IUP_TOGGLE_IMAGE)
  {
    if (value != iupAttribGet(ih, "IMINACTIVE"))
      iupAttribSet(ih, "IMINACTIVE", (char*)value);
    beToggleUpdateImage(ih, iupdrvIsActive(ih), beToggleGetCheck(ih));
    return 1;
  }
  else
    return 0;
}

static int gtkToggleSetImPressAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->type == IUP_TOGGLE_IMAGE)
  {
    if (value != iupAttribGet(ih, "IMPRESS"))
      iupAttribSet(ih, "IMPRESS", (char*)value);
    beToggleUpdateImage(ih, iupdrvIsActive(ih), beToggleGetCheck(ih));
    return 1;
  }
  else
    return 0;
}

// Event handlers

void iuphaikuToggleInvoked(Ihandle* ih, int value)
{
  IFni cb = (IFni) IupGetCallback(ih, "ACTION");
  if (cb && cb(ih, value) == IUP_CLOSE)
    IupExitLoop();

  if (iupObjectCheck(ih))
    iupBaseCallValueChangedCb(ih);
}

/****************************************************************************************************/

static int gtkToggleMapMethod(Ihandle* ih)
{
  Ihandle* radio = iupRadioFindToggleParent(ih);

  if (radio)
    ih->data->is_radio = 1;
  
  char* value = iupAttribGet(ih, "IMAGE");
  if (value)
  {
    ih->data->type = IUP_TOGGLE_IMAGE;
	// The pictures will be set only later, from the image callbacks.
	ih->handle = (InativeHandle*)new BButton(BRect(0,0,10,10), "PictureButton", NULL, NULL);
  }
  else
  {
    ih->data->type = IUP_TOGGLE_TEXT;

    if (radio) {
      ih->handle = (InativeHandle*)new BRadioButton(BRect(0,0,10,10), "Radio", NULL, NULL);
    } else {
      ih->handle = (InativeHandle*)new BCheckBox(BRect(0,0,10,10), "Checkbox", NULL, NULL);
      if (iupAttribGetBoolean(ih, "3STATE")) {
		  ((BCheckBox*)ih->handle)->SetPartialStateToOff(true);
	  }
    }
  }

  BControl* ctrl = (BControl*)(ih->handle);
  BMessage* message = new BMessage(checkboxInvoke);
  message->AddPointer("iHandle", ih);
  ctrl->SetMessage(message);

  iuphaikuBaseAddToParent(ih);
  return IUP_NOERROR;
}

void iupdrvToggleInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = gtkToggleMapMethod;

  /* Driver Dependent Attribute functions */

  /* Overwrite Common */
  //iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, gtkToggleSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, gtkToggleSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_DEFAULT);  /* black */
  iupClassRegisterAttribute(ic, "TITLE", NULL, beToggleSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

  /* IupToggle only */
  iupClassRegisterAttribute(ic, "ALIGNMENT", NULL, gtkToggleSetAlignmentAttrib, "ACENTER:ACENTER", NULL, IUPAF_NO_INHERIT); /* force new default value */
  iupClassRegisterAttribute(ic, "IMAGE", NULL, beToggleSetImageAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMINACTIVE", NULL, beToggleSetImInactiveAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMPRESS", NULL, gtkToggleSetImPressAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUE", gtkToggleGetValueAttrib, gtkToggleSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "PADDING", iupToggleGetPaddingAttrib, gtkToggleSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "MARKUP", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);

  /* NOT supported */
  iupClassRegisterAttribute(ic, "RIGHTBUTTON", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_DEFAULT);
}
