 /* The smooth Class Library
  * Copyright (C) 1998-2003 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the "Artistic License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_SCROLLBAR_
#define _H_OBJSMOOTH_SCROLLBAR_

namespace smooth
{
	class Scrollbar;
	class Timer;
};

#include "object.h"

namespace smooth
{
	class SMOOTHAPI Scrollbar : public Object
	{
		private:
			Timer		*timer;
			Bool		 timerActive;
			Int		 timerCount;

			Void		 TimerProc();
		protected:
			Bool		 button1Clicked;
			Bool		 button2Clicked;
			Bool		 button3Clicked;
			Bool		 button1Checked;
			Bool		 button2Checked;
			Bool		 button3Checked;

			Int		*variable;

			Int		 startValue;
			Int 		 endValue;

			Int		 mouseBias;
		public:
					 Scrollbar(Point, Size, Int, Int *, Int, Int, ProcParam, Void *);
					~Scrollbar();

			virtual Int	 Paint(Int);
			Int		 Process(Int, Int, Int);

			Int		 SetRange(Int, Int);
	};

	SMOOTHVAR Int OBJ_SCROLLBAR;
};

#endif
