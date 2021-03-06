 /* The smooth Class Library
  * Copyright (C) 1998-2014 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_BITMAPGDI
#define H_OBJSMOOTH_BITMAPGDI

#include <windows.h>

namespace smooth
{
	namespace GUI
	{
		class BitmapGDI;
	};
};

#include "../bitmapbackend.h"

namespace smooth
{
	namespace GUI
	{
		const Short	 BITMAP_GDI = 1;

		class BitmapGDI : public BitmapBackend
		{
			private:
				HBITMAP			 bitmap;
			public:
							 BitmapGDI(Void * = NIL);
							 BitmapGDI(const Size &, Int);
							 BitmapGDI(const int);
							 BitmapGDI(const BitmapGDI &);

							~BitmapGDI();

				Bool			 CreateBitmap(const Size &, Int);
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
