 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_PCIFFMAN
#define H_OBJSMOOTH_PCIFFMAN

class PCIFFManager;

#include <smooth.h>

using namespace smooth;
using namespace smooth::GUI;
using namespace smooth::GUI::Dialogs;

class PCIFFManager : public Application
{
	private:
		GUI::Window	*wnd;
		Titlebar	*title;

		Menubar		*menubar;
		Statusbar	*statusbar;

		PopupMenu	*menu_file;

		String		 filename;

		Void		 Close();
		Void		 OpenFile();
	public:
				 PCIFFManager();
				~PCIFFManager();
};

#endif
