 /* The smooth Class Library
  * Copyright (C) 1998-2006 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/gui/widgets/multi/menu/popupmenu.h>
#include <smooth/gui/widgets/multi/menu/popupmenuentry.h>
#include <smooth/gui/widgets/multi/menu/menubarentry.h>
#include <smooth/gui/application/application.h>
#include <smooth/gui/window/toolwindow.h>
#include <smooth/graphics/surface.h>
#include <smooth/misc/math.h>

const S::Int	 S::GUI::PopupMenu::classID = S::Object::RequestClassID();

S::Signal1<S::Void, S::Int>	 S::GUI::PopupMenu::internalOnOpenPopupMenu;

S::GUI::PopupMenu::PopupMenu()
{
	type		= classID;
	orientation	= OR_FREE;

	hasNext		= False;

	toolWindow = new ToolWindow(GetPosition(), GetSize());
	toolWindow->Hide();
	toolWindow->onPaint.Connect(&PopupMenu::OnToolWindowPaint, this);
	toolWindow->onLoseFocus.Connect(&internalRequestClose);

	RegisterObject(toolWindow);

	internalRequestClose.SetParentObject(this);

	internalOnOpenPopupMenu.Connect(&PopupMenu::OnOpenPopupMenu, this);
}

S::GUI::PopupMenu::~PopupMenu()
{
	if (toolWindow != NIL) DeleteObject(toolWindow);

	internalOnOpenPopupMenu.Disconnect(&PopupMenu::OnOpenPopupMenu, this);
}

S::GUI::MenuEntry *S::GUI::PopupMenu::AddEntry(const String &text, const Bitmap &bitmap, PopupMenu *popupMenu, Bool *bVar, Int *iVar, Int iCode)
{
	PopupMenuEntry	*newEntry = new PopupMenuEntry(text, bitmap, popupMenu, bVar, iVar, iCode);

	newEntry->SetOwner(this);

	RegisterObject(newEntry);

	SetSize(Size(50, 5));

	Int	 nextYPos = 3;

	for (Int i = 0; i < GetNOfObjects(); i++)
	{
		MenuEntry	*entry = (MenuEntry *) GetNthObject(i);

		if (entry->GetObjectType() != PopupMenuEntry::classID) continue;

		SetWidth(Math::Max(GetWidth(), 6 + entry->GetWidth()));
		SetHeight(GetHeight() + 5 + (entry->GetText() != NIL ? 11 : 0));

		entry->SetPosition(Point(3, nextYPos));

		nextYPos += entry->GetHeight() + 1;
	}

	for (Int j = 0; j < GetNOfObjects(); j++)
	{
		MenuEntry	*entry = (MenuEntry *) GetNthObject(j);

		if (entry->GetObjectType() != PopupMenuEntry::classID) continue;

		entry->SetWidth(GetWidth() - 6);
	}

	return newEntry;
}

S::Int S::GUI::PopupMenu::Show()
{
	if (IsVisible()) return Success();

	visible = True;

	if (!IsRegistered()) return Success();

	hasNext = False;

	for (Int i = 0; i < GetNOfObjects(); i++)
	{
		MenuEntry	*entry = (MenuEntry *) GetNthObject(i);

		if (entry->GetObjectType() != PopupMenuEntry::classID) continue;

		entry->SetRegisteredFlag(False);

		toolWindow->RegisterObject(entry);

		entry->Activate();
	}

	internalOnOpenPopupMenu.Emit(GetHandle());

	toolWindow->SetMetrics(GetPosition(), GetSize());
	toolWindow->Show();

	onShow.Emit();

	return Success();
}

S::Int S::GUI::PopupMenu::Hide()
{
	if (!visible) return Success();

	visible = False;

	if (!IsRegistered()) return Success();

	toolWindow->Hide();

	for (Int i = 0; i < GetNOfObjects(); i++)
	{
		MenuEntry	*entry = (MenuEntry *) GetNthObject(i);

		if (entry->GetObjectType() != PopupMenuEntry::classID) continue;

		toolWindow->UnregisterObject(entry);

		entry->SetRegisteredFlag(True);
		entry->SetContainer(this);

		entry->Deactivate();
	}

	onHide.Emit();

	return Success();
}

S::Void S::GUI::PopupMenu::OnOpenPopupMenu(Int handle)
{
	if (hasNext) return;

	if (handle != GetHandle()) internalRequestClose.Emit();
}

S::Void S::GUI::PopupMenu::OnToolWindowPaint()
{
	Surface	*surface = toolWindow->GetDrawSurface();
	Rect	 frame = Rect(Point(0, 0), GetSize());

	surface->Frame(frame, FRAME_UP);
}
