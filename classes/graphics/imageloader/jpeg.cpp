 /* The smooth Class Library
  * Copyright (C) 1998-2016 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/graphics/imageloader/jpeg.h>
#include <smooth/system/system.h>
#include <smooth/io/instream.h>
#include <smooth/files/file.h>

#include <stdio.h>

extern "C"
{
#	include <jpeglib.h>
#	include <jerror.h>
}

using namespace smooth::IO;

S::GUI::ImageLoaderJPEG::ImageLoaderJPEG(const String &iFileName) : ImageLoader(iFileName)
{
}

S::GUI::ImageLoaderJPEG::ImageLoaderJPEG(const Buffer<UnsignedByte> &iBuffer) : ImageLoader(iBuffer)
{
}

S::GUI::ImageLoaderJPEG::~ImageLoaderJPEG()
{
}

void my_error_exit(j_common_ptr cinfo)
{
}

const S::GUI::Bitmap &S::GUI::ImageLoaderJPEG::Load()
{
	/* Make sure bitmap is initialized.
	 */
	bitmap = NIL;

	/* Check magic number.
	 */
	if	( gotBuffer && (buffer[0] != 0xFF || buffer[1] != 0xD8))			     return bitmap;
	else if (!gotBuffer && InStream(STREAM_FILE, fileName, IS_READ).InputNumberRaw(2) != 0xFFD8) return bitmap;

	/* Open stream.
	 */
	FILE	 *stream = NIL;

	if (!gotBuffer)
	{
#ifdef __WIN32__
		stream = _wfopen(fileName, L"rb");
#else
		stream = fopen(fileName.ConvertTo("UTF-8"), "rb");
#endif

		if (stream == NIL) return bitmap;
	}

	/* This struct contains the JPEG decompression parameters and pointers to
	 * working space (which is allocated as needed by the JPEG library).
	 */
	jpeg_decompress_struct	 cinfo;

	/* We set up the normal JPEG error routines, then override error_exit.
	 */
	jpeg_error_mgr	 jerr;

	cinfo.err = jpeg_std_error(&jerr);
	cinfo.err->error_exit = my_error_exit;

	/* Now we can initialize the JPEG decompression object.
	 */
	jpeg_create_decompress(&cinfo);

	/* Specify data source (eg, a file)
	 */
	if (gotBuffer) jpeg_mem_src(&cinfo, buffer, buffer.Size());
	else	       jpeg_stdio_src(&cinfo, stream);

	/* Read file parameters with jpeg_read_header()
	 */
	if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK)
	{
		jpeg_destroy_decompress(&cinfo);

		if (!gotBuffer) fclose(stream);

		return bitmap;
	}

	/* We can ignore the return value from jpeg_read_header since
	 *   (a) suspension is not possible with the stdio data source, and
	 *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
	 * See libjpeg.doc for more info.
	 */

	/* Start decompressor
	 */
	jpeg_start_decompress(&cinfo);

	/* We can ignore the return value since suspension is not possible
	 * with the stdio data source.
	 */

	/* We may need to do some setup of our own at this point before reading
	 * the data.  After jpeg_start_decompress() we have the correct scaled
	 * output image dimensions available, as well as the output colormap
	 * if we asked for color quantization.
	 * In this example, we need to make an output work buffer of the right size.
	 */

	/* JSAMPLEs per row in output buffer
	 */
	int		 row_stride = cinfo.output_width * cinfo.output_components;

	/* Make a one-row-high sample array that will go away when done with image
	 */
	JSAMPARRAY	 buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	bitmap.CreateBitmap(Size(cinfo.output_width, cinfo.output_height));

	/* Here we use the library's state variable cinfo.output_scanline as the
	 * loop counter, so that we don't have to keep track ourselves.
	 */
	while (cinfo.output_scanline < cinfo.output_height)
	{
		/* jpeg_read_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could ask for
		 * more than one scanline at a time if that's more convenient.
		 */
		jpeg_read_scanlines(&cinfo, buffer, 1);

		/* TODO: The copying step needs lots of optimization
		 */
		for (Int x = 0; x < (signed) cinfo.output_width; x++)
		{
			bitmap.SetPixel(Point(x, cinfo.output_scanline - 1), Color(buffer[0][3 * x], buffer[0][3 * x + 1], buffer[0][3 * x + 2], Color::RGB));
		}
	}

	/* We can ignore the return value since suspension is not possible
	 * with the stdio data source.
	 */
	jpeg_finish_decompress(&cinfo);

	/* This is an important step since it will release a good deal of memory.
	 */
	jpeg_destroy_decompress(&cinfo);

	/* After finish_decompress, we can close the input file.
	 * Here we postpone it until after no more JPEG errors are possible,
	 * so as to simplify the setjmp error logic above.  (Actually, I don't
	 * think that jpeg_destroy can do an error exit, but why assume anything...)
	 */
	if (!gotBuffer) fclose(stream);

	/* At this point you may want to check to see whether any corrupt-data
	 * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
	 */

	/* And we're done!
	 */
	return bitmap;
}
