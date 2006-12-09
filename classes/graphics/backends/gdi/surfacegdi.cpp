 /* The smooth Class Library
  * Copyright (C) 1998-2006 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/graphics/backends/gdi/surfacegdi.h>
#include <smooth/graphics/surface.h>
#include <smooth/graphics/bitmap.h>
#include <smooth/graphics/color.h>
#include <smooth/misc/math.h>

#include <fribidi.h>

S::GUI::SurfaceBackend *CreateSurfaceGDI(S::Void *iSurface, const S::GUI::Size &maxSize)
{
	return new S::GUI::SurfaceGDI(iSurface, maxSize);
}

S::Int	 surfaceGDITmp = S::GUI::SurfaceBackend::AddBackend(&CreateSurfaceGDI);

S::GUI::SurfaceGDI::SurfaceGDI(Void *iWindow, const Size &maxSize)
{
	type = SURFACE_GDI;

	window = (HWND) iWindow;

	if (window != NIL)
	{
		HDC	 gdi_dc = GetWindowDC(window);

		size = maxSize;

		if (maxSize == Size())
		{
			size.cx	= GetDeviceCaps(gdi_dc, HORZRES) + 2;
			size.cy	= GetDeviceCaps(gdi_dc, VERTRES) + 2;
		}

		HDC	 bmp_dc = CreateCompatibleDC(gdi_dc);

		HBITMAP	 bitmap = CreateCompatibleBitmap(gdi_dc, size.cx, size.cy);

		cDc_contexts.AddEntry(bmp_dc);
		cDc_bitmaps.AddEntry((HBITMAP) SelectObject(bmp_dc, bitmap));
		cDc_rects.AddEntry(new Rect(Point(0, 0), size));

		ReleaseDC(window, gdi_dc);

		allocSize = size;
	}
}

S::GUI::SurfaceGDI::~SurfaceGDI()
{
	if (window != NIL)
	{
		HBITMAP	 bitmap = (HBITMAP) SelectObject(cDc_contexts.GetFirstEntry(), cDc_bitmaps.GetFirstEntry());

		DeleteDC(cDc_contexts.GetFirstEntry());
		::DeleteObject(bitmap);

		delete cDc_rects.GetFirstEntry();
	}
}

S::Int S::GUI::SurfaceGDI::SetSize(const Size &nSize)
{
	size = nSize;

	if (allocSize.cx >= nSize.cx && allocSize.cy >= nSize.cy) return Success();

	if (window != NIL && !painting)
	{
		HBITMAP	 bitmap = (HBITMAP) SelectObject(cDc_contexts.GetFirstEntry(), cDc_bitmaps.GetFirstEntry());

		::DeleteObject(bitmap);

		delete cDc_rects.GetFirstEntry();

		cDc_bitmaps.RemoveAll();
		cDc_rects.RemoveAll();

		HDC	 gdi_dc = GetWindowDC(window);

		bitmap = CreateCompatibleBitmap(gdi_dc, size.cx, size.cy);

		cDc_bitmaps.AddEntry((HBITMAP) SelectObject(cDc_contexts.GetFirstEntry(), bitmap));
		cDc_rects.AddEntry(new Rect(Point(0, 0), size));

		ReleaseDC(window, gdi_dc);
	}

	allocSize = nSize;

	return Success();
}

const S::GUI::Size &S::GUI::SurfaceGDI::GetSize() const
{
	return size;
}

S::Int S::GUI::SurfaceGDI::PaintRect(const Rect &pRect)
{
	if (painting) return Error();

	if (window != NIL)
	{
		HDC	 gdi_dc = GetWindowDC(window);

		BitBlt(gdi_dc, pRect.left, pRect.top, pRect.right - pRect.left, pRect.bottom - pRect.top, cDc_contexts.GetFirstEntry(), pRect.left, pRect.top, SRCCOPY);

		ReleaseDC(window, gdi_dc);
	}

	return Success();
}

S::Int S::GUI::SurfaceGDI::StartPaint(const Rect &iPRect)
{
	if (window == NIL) return Success();

	Rect	 pRect = TranslateRect(iPRect);
	Rect	 oRect = *(cDc_rects.GetLastEntry());

	HDC	 bmp_dc = CreateCompatibleDC(cDc_contexts.GetFirstEntry());
	HBITMAP	 bitmap = CreateCompatibleBitmap(cDc_contexts.GetFirstEntry(), size.cx, size.cy);

	cDc_bitmaps.AddEntry((HBITMAP) SelectObject(bmp_dc, bitmap));

	if (pRect.left >= oRect.left && pRect.top >= oRect.top && pRect.right <= oRect.right && pRect.bottom <= oRect.bottom)	BitBlt(bmp_dc, pRect.left, pRect.top, pRect.right - pRect.left, pRect.bottom - pRect.top, cDc_contexts.GetLastEntry(), pRect.left, pRect.top, SRCCOPY);
	else															BitBlt(bmp_dc, pRect.left, pRect.top, pRect.right - pRect.left, pRect.bottom - pRect.top, cDc_contexts.GetFirstEntry(), pRect.left, pRect.top, SRCCOPY);

	cDc_contexts.AddEntry(bmp_dc);
	cDc_rects.AddEntry(new Rect(pRect));

	painting++;

	return Success();
}

S::Int S::GUI::SurfaceGDI::EndPaint()
{
	if (!painting) return Error();

	painting--;

	Rect	 iRect = Rect::OverlapRect(*(cDc_rects.GetLastEntry()), *(cDc_rects.GetNthEntry(cDc_rects.GetNOfEntries() - 2)));

	BitBlt(cDc_contexts.GetNthEntry(cDc_contexts.GetNOfEntries() - 2), iRect.left, iRect.top, iRect.right - iRect.left, iRect.bottom - iRect.top, cDc_contexts.GetLastEntry(), iRect.left, iRect.top, SRCCOPY);

	if (painting == 0) PaintRect(iRect);

	HBITMAP	 bitmap = (HBITMAP) SelectObject(cDc_contexts.GetLastEntry(), cDc_bitmaps.GetLastEntry());

	DeleteDC(cDc_contexts.GetLastEntry());
	::DeleteObject(bitmap);

	delete cDc_rects.GetLastEntry();

	cDc_contexts.RemoveEntry(cDc_contexts.GetNthEntryIndex(cDc_contexts.GetNOfEntries() - 1));
	cDc_bitmaps.RemoveEntry(cDc_bitmaps.GetNthEntryIndex(cDc_bitmaps.GetNOfEntries() - 1));
	cDc_rects.RemoveEntry(cDc_rects.GetNthEntryIndex(cDc_rects.GetNOfEntries() - 1));

	return Success();
}

S::Void *S::GUI::SurfaceGDI::GetSystemSurface() const
{
	return (Void *) window;
}

S::Int S::GUI::SurfaceGDI::SetPixel(Int x, Int y, Int color)
{
	if (window == NIL) return Success();

	if (!painting)
	{
		HDC	 gdi_dc = GetWindowDC(window);

		::SetPixel(gdi_dc, TranslateX(x), TranslateY(y), color);

		ReleaseDC(window, gdi_dc);
	}

	::SetPixel(cDc_contexts.GetLastEntry(), TranslateX(x), TranslateY(y), color);

	return Success();
}

S::Int S::GUI::SurfaceGDI::GetPixel(Int x, Int y) const
{
	if (window == NIL) return 0;

	HDC	 gdi_dc = GetWindowDC(window);
	Int	 pixel = ::GetPixel(gdi_dc, TranslateX(x), TranslateY(y));

	ReleaseDC(window, gdi_dc);

	return pixel;
}

S::Int S::GUI::SurfaceGDI::Line(const Point &iPos1, const Point &iPos2, Int color)
{
	if (window == NIL) return Success();

	Point	 pos1 = TranslatePoint(iPos1);
	Point	 pos2 = TranslatePoint(iPos2);

	HPEN	 hPen = CreatePen(PS_SOLID, 1, color);
	HPEN	 hOldPen = NIL;

	if (!painting)
	{
		HDC	 gdi_dc = GetWindowDC(window);

		hOldPen = (HPEN) SelectObject(gdi_dc, hPen);

		MoveToEx(gdi_dc, pos1.x, pos1.y, NIL);
		LineTo(gdi_dc, pos2.x, pos2.y);

		hPen = (HPEN) SelectObject(gdi_dc, hOldPen);

		ReleaseDC(window, gdi_dc);
	}

	hOldPen = (HPEN) SelectObject(cDc_contexts.GetLastEntry(), hPen);

	MoveToEx(cDc_contexts.GetLastEntry(), pos1.x, pos1.y, NIL);
	LineTo(cDc_contexts.GetLastEntry(), pos2.x, pos2.y);

	hPen = (HPEN) SelectObject(cDc_contexts.GetLastEntry(), hOldPen);

	::DeleteObject(hPen);

	return Success();
}

S::Int S::GUI::SurfaceGDI::Box(const Rect &iRect, Int color, Int style)
{
	if (window == NIL) return Success();

	Rect	 rect = TranslateRect(iRect);

	HDC	 gdi_dc = GetWindowDC(window);
	HBRUSH	 brush = CreateSolidBrush(color);
	RECT	 wRect = rect;

	if (style == FILLED)
	{
		if (!painting) FillRect(gdi_dc, &wRect, brush);
		FillRect(cDc_contexts.GetLastEntry(), &wRect, brush);
	}
	else if (style == OUTLINED)
	{
		if (!painting) FrameRect(gdi_dc, &wRect, brush);
		FrameRect(cDc_contexts.GetLastEntry(), &wRect, brush);
	}
	else if (style == INVERT)
	{
		if (!painting) InvertRect(gdi_dc, &wRect);
		InvertRect(cDc_contexts.GetLastEntry(), &wRect);
	}
	else if (style == OUTLINEDOTS)
	{
		Bool	 dot = False;
		Int	 x;
		Int	 y = rect.top;

		for (x = rect.left; x < rect.right - 1; x++)
		{
			if (dot == True)
			{
				if (!painting) ::SetPixel(gdi_dc, x, y, color);
				::SetPixel(cDc_contexts.GetLastEntry(), x, y, color);
				dot = False;
			}
			else dot = True;
		}

		x = rect.right - 1;

		for (y = rect.top; y < rect.bottom; y++)
		{
			if (dot == True)
			{
				if (!painting) ::SetPixel(gdi_dc, x, y, color);
				::SetPixel(cDc_contexts.GetLastEntry(), x, y, color);
				dot = False;
			}
			else dot = True;
		}

		y = rect.bottom - 1;

		for (x = rect.right - 2; x >= rect.left; x--)
		{
			if (dot == True)
			{
				if (!painting) ::SetPixel(gdi_dc, x, y, color);
				::SetPixel(cDc_contexts.GetLastEntry(), x, y, color);
				dot = False;
			}
			else dot = True;
		}

		x = rect.left;

		for (y = rect.bottom - 2; y >= rect.top; y--)
		{
			if (dot == True)
			{
				if (!painting) ::SetPixel(gdi_dc, x, y, color);
				::SetPixel(cDc_contexts.GetLastEntry(), x, y, color);
				dot = False;
			}
			else dot = True;
		}
	}

	::DeleteObject(brush);

	ReleaseDC(window, gdi_dc);

	return Success();
}

S::Int S::GUI::SurfaceGDI::SetText(const String &string, const Rect &iRect, const Font &font, Bool shadow)
{
	if (window == NIL)	return Success();

	if (string == NIL)	return Error();
	if (shadow)		return SurfaceBackend::SetText(string, iRect, font, shadow);

	HDC	 gdi_dc = GetWindowDC(window);
	HFONT	 hfont;
	HFONT	 holdfont = NIL;
	HFONT	 holdfont2 = NIL;
	int	 lines = 1;
	int	 offset = 0;
	int	 origoffset;
	int	 height = font.GetLineSizeY(string) + 3;
	int	 txtsize = string.Length();
	String	 line;
	Rect	 rect = iRect;

	for (Int j = 0; j < txtsize; j++) if (string[j] == 10) lines++;

	if (Setup::enableUnicode)	hfont = CreateFontW(-Math::Round(font.GetSize() * 128.0 / GetDeviceCaps(gdi_dc, LOGPIXELSY)), 0, 0, 0, (font.GetWeight() == Font::Bold) ? FW_BOLD : FW_NORMAL, font.GetStyle() & Font::Italic, font.GetStyle() & Font::Underline, font.GetStyle() & Font::StrikeOut, ANSI_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, FF_ROMAN, font.GetName());
	else				hfont = CreateFontA(-Math::Round(font.GetSize() * 128.0 / GetDeviceCaps(gdi_dc, LOGPIXELSY)), 0, 0, 0, (font.GetWeight() == Font::Bold) ? FW_BOLD : FW_NORMAL, font.GetStyle() & Font::Italic, font.GetStyle() & Font::Underline, font.GetStyle() & Font::StrikeOut, ANSI_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, FF_ROMAN, font.GetName());

	if (!painting)
	{
		SetBkMode(gdi_dc, TRANSPARENT);
		SetTextColor(gdi_dc, font.GetColor());

		holdfont = (HFONT) SelectObject(gdi_dc, hfont);
	}

	SetBkMode(cDc_contexts.GetLastEntry(), TRANSPARENT);
	SetTextColor(cDc_contexts.GetLastEntry(), font.GetColor());

	holdfont2 = (HFONT) SelectObject(cDc_contexts.GetLastEntry(), hfont);

	for (Int i = 0; i < lines; i++)
	{
		Bool	 rtlCharacters = False;

		origoffset = offset;

		for (Int j = 0; j <= txtsize; j++)
		{
			if (j + origoffset == txtsize)
			{
				line[j] = 0;
				break;
			}

			if (string[j + origoffset] == 10 || string[j + origoffset] == 0)
			{
				offset++;
				line[j] = 0;
				break;
			}
			else
			{
				offset++;
				line[j] = string[j + origoffset];

				if (line[j] >= 0x0590 && line[j] <= 0x07BF) rtlCharacters = True;
			}
		}

		RECT	 Rect = TranslateRect(rect);

		if (rightToLeft) Rect.right--;

		if (rtlCharacters && Setup::useIconv)
		{
			/*	Reorder the string with fribidi, then get
				the glyph indices using GetCharacterPlacement
				and display using the glyph indices.
				This does not work with Kanji.	*/

			FriBidiParType	*visual = new FriBidiParType [line.Length() + 1];
			FriBidiParType	 type = FRIBIDI_TYPE_ON;

			fribidi_log2vis((FriBidiChar *) line.ConvertTo("UCS-4LE"), line.Length(), &type, (FriBidiChar *) visual, NIL, NIL, NIL);

			line.ImportFrom("UCS-4LE", (char *) visual);

			delete [] visual;

			GCP_RESULTSA	 resultsa;
			GCP_RESULTSW	 resultsw;
			wchar_t		*glyphs = new wchar_t [line.Length() + 1];

			ZeroMemory(&resultsa, sizeof(resultsa));
			ZeroMemory(&resultsw, sizeof(resultsw));

			resultsa.lStructSize = sizeof(resultsa);
			resultsa.lpGlyphs = glyphs;
			resultsa.nGlyphs = line.Length() + 1;

			resultsw.lStructSize = sizeof(resultsw);
			resultsw.lpGlyphs = glyphs;
			resultsw.nGlyphs = line.Length() + 1;

			ZeroMemory(glyphs, 2 * (line.Length() + 1));

			if (!painting)
			{
				if (rightToLeft)	SetTextAlign(gdi_dc, TA_RIGHT);
				else			SetTextAlign(gdi_dc, TA_LEFT);
			}

			if (rightToLeft)	SetTextAlign(cDc_contexts.GetLastEntry(), TA_RIGHT);
			else			SetTextAlign(cDc_contexts.GetLastEntry(), TA_LEFT);

			if (Setup::enableUnicode)	GetCharacterPlacementW(cDc_contexts.GetLastEntry(), line, line.Length(), 0, &resultsw, 0);
			else				GetCharacterPlacementA(cDc_contexts.GetLastEntry(), line, line.Length(), 0, &resultsa, 0);

			if (rightToLeft) Rect.left -= 10;
			else		 Rect.right += 10;

			if (!painting)
			{
				if (Setup::enableUnicode)	ExtTextOutW(gdi_dc, (rightToLeft ? Rect.right : Rect.left), Rect.top, ETO_CLIPPED | ETO_GLYPH_INDEX, &Rect, resultsw.lpGlyphs, resultsw.nGlyphs, NIL);
				else				ExtTextOutA(gdi_dc, (rightToLeft ? Rect.right : Rect.left), Rect.top, ETO_CLIPPED | ETO_GLYPH_INDEX, &Rect, (char *) resultsa.lpGlyphs, resultsa.nGlyphs, NIL);
			}

			if (Setup::enableUnicode)	ExtTextOutW(cDc_contexts.GetLastEntry(), (rightToLeft ? Rect.right : Rect.left), Rect.top, ETO_CLIPPED | ETO_GLYPH_INDEX, &Rect, resultsw.lpGlyphs, resultsw.nGlyphs, NIL);
			else				ExtTextOutA(cDc_contexts.GetLastEntry(), (rightToLeft ? Rect.right : Rect.left), Rect.top, ETO_CLIPPED | ETO_GLYPH_INDEX, &Rect, (char *) resultsa.lpGlyphs, resultsa.nGlyphs, NIL);

			delete [] glyphs;
		}
		else
		{
			/*	Let Windows do any reordering and ligating.
				Works with Kanji, but RTL is only supported
				on XP and later versions of Windows.	*/

			if (!painting)
			{
				SetTextAlign(gdi_dc, TA_LEFT);

				if (Setup::enableUnicode)	DrawTextExW(gdi_dc, line, -1, &Rect, DT_EXPANDTABS | DT_NOPREFIX | (rightToLeft ? DT_RIGHT : DT_LEFT) | (rtlCharacters ? DT_RTLREADING : 0), NIL);
				else				DrawTextExA(gdi_dc, line, -1, &Rect, DT_EXPANDTABS | DT_NOPREFIX | (rightToLeft ? DT_RIGHT : DT_LEFT) | (rtlCharacters ? DT_RTLREADING : 0), NIL);
			}

			SetTextAlign(cDc_contexts.GetLastEntry(), TA_LEFT);

			if (Setup::enableUnicode)	DrawTextExW(cDc_contexts.GetLastEntry(), line, -1, &Rect, DT_EXPANDTABS | DT_NOPREFIX | (rightToLeft ? DT_RIGHT : DT_LEFT) | (rtlCharacters ? DT_RTLREADING : 0), NIL);
			else				DrawTextExA(cDc_contexts.GetLastEntry(), line, -1, &Rect, DT_EXPANDTABS | DT_NOPREFIX | (rightToLeft ? DT_RIGHT : DT_LEFT) | (rtlCharacters ? DT_RTLREADING : 0), NIL);
		}

		rect.top += height;
	}

	if (!painting) SelectObject(gdi_dc, holdfont);
	SelectObject(cDc_contexts.GetLastEntry(), holdfont2);

	::DeleteObject(hfont);

	ReleaseDC(window, gdi_dc);

	return Success();
}

