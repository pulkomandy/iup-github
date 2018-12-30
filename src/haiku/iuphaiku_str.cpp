/** \file
 * \brief Haiku string Functions
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "iup_str.h"


#ifdef USE_ICONV
#include <iconv.h>
#endif

static char* iupCheckUtf8Buffer(char* utf8_buffer, int *utf8_buffer_max, int len)
{
  if (!utf8_buffer)
  {
    utf8_buffer = (char*)malloc(len + 1);
    *utf8_buffer_max = len;
  }
  else if (*utf8_buffer_max < len)
  {
    utf8_buffer = (char*)realloc(utf8_buffer, len + 1);
    *utf8_buffer_max = len;
  }

  return utf8_buffer;
}

static char* iupStrCopyToUtf8Buffer(const char* str, int len, char* utf8_buffer, int *utf8_buffer_max)
{
  utf8_buffer = iupCheckUtf8Buffer(utf8_buffer, utf8_buffer_max, len);
  memcpy(utf8_buffer, str, len);
  utf8_buffer[len] = 0;
  return utf8_buffer;
}

extern "C" char* iupStrConvertToUTF8(const char* str, int len, char* utf8_buffer, int *utf8_buffer_max, int utf8mode)
{
  if (utf8mode || iupStrIsAscii(str)) /* string is already utf8 or is ascii */
    return iupStrCopyToUtf8Buffer(str, len, utf8_buffer, utf8_buffer_max);

#ifdef USE_ICONV
  {
    size_t ulen = (size_t)len;
    size_t mlen = ulen * 2;
    iconv_t cd_iconv = iconv_open("UTF-8", "ISO-8859-1");

    if (cd_iconv == (iconv_t)-1)
      return iupStrCopyToUtf8Buffer(str, len, utf8_buffer, utf8_buffer_max);

    utf8_buffer = iupCheckUtf8Buffer(utf8_buffer, utf8_buffer_max, mlen);

    iconv(cd_iconv, (char**)&str, &ulen, &utf8_buffer, &mlen);
    utf8_buffer[mlen] = 0;

    iconv_close(cd_iconv);
  }
#else
  return iupStrCopyToUtf8Buffer(str, len, utf8_buffer, utf8_buffer_max);
#endif

  return utf8_buffer;
}

