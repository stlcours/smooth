 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_BITMAPXLIB
#define H_OBJSMOOTH_BITMAPXLIB

namespace smooth
{
	namespace GUI
	{
		class BitmapXLib;
	};
};

#include "../bitmapbackend.h"

namespace X11
{
	extern "C"
	{
#		include <X11/Xlib.h>
#		include <X11/Xutil.h>

#		undef True
#		undef False
#		undef Bool
#		undef Success
	}
};

namespace smooth
{
	namespace GUI
	{
		const Short	 BITMAP_XLIB = 2;

		class BitmapXLib : public BitmapBackend
		{
			private:
				X11::Display		*display;
				X11::Pixmap		 bitmap;

				Void			 Initialize();
			public:
							 BitmapXLib(Void * = NIL);
							 BitmapXLib(Int, Int, Int);
							 BitmapXLib(const int);
							 BitmapXLib(const BitmapXLib &);

							~BitmapXLib();

				Bool			 CreateBitmap(Int, Int, Int);
				Bool			 DeleteBitmap();

				Bool			 SetSystemBitmap(Void *);
				Void			*GetSystemBitmap() const;

				Bool			 SetPixel(const Point &, const Color &);
				Color			 GetPixel(const Point &) const;

				BitmapBackend &operator	 =(const BitmapBackend &);

				Bool operator		 ==(const int) const;
				Bool operator		 !=(const int) const;
		};
	};
};

#endif