S::Int S::GUI::SurfaceGDI::BlitFromBitmap(const Bitmap &oBitmap, const Rect &srcRect, const Rect &iDestRect)
{
	if (window == NIL) return Success();

	Bitmap	 bitmap	  = oBitmap;
	Rect	 destRect = TranslateRect(iDestRect);
	HDC	 gdi_dc	  = GetWindowDC(window);
	HDC	 cdc	  = CreateCompatibleDC(gdi_dc);
	HBITMAP	 backup	  = (HBITMAP) SelectObject(cdc, bitmap.GetSystemBitmap());

	if ((destRect.right - destRect.left == srcRect.right - srcRect.left) && (destRect.bottom - destRect.top == srcRect.bottom - srcRect.top))
	{
		if (!painting) BitBlt(gdi_dc, destRect.left, destRect.top, destRect.right - destRect.left, destRect.bottom - destRect.top, cdc, srcRect.left, srcRect.top, SRCCOPY);
		BitBlt(cDc_contexts.GetLastEntry(), destRect.left, destRect.top, destRect.right - destRect.left, destRect.bottom - destRect.top, cdc, srcRect.left, srcRect.top, SRCCOPY);
	}
	else
	{
		if (!painting) StretchBlt(gdi_dc, destRect.left, destRect.top, destRect.right - destRect.left, destRect.bottom - destRect.top, cdc, srcRect.left, srcRect.top, srcRect.right - srcRect.left, srcRect.bottom - srcRect.top, SRCCOPY);
		StretchBlt(cDc_contexts.GetLastEntry(), destRect.left, destRect.top, destRect.right - destRect.left, destRect.bottom - destRect.top, cdc, srcRect.left, srcRect.top, srcRect.right - srcRect.left, srcRect.bottom - srcRect.top, SRCCOPY);
	}

	SelectObject(cdc, backup);

	DeleteDC(cdc);
	ReleaseDC(window, gdi_dc);

	return Success();
}

