 /* The smooth Class Library
  * Copyright (C) 1998-2012 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_SURFACE
#define H_OBJSMOOTH_SURFACE

namespace smooth
{
	namespace GUI
	{
		class Surface;
		class SurfaceBackend;
	};
};

#include "../definitions.h"
#include "../misc/string.h"
#include "forms/rect.h"
#include "font.h"
#include "bitmap.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Surface
		{
			private:
				static Surface		*nullSurface;

				SurfaceBackend		*backend;
			public:
				static Int		 Initialize();
				static Int		 Free();

							 Surface(Void * = NIL, const Size & = Size());
							~Surface();

				static Surface		*GetNullSurface();

				Short			 GetSurfaceType() const;

				Int			 SetSize(const Size &);
				const Size		&GetSize() const;

				Int			 SetRightToLeft(Bool);

				Int			 PaintRect(const Rect &);

				Int			 StartPaint(const Rect &);
				Int			 EndPaint();

				Void			*GetSystemSurface() const;

				Short			 GetSurfaceDPI() const;

				Int			 SetPixel(const Point &, const Color &);

				Int			 Line(const Point &, const Point &, const Color &);
				Int			 Frame(const Rect &, Short);
				Int			 Box(const Rect &, const Color &, Int, const Size & = Size(0, 0));

				Int			 SetText(const String &, const Rect &, const Font &, Bool = False);

				Int			 Gradient(const Rect &, const Color &, const Color &, Int);
				Int			 Bar(const Point &, const Point &, Int);

				Int			 BlitFromBitmap(const Bitmap &, const Rect &, const Rect &);
				Int			 BlitToBitmap(const Rect &, Bitmap &, const Rect &);
		};

		const Short	 FRAME_UP	= 0;
		const Short	 FRAME_DOWN	= 1;
	};
};

#endif
