/** \file
 * \brief GTK Driver IupHelp for non Windows systems
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "iup.h"

#include "iup_str.h"

int IupExecute(const char *filename, const char* parameters)
{
  int ret;
  if (parameters)
  {
    char* cmd = (char*)malloc(sizeof(char)*(strlen(filename) + strlen(parameters) + 4));
    sprintf(cmd, "%s %s &", filename, parameters);
	fprintf(stderr, "%s(%s)\n", __func__, cmd);
    ret = system(cmd);
    free(cmd);
  }
  else
  {
    char* cmd = (char*)malloc(sizeof(char)*(strlen(filename) + 3));
    sprintf(cmd, "%s &", filename);
    ret = system(cmd);
    free(cmd);
  }
  if (ret == -1)
    return -1;
  return 1;
}

int IupExecuteWait(const char *filename, const char* parameters)
{
  int ret;
  if (parameters)
  {
    char* cmd = (char*)malloc(sizeof(char)*(strlen(filename) + strlen(parameters) + 3));
    sprintf(cmd, "%s %s", filename, parameters);
    ret = system(cmd);
    free(cmd);
  }
  else
    ret = system(filename);
  if (ret == -1)
    return -1;
  return 1;
}

int IupHelp(const char *url)
{
  char *browser = getenv("IUP_HELPAPP");
  if (!browser) 
    browser = IupGetGlobal("HELPAPP");

  // TODO use mime db directly?
  if (!browser) 
  { 
	  browser = "open";
  }
  
  return IupExecute(browser, url);
}
