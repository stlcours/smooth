 /* The SMOOTH Windowing Toolkit
  * Copyright (C) 1998-2002 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the "Artistic License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef __OBJSMOOTH_MDIWINDOW_
#define __OBJSMOOTH_MDIWINDOW_

#include <smooth/mdiwindow.h>
#include <smooth/window.h>
#include <smooth/definitions.h>
#include <smooth/stk.h>
#include <smooth/objectproperties.h>

#ifdef __WIN32__
__declspec (dllexport)
#endif

SMOOTHInt	 OBJ_MDIWINDOW = SMOOTH::RequestObjectID();

SMOOTHMDIWindow::SMOOTHMDIWindow(SMOOTHString title) : SMOOTHWindow(title)
{
	type				= OBJ_MDIWINDOW;
	containerType			= OBJ_WINDOW;

	objectProperties->pos.x		= 0;
	objectProperties->pos.y		= 0;
	objectProperties->size.cx	= 100;
	objectProperties->size.cy	= 100;
	objectProperties->orientation	= OR_FREE;

	possibleContainers.DeleteAll();
	possibleContainers.AddEntry(OBJ_WINDOW);
}

SMOOTHMDIWindow::~SMOOTHMDIWindow()
{
	if (registered && myContainer != NIL) myContainer->UnregisterObject(this);
}

#endif
