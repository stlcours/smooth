 /* The smooth Class Library
  * Copyright (C) 1998-2014 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/io/drivers/driver_ansi.h>

#include <smooth/io/instream.h>
#include <smooth/io/outstream.h>

#include <stdio.h>

#if defined __WIN32__
#	include <smooth/backends/win32/backendwin32.h>
#else
#	define _wfopen fopen
#endif

S::IO::DriverANSI::DriverANSI(const String &fileName, Int mode) : Driver()
{
	stream	    = NIL;
	closeStream = false;

	static Bool	 enableUnicode = Setup::enableUnicode;

#if defined __WIN32__
	/* Disable Unicode functions on Windows 9x even if we
	 * have Unicows as it does not work correctly there.
	 */
	enableUnicode = Backends::BackendWin32::IsWindowsVersionAtLeast(VER_PLATFORM_WIN32_NT);
#else
	/* Set output format to UTF-8 on non Windows systems.
	 */
	const char	*previousOutputFormat = String::SetOutputFormat("UTF-8");
#endif

	switch (mode)
	{
		default:
			lastError = IO_ERROR_BADPARAM;

			return;
		case OS_APPEND:		   // open a file for appending data
			if (enableUnicode) stream = _wfopen(fileName, String("r+b"));
			else		   stream =   fopen(fileName,	     "r+b" );

			if (stream != NIL)
			{
				Int64	 size = GetSize();

				if (size >= 0) Seek(size);
			}

			break;
		case OS_REPLACE:	   // create or overwrite a file
			if (enableUnicode) stream = _wfopen(fileName, String("w+b"));
			else		   stream =   fopen(fileName,	     "w+b" );

			break;
		case IS_READ | IS_WRITE:   // open a file for reading data
			if (enableUnicode) stream = _wfopen(fileName, String("r+b"));
			else		   stream =   fopen(fileName,	     "r+b" );

			break;
		case IS_READ:		   // open a file in read only mode
			if (enableUnicode) stream = _wfopen(fileName, String("rb"));
			else		   stream =   fopen(fileName,	     "rb" );

			break;
	}

#if !defined __WIN32__
	/* Restore original output format.
	 */
	String::SetOutputFormat(previousOutputFormat);
#endif

	/* Check if stream was opened successfully.
	 */
	if (stream == NIL)
	{
		lastError = IO_ERROR_UNEXPECTED;

		return;
	}

	streamID = fileName;

	closeStream = true;
}

S::IO::DriverANSI::DriverANSI(FILE *iStream) : Driver()
{
	stream		= iStream;
	closeStream	= false;
}

S::IO::DriverANSI::~DriverANSI()
{
	if (closeStream) fclose(stream);
}

S::Int S::IO::DriverANSI::ReadData(UnsignedByte *data, Int dataSize)
{
	if (dataSize <= 0) return 0;

	return fread((Void *) data, 1, (dataSize < (GetSize() - GetPos()) ? dataSize : (GetSize() - GetPos())), stream);
}

S::Int S::IO::DriverANSI::WriteData(UnsignedByte *data, Int dataSize)
{
	if (dataSize <= 0) return 0;

	return fwrite((Void *) data, 1, dataSize, stream);
}

S::Int64 S::IO::DriverANSI::Seek(Int64 newPos)
{
	if (fseek(stream, newPos, SEEK_SET) != 0) return -1;

	return GetPos();
}

S::Int64 S::IO::DriverANSI::GetSize() const
{
	Int64	 oldPos = GetPos();

	if (fseek(stream,      0, SEEK_END) != 0) return -1;

	Int64	 size = GetPos();

	if (fseek(stream, oldPos, SEEK_SET) != 0) return -1;

	return size;
}

S::Int64 S::IO::DriverANSI::GetPos() const
{
	return ftell(stream);
}