S::Int S::GUI::SurfaceGDI::BlitToBitmap(const Rect &iSrcRect, const Bitmap &oBitmap, const Rect &destRect)
{
	if (window == NIL) return Success();

	Bitmap	 bitmap	 = oBitmap;
	Rect	 srcRect = TranslateRect(iSrcRect);
	HDC	 gdi_dc	 = GetWindowDC(window);
	HDC	 cdc	 = CreateCompatibleDC(gdi_dc);
	HBITMAP	 backup	 = (HBITMAP) SelectObject(cdc, bitmap.GetSystemBitmap());

	if ((destRect.right - destRect.left == srcRect.right - srcRect.left) && (destRect.bottom - destRect.top == srcRect.bottom - srcRect.top))
	{
		BitBlt(cdc, destRect.left, destRect.top, destRect.right - destRect.left, destRect.bottom - destRect.top, gdi_dc, srcRect.left, srcRect.top, SRCCOPY);
	}
	else
	{
		StretchBlt(cdc, destRect.left, destRect.top, destRect.right - destRect.left, destRect.bottom - destRect.top, gdi_dc, srcRect.left, srcRect.top, srcRect.right - srcRect.left, srcRect.bottom - srcRect.top, SRCCOPY);
	}

	SelectObject(cdc, backup);

	DeleteDC(cdc);
	ReleaseDC(window, gdi_dc);

	return Success();
}
