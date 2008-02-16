 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_XULLOADER_
#define _H_OBJSMOOTH_XULLOADER_

namespace smooth
{
	namespace GUI
	{
		class XULLoader;
	};

	namespace XML
	{
		namespace XUL
		{
			class Renderer;
		};
	};
};

#include "application.h"

namespace smooth
{
	namespace GUI
	{
		class XULLoader : public Application
		{
			private:
				XML::XUL::Renderer	*xulRenderer;
			public:
							 XULLoader(const String &);
							~XULLoader();
		};

		extern "C"
		{
			SMOOTHAPI int WINAPI LoadXUL(HWND, HINSTANCE, LPSTR, int);
		}
	};
};

#endif
