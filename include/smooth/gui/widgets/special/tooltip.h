 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_TOOLTIP_
#define _H_OBJSMOOTH_TOOLTIP_

namespace smooth
{
	namespace GUI
	{
		class Tooltip;
		class ToolWindow;
	};

	namespace System
	{
		class Timer;
	};
};

#include "../widget.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Tooltip : public Widget
		{
			private:
				ToolWindow		*toolWindow;
				System::Timer		*timer;
			protected:
				Int			 timeOut;
			public:
				static const Int	 classID;

							 Tooltip();
				virtual			~Tooltip();

				virtual Int		 Show();
				virtual Int		 Hide();
			accessors:
				Int			 SetTimeout(Int);
			slots:
				Void			 OnToolWindowPaint();

				Void			 OnTimer();
		};
	};
};

#endif
