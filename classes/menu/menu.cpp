 /* The smooth Class Library
  * Copyright (C) 1998-2003 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the "Artistic License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/menu.h>
#include <smooth/definitions.h>
#include <smooth/array.h>
#include <smooth/toolkit.h>
#include <smooth/stk.h>
#include <smooth/system.h>
#include <smooth/metrics.h>
#include <smooth/i18n.h>

S::Menu::Menu()
{
	nOfEntries = 0;

	sizeset = False;
	entrysizesset = False;
}

S::Menu::~Menu()
{
	Int	 maxCount = nOfEntries;

	for (Int i = 0; i < maxCount; i++)
	{
		RemoveEntry(entries.GetFirstEntry());
	}
}

S::Menu::Entry *S::Menu::AddEntry(String text, HBITMAP bitmap, Menu *popupMenu, Bool *bVar, Int *iVar, Int iCode, Int orientation)
{
	Int	 id	= System::RequestGUID();
	Int	 type	= SM_SEPARATOR;

	if (text != NIL)	type = type | SM_TEXT;
	if (bitmap != NIL)	type = type | SM_BITMAP;

	Menu::Entry	*newEntry = new Menu::Entry(type, id);

	newEntry->text		= text;
	newEntry->orientation	= orientation;
	newEntry->bitmap	= DetectTransparentRegions(bitmap);
	newEntry->graymap	= DetectTransparentRegions(GrayscaleBitmap(bitmap));
	newEntry->popup		= popupMenu;
	newEntry->bVar		= bVar;
	newEntry->iVar		= iVar;
	newEntry->iCode		= iCode;
	newEntry->sizeset	= False;

	if (entries.AddEntry(newEntry, id) == True)
	{
		nOfEntries++;

		sizeset		= False;
		entrysizesset	= False;

		return newEntry;
	}
	else
	{
		delete newEntry;

		return NIL;
	}
}

S::Int S::Menu::RemoveEntry(Menu::Entry *entry)
{
	if (entry == NIL) return Error;

	if (entries.GetEntry(entry->id) != NIL)
	{
		entries.RemoveEntry(entry->id);

		delete entry;

		nOfEntries--;

		return Success;
	}

	return Error;
}

S::Int S::Menu::RemoveEntry(Int id)
{
	return RemoveEntry(entries.GetEntry(id));
}

S::Int S::Menu::Clear()
{
	Int	 maxCount = nOfEntries;

	for (Int i = 0; i < maxCount; i++)
	{
		RemoveEntry(entries.GetFirstEntry());
	}

	return Success;
}

S::Void S::Menu::GetSize()
{
	if (!entrysizesset)
	{
		GetMenuEntriesSize();

		entrysizesset = True;
	}
	if (!sizeset)
	{
		popupsize.cx = GetSizeX();
		popupsize.cy = GetSizeY();

		sizeset = True;
	}
}

S::Int S::Menu::GetSizeX()
{
	Int	 mSize = 50;
	Int	 greatest = 0;

	if (nOfEntries == 0) return mSize;

	for (Int i = 0; i < nOfEntries; i++)
	{
		Menu::Entry	*entry = entries.GetNthEntry(i);

		if (entry->size > greatest)
		{
			mSize		= 50 + entry->size;
			greatest	= entry->size;
		}
	}

	return mSize;
}

S::Int S::Menu::GetSizeY()
{
	Int	 mSize = 4;

	if (nOfEntries == 0) return mSize;

	for (Int i = 0; i < nOfEntries; i++)
	{
		Menu::Entry	*entry = entries.GetNthEntry(i);

		if (entry->type == SM_SEPARATOR)	mSize = mSize + 5;
		else					mSize = mSize + METRIC_POPUPENTRYSIZE;
	}

	return mSize;
}

S::Void S::Menu::GetMenuEntriesSize()
{
	if (nOfEntries == 0) return;

	for (Int i = 0; i < nOfEntries; i++)
	{
		Menu::Entry	*operat = entries.GetNthEntry(i);

		if (!operat->sizeset) operat->size = GetTextSizeX(operat->text, I18N_DEFAULTFONT, I18N_SMALLFONTSIZE, FW_NORMAL);

		operat->sizeset = True;
	}
}

S::Int S::Menu::GetNOfEntries()
{
	return nOfEntries;
}
