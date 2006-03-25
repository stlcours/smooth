 /* The smooth Class Library
  * Copyright (C) 1998-2006 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "../definitions.h"

#ifndef _H_OBJSMOOTH_ERROR_
#define _H_OBJSMOOTH_ERROR_

namespace smooth
{
	namespace Errors
	{
		class Error;
	};
};

#include "../misc/string.h"

namespace smooth
{
	namespace Errors
	{
		class SMOOTHAPI Error
		{
			protected:
				Int			 code;
			public:
							 Error()			{ code = -1; }
							 Error(const Error &oError)	{ code = oError.code; }

				virtual			~Error()			{ }

				virtual String		 ToString();

				operator		 Int() const			{ return code; }

				virtual Bool operator	 ==(const Error &) const;
				virtual Bool operator	 !=(const Error &) const;
		};
	};
};

#endif
