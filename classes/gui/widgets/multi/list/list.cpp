 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/gui/widgets/multi/list/list.h>
#include <smooth/graphics/surface.h>
#include <smooth/misc/math.h>

const S::Int	 S::GUI::List::classID = S::Object::RequestClassID();

S::GUI::List::List() : Widget(Point(), Size())
{
	type	= classID;

	onSelectEntry.SetParentObject(this);
	onMarkEntry.SetParentObject(this);

	onChangeEntryOrder.SetParentObject(this);
}

S::GUI::List::~List()
{
	RemoveAllEntries();
}

S::GUI::ListEntry *S::GUI::List::AddEntry(const String &text)
{
	ListEntry	*newEntry = new ListEntry(text);

	if (Add(newEntry) == Success())
	{
		createdEntry.Set(newEntry->GetHandle(), True);

		return newEntry;
	}

	DeleteObject(newEntry);

	return NIL;
}

S::GUI::ListEntrySeparator *S::GUI::List::AddSeparator()
{
	ListEntrySeparator	*newEntry = new ListEntrySeparator();

	if (Add(newEntry) == Success())
	{
		createdEntry.Set(newEntry->GetHandle(), True);

		return newEntry;
	}

	DeleteObject(newEntry);

	return NIL;
}

S::Int S::GUI::List::Add(Widget *widget)
{
	if (widget == NIL) return Error();

	if (widget->GetObjectType() == ListEntry::classID) widget->Hide();

	if (Widget::Add(widget) == Success() && widget->GetObjectType() == ListEntry::classID)
	{
		elementOrder.Add((ListEntry *) widget, widget->GetHandle());
		createdEntry.Add(False, widget->GetHandle());

		Paint(SP_UPDATE);

		onChangeEntryOrder.Emit();

		return Success();
	}

	return Error();
}

S::Int S::GUI::List::Remove(Widget *widget)
{
	if (widget == NIL) return Error();

	if (Widget::Remove(widget) == Success() && widget->GetObjectType() == ListEntry::classID)
	{
		Int	 entryHandle = widget->GetHandle();

		if (createdEntry.Get(entryHandle)) DeleteObject(widget);

		elementOrder.Remove(entryHandle);
		createdEntry.Remove(entryHandle);

		Paint(SP_UPDATE);

		onChangeEntryOrder.Emit();

		return Success();
	}

	return Error();
}

S::Int S::GUI::List::RemoveAllEntries()
{
	Surface	*surface = NIL;

	if (IsRegistered()) surface = GetDrawSurface();

	Rect	 frame	 = Rect(GetRealPosition(), GetSize());
	Bool	 visible = IsVisible();

	if (surface != NIL)
	{
		surface->StartPaint(frame);

		if (visible) Hide();
	}

	while (Length() > 0) Remove(GetNthEntry(Length() - 1));

	if (surface != NIL)
	{
		if (visible) Show();

		surface->EndPaint();
	}

	return Success();
}

S::Int S::GUI::List::SwitchEntries(Int entry1n, Int entry2n)
{
	if (entry1n == entry2n)			        return Success();
	if (entry1n >= Length() || entry2n >= Length()) return Error();
	if (entry1n <  0	|| entry2n <  0	      ) return Error();

	ListEntry	*entry1 = elementOrder.GetNth(Math::Min(entry1n, entry2n));
	ListEntry	*entry2 = elementOrder.GetNth(Math::Max(entry1n, entry2n));

	elementOrder.Remove(entry1->GetHandle());
	elementOrder.Remove(entry2->GetHandle());

	elementOrder.InsertAtPos(Math::Min(entry1n, entry2n), entry2, entry2->GetHandle());
	elementOrder.InsertAtPos(Math::Max(entry1n, entry2n), entry1, entry1->GetHandle());

	onChangeEntryOrder.Emit();

	return Success();
}

S::Int S::GUI::List::Length() const
{
	return elementOrder.Length();
}

S::GUI::ListEntry *S::GUI::List::GetNthEntry(Int n) const
{
	return elementOrder.GetNth(n);
}

S::Int S::GUI::List::SelectEntry(ListEntry *entryToSelect)
{
	for (Int i = 0; i < elementOrder.Length(); i++)
	{
		ListEntry	*entry = GetNthEntry(i);

		if (entry == entryToSelect)
		{
			entry->Select();

			return Success();
		}
	}

	return Error();
}

S::GUI::ListEntry *S::GUI::List::GetSelectedEntry() const
{
	for (Int i = 0; i < elementOrder.Length(); i++)
	{
		ListEntry	*entry = elementOrder.GetNth(i);

		if (entry->IsSelected()) return entry;
	}

	return NIL;
}

S::Int S::GUI::List::SelectNthEntry(Int n)
{
	if (n < 0 || n >= Length()) return Error();

	ListEntry	*entry = GetNthEntry(n);

	entry->Select();

	return Success();
}

S::Int S::GUI::List::GetSelectedEntryNumber() const
{
	for (Int i = 0; i < elementOrder.Length(); i++)
	{
		if (elementOrder.GetNth(i)->IsSelected()) return i;
	}

	return -1;
}

S::Int S::GUI::List::GetEntryNumber(const String &entryText) const
{
	for (Int i = 0; i < elementOrder.Length(); i++)
	{
		ListEntry	*entry = elementOrder.GetNth(i);

		if (entry->GetText() == entryText) return i;
	}

	return -1;
}

S::GUI::ListEntry *S::GUI::List::GetEntry(const String &entryText) const
{
	return GetNthEntry(GetEntryNumber(entryText));
}

S::Int S::GUI::List::SelectEntry(const String &entryText)
{
	return SelectNthEntry(GetEntryNumber(entryText));
}
