 /* The smooth Class Library
  * Copyright (C) 1998-2003 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the "Artistic License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_CHECKBOX_
#define _H_OBJSMOOTH_CHECKBOX_

namespace smooth
{
	class CheckBox;
};

#include "object.h"

namespace smooth
{
	class SMOOTHAPI CheckBox : public Object
	{
		protected:
			Bool		*variable;
			Bool		 state;
		public:
					 CheckBox(String, Point, Size, Bool *, ProcParam, Void *);
					~CheckBox();

			virtual Int	 Paint(Int);
			Int		 Process(Int, Int, Int);
	};

	SMOOTHVAR Int OBJ_CHECKBOX;
};

#endif
