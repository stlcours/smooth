 /* The smooth Class Library
  * Copyright (C) 1998-2003 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the "Artistic License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_DLLMAIN_
#define _H_OBJSMOOTH_DLLMAIN_

#include "system.h"

#ifdef __WIN32__
extern HINSTANCE	 hDllInstance;

#ifndef __SMOOTH_STATIC__

extern "C"
{
	BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID);
}

#endif
#endif

#endif