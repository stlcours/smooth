 /* The smooth Class Library
  * Copyright (C) 1998-2003 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the "Artistic License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_MENUBAR_
#define _H_OBJSMOOTH_MENUBAR_

namespace smooth
{
	class Menubar;
};

#include "object.h"
#include "menu.h"

namespace smooth
{
	const Int	 MB_GRAYSCALE	= 0;
	const Int	 MB_COLOR	= 1;

	class SMOOTHAPI Menubar : public Object, public Menu
	{
		private:
			Int		 popupHandle;
			Int		 style;

			String		 backupStatusText;
		public:
					 Menubar();
					~Menubar();

			virtual Int	 Paint(Int);
			Int		 Process(Int, Int, Int);

			Int		 SetStyle(Int);
	};

	SMOOTHVAR Int OBJ_MENUBAR;
};

#endif
