 /* The smooth Class Library
  * Copyright (C) 1998-2019 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/gui/widgets/basic/editbox.h>
#include <smooth/gui/widgets/special/cursor.h>
#include <smooth/gui/widgets/multi/list/combobox.h>
#include <smooth/misc/binary.h>
#include <smooth/misc/math.h>
#include <smooth/graphics/surface.h>
#include <smooth/gui/window/window.h>

const S::Short	 S::GUI::EditBox::classID = S::Object::RequestClassID();

S::GUI::EditBox::EditBox(const Point &iPos, const Size &iSize, Int maxSize) : Widget(iPos, iSize)
{
	type		= classID;

	dropDownList	= NIL;
	comboBox	= NIL;

	font.SetColor(Setup::ClientTextColor);

	if (GetWidth()	== 0) SetWidth(80);
	if (GetHeight() == 0) SetHeight(19);

	cursor = new Cursor(Point(3, Math::Ceil(Float(GetHeight() - font.GetUnscaledTextSizeY()) / 2) - 2),
			    GetSize() - Size(6, Math::Ceil(Float(GetHeight() - font.GetUnscaledTextSizeY()) / 2) - 1));
	cursor->SetMaxSize(maxSize);
	cursor->SetBackgroundColor(Setup::ClientColor);
	cursor->SetFont(font);
	cursor->onInput.Connect(&onInput);
	cursor->onEnter.Connect(&onEnter);

	Add(cursor);

	onInput.SetParentObject(this);
	onEnter.SetParentObject(this);

	onChangeSize.Connect(&EditBox::OnChangeSize, this);
	onLoseFocus.Connect(&cursor->onLoseFocus);
}

S::GUI::EditBox::EditBox(const String &iText, const Point &iPos, const Size &iSize, Int maxSize) : Widget(iPos, iSize)
{
	type		= classID;

	dropDownList	= NIL;
	comboBox	= NIL;

	font.SetColor(Setup::ClientTextColor);

	if (GetWidth()	== 0) SetWidth(80);
	if (GetHeight() == 0) SetHeight(19);

	cursor = new Cursor(Point(3, Math::Ceil(Float(GetHeight() - font.GetUnscaledTextSizeY()) / 2) - 2),
			    GetSize() - Size(6, Math::Ceil(Float(GetHeight() - font.GetUnscaledTextSizeY()) / 2) - 1));
	cursor->SetMaxSize(maxSize);
	cursor->SetBackgroundColor(Setup::ClientColor);
	cursor->SetFont(font);
	cursor->SetText(iText);
	cursor->onInput.Connect(&onInput);
	cursor->onEnter.Connect(&onEnter);

	Add(cursor);

	onInput.SetParentObject(this);
	onEnter.SetParentObject(this);

	onChangeSize.Connect(&EditBox::OnChangeSize, this);
	onLoseFocus.Connect(&cursor->onLoseFocus);
}

S::GUI::EditBox::~EditBox()
{
	DeleteObject(cursor);

	if (comboBox != NIL) DeleteObject(comboBox);
}

S::Int S::GUI::EditBox::Paint(Int message)
{
	if (!IsRegistered()) return Error();
	if (!IsVisible())    return Success();

	switch (message)
	{
		case SP_PAINT:
		{
			Surface	*surface	 = GetDrawSurface();
			Rect	 frame		 = Rect(GetRealPosition(), GetRealSize());
			Color	 backgroundColor = IsActive() ? Setup::ClientColor : Setup::BackgroundColor;

			surface->StartPaint(GetVisibleArea());

			cursor->SetBackgroundColor(backgroundColor);

			surface->Box(frame, backgroundColor, Rect::Filled);
			surface->Frame(frame, FRAME_DOWN);

			Widget::Paint(message);

			surface->EndPaint();

			return Success();
		}
	}

	return Widget::Paint(message);
}

S::Int S::GUI::EditBox::SetDropDownList(List *nDropDownList)
{
	Surface	*surface = GetDrawSurface();

	surface->StartPaint(GetVisibleArea());

	if (comboBox != NIL) DeleteObject(comboBox);

	dropDownList = nDropDownList;

	if (dropDownList != NIL)
	{
		comboBox = new ComboBox(Point(), Size());
		comboBox->SetFlags(CB_HOTSPOTONLY);
		comboBox->onSelectEntry.Connect(&EditBox::OnSelectEntry, this);

		for (Int i = 0; i < dropDownList->Length(); i++)
		{
			if (dropDownList->GetNthEntry(i)->GetObjectType() == ListEntrySeparator::classID) comboBox->AddSeparator();
			else										  comboBox->AddEntry(dropDownList->GetNthEntry(i)->GetText());
		}

		Add(comboBox);

		OnChangeSize(GetSize());
	}

	Paint(SP_PAINT);

	surface->EndPaint();

	return Success();
}

S::Void S::GUI::EditBox::OnSelectEntry(ListEntry *entry)
{
	cursor->SetText(entry->GetText());
	cursor->MarkAll();

	onSelectEntry.Emit(entry);
	onInput.Emit(cursor->GetText());
}

S::Void S::GUI::EditBox::OnChangeSize(const Size &nSize)
{
	cursor->SetMetrics(Point(3, Math::Ceil(Float(nSize.cy - font.GetUnscaledTextSizeY()) / 2) - 2),
			   nSize - Size(6 + (comboBox != NIL ? 17 : 0), Math::Ceil(Float(nSize.cy - font.GetUnscaledTextSizeY()) / 2) - 1));

	if (comboBox != NIL) comboBox->SetSize(nSize);
}
