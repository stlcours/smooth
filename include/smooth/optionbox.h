 /* The smooth Class Library
  * Copyright (C) 1998-2003 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the "Artistic License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_OPTIONBOX_
#define _H_OBJSMOOTH_OPTIONBOX_

namespace smooth
{
	class OptionBox;
};

#include "object.h"

namespace smooth
{
	class SMOOTHAPI OptionBox : public Object
	{
		protected:
			Int		*variable;
			Int		 code;
			Bool		 state;
		public:
					 OptionBox(String, Point, Size, Int *, Int);
					~OptionBox();

			virtual Int	 Paint(Int);
			Int		 Process(Int, Int, Int);

			Int		 SetText(String);
	};

	SMOOTHVAR Int OBJ_OPTIONBOX;
};

#endif