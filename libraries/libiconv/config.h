/* Copyright (C) 1999-2000 Free Software Foundation, Inc.
   This file is part of the GNU LIBICONV Library.

   The GNU LIBICONV Library is free software; you can redistribute it
   and/or modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   The GNU LIBICONV Library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU LIBICONV Library; see the file COPYING.LIB.
   If not, write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA.  */


/* Define if you have <locale.h>. */
#undef HAVE_LOCALE_H

/* Define to a type if <wchar.h> does not define. */
#undef mbstate_t

/* Define if you have <iconv.h>, the iconv_t type, and the
   iconv_open, iconv, iconv_close functions. */
#undef HAVE_ICONV
/* Define as const if the declaration of iconv() needs const. */
#define ICONV_CONST const

/* Define if you have the mbrtowc() function. */
#undef HAVE_MBRTOWC

/* Define if you have the wcrtomb() function. */
#undef HAVE_WCRTOMB

/* Define if you have the setlocale() function. */
#undef HAVE_SETLOCALE

/* Define if the machine's byte ordering is little endian. */
#define WORDS_LITTLEENDIAN 1

