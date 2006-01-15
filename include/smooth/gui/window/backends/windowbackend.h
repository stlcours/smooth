 /* The smooth Class Library
  * Copyright (C) 1998-2006 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_WINDOWBACKEND_
#define _H_OBJSMOOTH_WINDOWBACKEND_

namespace smooth
{
	namespace GUI
	{
		class WindowBackend;
	};
};

#include "../../../misc/string.h"
#include "../../../graphics/forms/point.h"
#include "../../../graphics/forms/size.h"
#include "../../../templates/callbacks.h"
#include "../../../graphics/surface.h"

namespace smooth
{
	namespace GUI
	{
		const Int	 WINDOW_NONE = 0;

		class WindowBackend
		{
			private:
				static Array<WindowBackend *(*)(), Void *>	*backend_creators;
			protected:
				Int						 type;

				Surface						*nullSurface;
				Surface						*drawSurface;
			public:
				static Int					 AddBackend(WindowBackend *(*)());

				static WindowBackend				*CreateBackendInstance();

										 WindowBackend(Void * = NIL);
				virtual						~WindowBackend();

				Int						 GetWindowType();

				virtual Void					*GetSystemWindow();

				virtual Surface					*GetDrawSurface();

				virtual Int					 Open(const String &, const Point &, const Size &, Int);
				virtual Int					 Close();

				virtual Int					 SetTitle(const String &);
				virtual Int					 SetIcon(const Bitmap &);

				virtual Int					 SetMinimumSize(const Size &);
				virtual Int					 SetMaximumSize(const Size &);

				virtual Int					 Show();
				virtual Int					 Hide();

				virtual Rect					 GetRestoredWindowRect();

				virtual Int					 SetMetrics(const Point &, const Size &);

				virtual Int					 Minimize();

				virtual Int					 Maximize();
				virtual Int					 Restore();
			signals:
				Callback3<Int, Int, Int, Int>			 onEvent;
		};
	};
};

#endif