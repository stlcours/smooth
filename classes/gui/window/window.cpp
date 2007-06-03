 /* The smooth Class Library
  * Copyright (C) 1998-2007 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/gui/window/window.h>
#include <smooth/gui/window/backends/windowbackend.h>
#include <smooth/gui/application/application.h>
#include <smooth/gui/widgets/basic/titlebar.h>
#include <smooth/gui/widgets/basic/statusbar.h>
#include <smooth/gui/widgets/multi/menu/popupmenu.h>
#include <smooth/gui/widgets/layer.h>
#include <smooth/misc/math.h>
#include <smooth/gui/window/toolwindow.h>
#include <smooth/graphics/color.h>
#include <smooth/gui/widgets/multi/menu/menubar.h>
#include <smooth/gui/mdi/window.h>
#include <smooth/basic/input.h>
#include <smooth/resources.h>
#include <smooth/misc/binary.h>
#include <smooth/graphics/surface.h>
#include <smooth/backends/win32/backendwin32.h>
#include <smooth/system/event.h>
#include <smooth/system/multimonitor.h>

const S::Int	 S::GUI::Window::classID = S::Object::RequestClassID();
S::Int		 S::GUI::Window::nOfActiveWindows = 0;

S::GUI::Window::Window(const String &title, const Point &iPos, const Size &iSize, Void *iWindow) : Widget(iPos, iSize)
{
	backend = WindowBackend::CreateBackendInstance();
	backend->onEvent.SetParentObject(this);
	backend->onEvent.Connect(&Window::Process, this);

	stay		= False;
	maximized	= False;
	minimized	= False;

	type = classID;

	order = 0;

	layoutDirection = LD_DEFAULT;

	if (title != NIL)	text = title;
	else			text = "smooth Application";

	frameWidth = GetSystemMetrics(SM_CXFRAME);
	updateRect = Rect(Point(-1, -1), Size(0, 0));

	icon = NIL;

	created		= False;
	visible		= False;
	destroyed	= False;
	initshow	= False;

	trackMenu = NIL;

	mainLayer = new Layer();

	Add(mainLayer);

	doQuit.Connect(True);

	onCreate.SetParentObject(this);
	onPaint.SetParentObject(this);
	onResize.SetParentObject(this);
	onPeek.SetParentObject(this);
	onEvent.SetParentObject(this);
}

S::GUI::Window::~Window()
{
	if (created && !destroyed) backend->Close();

	Remove(mainLayer);
	DeleteObject(mainLayer);

	if (trackMenu != NIL) ClosePopupMenu();

	if (onPeek.GetNOfConnectedSlots() > 0) peekLoop--;

	delete backend;
}

S::Int S::GUI::Window::SetMetrics(const Point &nPos, const Size &nSize)
{
	if (created && visible) backend->SetMetrics(nPos, nSize);

	Bool	 resized = (GetWidth() != nSize.cx || GetHeight() != nSize.cy);
	Bool	 prevVisible = visible;

	visible = False;

	Widget::SetMetrics(nPos, nSize);

	if (resized)
	{
		Surface	*surface = GetDrawSurface();

		surface->SetSize(GetSize() * Font::GetSystemFontSize() / 96);

		CalculateOffsets();

		onResize.Emit();
	}

	visible = prevVisible;

	return Success();
}

const S::GUI::Bitmap &S::GUI::Window::GetIcon() const
{
	return icon;
}

S::Int S::GUI::Window::SetIcon(const Bitmap &nIcon)
{
	Bitmap	 newIcon = nIcon;

#ifdef __WIN32__
	if (nIcon == NIL) newIcon = Bitmap((HBITMAP) LoadImageA(hDllInstance, MAKEINTRESOURCEA(IDB_ICON), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_SHARED));
#endif

	if (newIcon != NIL)
	{
		icon = newIcon;

		icon.ReplaceColor(Color(192, 192, 192), Setup::BackgroundColor);

		backend->SetIcon(icon);
	}

	return Success();
}

S::GUI::Layer *S::GUI::Window::GetMainLayer() const
{
	return mainLayer;
}

S::Int S::GUI::Window::SetText(const String &nTitle)
{
	text = nTitle;

	if (created)
	{
		Process(SM_WINDOWTITLECHANGED, 0, 0);

		backend->SetTitle(text);
	}

	return Success();
}

S::Int S::GUI::Window::SetStatusText(const String &nStatus)
{
	for (Int i = 0; i < GetNOfObjects(); i++)
	{
		Widget *widget = GetNthObject(i);

		if (widget == NIL) continue;

		if (widget->GetObjectType() == Statusbar::classID)
		{
			widget->SetText(nStatus);

			return Success();
		}
	}

	return Error();
}

const S::String &S::GUI::Window::GetStatusText() const
{
	for (Int i = 0; i < GetNOfObjects(); i++)
	{
		Widget *widget = GetNthObject(i);

		if (widget == NIL) continue;

		if (widget->GetObjectType() == Statusbar::classID) return widget->GetText();
	}

	return defaultStatus;
}

S::Int S::GUI::Window::SetDefaultStatusText(const String &nStatus)
{
	defaultStatus = nStatus;

	RestoreDefaultStatusText();

	return Success();
}

S::Int S::GUI::Window::RestoreDefaultStatusText()
{
	for (Int i = 0; i < GetNOfObjects(); i++)
	{
		Widget *widget = GetNthObject(i);

		if (widget == NIL) continue;

		if (widget->GetObjectType() == Statusbar::classID)
		{
			widget->SetText(defaultStatus);

			break;
		}
	}

	return Success();
}

S::Int S::GUI::Window::SetRightToLeft(Bool nRightToLeft)
{
	layoutDirection = (nRightToLeft ? LD_RIGHTTOLEFT : LD_LEFTTORIGHT);

	return Success();
}

S::Bool S::GUI::Window::IsRightToLeft() const
{
	return ((layoutDirection == LD_DEFAULT) ? Setup::rightToLeft : (layoutDirection == LD_RIGHTTOLEFT));
}

S::Int S::GUI::Window::Show()
{
	order = Object::RequestObjectHandle();

	if (!created) Create();

	backend->SetMetrics(GetPosition(), GetSize());

	onResize.Emit();

	if (maximized && !initshow)
	{
		backend->Hide();

		maximized = False;
		Maximize();
	}

	if (minimized && !initshow)
	{
		backend->Hide();

		minimized = False;
		Minimize();
	}

	backend->Show();

	initshow	= True;
	visible		= True;

	onShow.Emit();

	return Success();
}

S::Int S::GUI::Window::Hide()
{
	if (!created) Create();

	backend->Hide();

	if (maximized && !initshow)
	{
		maximized = False;
		Maximize();
	}

	if (minimized && !initshow)
	{
		minimized = False;
		Minimize();
	}

	initshow	= True;
	visible		= False;

	onHide.Emit();

	return Success();
}

S::Int S::GUI::Window::Minimize()
{
	if (minimized) return Success();

	if (!created)
	{
		minimized = True;

		return Success();
	}

	backend->Minimize();

	minimized = True;

	return Success();
}

S::Int S::GUI::Window::Maximize()
{
	if (maximized) return Success();

	if (!created)
	{
		maximized = True;

		return Success();
	}

	backend->Maximize();

	maximized = True;

	return Success();
}

S::Int S::GUI::Window::Restore()
{
	if (!maximized && !minimized) return Success();

	if (!created)
	{
		maximized = False;
		minimized = False;

		return Success();
	}

	backend->Restore();

	maximized = False;
	minimized = False;

	return Success();
}

S::GUI::Point S::GUI::Window::GetRealPosition() const
{
	return Point(0, 0);
}

S::GUI::Rect S::GUI::Window::GetWindowRect() const
{
	return Rect(GetPosition(), GetSize());
}

S::GUI::Rect S::GUI::Window::GetClientRect() const
{
	return Rect(Point(innerOffset.left, innerOffset.top), GetSize() - Size(innerOffset.left + innerOffset.right, innerOffset.top + innerOffset.bottom));
}

S::GUI::Rect S::GUI::Window::GetRestoredWindowRect() const
{
	if (IsMaximized())	return backend->GetRestoredWindowRect();
	else			return GetWindowRect();
}

const S::GUI::Rect &S::GUI::Window::GetUpdateRect() const
{
	return updateRect;
}

S::Int S::GUI::Window::SetUpdateRect(const Rect &newUpdateRect)
{
	updateRect = newUpdateRect;

	return Success();
}

S::Int S::GUI::Window::SetMinimumSize(const Size &newMinSize)
{
	backend->SetMinimumSize(newMinSize);

	return Success();
}

S::Int S::GUI::Window::SetMaximumSize(const Size &newMaxSize)
{
	backend->SetMaximumSize(newMaxSize);

	return Success();
}

S::Bool S::GUI::Window::Create()
{
	if (IsRegistered() && !created)
	{
		if (GetPosition().x >= 0 && GetPosition().y >= 0 && GetPosition().x < LiSAGetDisplaySizeX() && GetPosition().y < LiSAGetDisplaySizeY())
		{
			Rect	 monitorRect = System::MultiMonitor::GetActiveMonitorMetrics();

			SetPosition(Point(monitorRect.left, monitorRect.top) + GetPosition());
		}

		if (backend->Open(text, GetPosition(), GetSize(), flags) == Success())
		{
			if (GetObjectType() != ToolWindow::classID) nOfActiveWindows++;

			created = True;
			visible = False;

			if (flags & WF_NORESIZE) frameWidth = 4;

			CalculateOffsets();

			onCreate.Emit();

			return True;
		}
		else
		{
			return False;
		}
	}

	if (onPeek.GetNOfConnectedSlots() > 0) peekLoop++;

	return False;
}

S::Int S::GUI::Window::Stay()
{
	if (!IsRegistered()) return Error();

	SetFlags(flags | WF_MODAL);

	if (!created)	Create();
	if (!visible)	Show();

	stay = True;

	System::EventProcessor	*event = new System::EventProcessor();

	while (!destroyed)
	{
		if (peekLoop > 0) event->ProcessNextEvent(False);
		else		  event->ProcessNextEvent(True);
	}

	delete event;

#ifdef __WIN32__
	if (nOfActiveWindows == 0 && !initializing) PostQuitMessage(0);
#endif

	return Success();
}

S::Int S::GUI::Window::Close()
{
#ifdef __WIN32__
	if (Setup::enableUnicode) PostMessageW((HWND) backend->GetSystemWindow(), WM_CLOSE, 0, 0);
	else			  PostMessageA((HWND) backend->GetSystemWindow(), WM_CLOSE, 0, 0);
#endif

	return Success();
}

S::Bool S::GUI::Window::IsInUse() const
{
	return (created && !destroyed);
}

S::Int S::GUI::Window::Process(Int message, Int wParam, Int lParam)
{
	if (!created) return Success();

	EnterProtectedRegion();

	if (!(message == SM_MOUSEMOVE && wParam == 1)) onEvent.Emit(message, wParam, lParam);

	Int	 rVal = Success();

#ifdef __WIN32__
	switch (message)
	{
		case WM_CLOSE:
			if (doQuit.Call()) backend->Close();

			rVal = Break;

			break;
		case WM_DESTROY:
			destroyed = True;

			if (nOfActiveWindows == 0 && loopActive)
			{
				if (Setup::enableUnicode)	::SendMessageW((HWND) backend->GetSystemWindow(), WM_QUIT, 0, 0);
				else				::SendMessageA((HWND) backend->GetSystemWindow(), WM_QUIT, 0, 0);
			}
			else
			{
				if (GetObjectType() != ToolWindow::classID) nOfActiveWindows--;
			}

			rVal = Break;

			break;
		case WM_QUIT:
			destroyed = True;

			if (GetObjectType() != ToolWindow::classID) nOfActiveWindows--;

			LeaveProtectedRegion();

			PostQuitMessage(0);

			return Break;
		case WM_SYSCOLORCHANGE:
			GetColors();

			break;
		case WM_PAINT:
			{
				RECT	 uRect = { 0, 0, 0, 0 };

				updateRect = uRect;

				if (::GetUpdateRect((HWND) backend->GetSystemWindow(), &uRect, 0))
				{
					updateRect = uRect;

					updateRect.right += frameWidth;
					updateRect.bottom += frameWidth;

					PAINTSTRUCT	 ps;

					BeginPaint((HWND) backend->GetSystemWindow(), &ps);

					if (Math::Abs((updateRect.right - updateRect.left) - GetWidth()) < 20 && Math::Abs((updateRect.bottom - updateRect.top) - GetHeight()) < 20)	Paint(SP_PAINT);
					else																		Paint(SP_UPDATE);

					EndPaint((HWND) backend->GetSystemWindow(), &ps);
				}
			}

			rVal = Break;

			break;
		case WM_WINDOWPOSCHANGED:
			{
				WINDOWPOS	*wndpos = (LPWINDOWPOS) lParam;

				Window::SetMetrics(Point(wndpos->x, wndpos->y), Size(wndpos->cx, wndpos->cy));
			}

			break;
		case WM_SIZE:
			if (wParam == SIZE_RESTORED) minimized = False;

			break;
		case WM_ACTIVATE:
			if (LOWORD(wParam) != WA_INACTIVE && !(flags & WF_SYSTEMMODAL))
			{
				for (Int i = 0; i < Object::GetNOfObjects(); i++)
				{
					Object	*object = Object::GetNthObject(i);

					if (object == NIL) continue;

					if (object->GetObjectType() == classID && ((Window *) object)->GetOrder() > GetOrder() && (object->GetFlags() & WF_MODAL))
					{
						SetActiveWindow((HWND) ((Window *) object)->GetSystemWindow());

						rVal = Break;

						break;
					}
				}
			}

			if (LOWORD(wParam) == WA_INACTIVE && focussed)
			{
				const Window	*window = GetWindow((HWND) lParam);

				if (window != NIL)
				{
					if (window->GetObjectType() == ToolWindow::classID && window->GetOrder() >= GetOrder()) break;
				}

				focussed = False;

				onLoseFocus.Emit();
			}

			break;
		case WM_SETFOCUS:
			if (!focussed)
			{
				focussed = True;

				onGetFocus.Emit();
			}

			break;
		case WM_KILLFOCUS:
			if (focussed)
			{
				const Window	*window = Window::GetWindow((HWND) wParam);

				if (window != NIL)
				{
					if (window->GetObjectType() == ToolWindow::classID && window->GetOrder() >= GetOrder()) break;
				}

				focussed = False;

				onLoseFocus.Emit();
			}

		case WM_ACTIVATEAPP:
			if (flags & WF_MODAL)
			{
				Bool	 activate = False;
				HWND	 actWnd = ::GetActiveWindow();

				if (actWnd == (HWND) backend->GetSystemWindow()) break;

				Window	*window = GetWindow(actWnd);

				if (window == NIL)				activate = False;
				else if (window->type == ToolWindow::classID)	break;
				else if (window->GetOrder() < GetOrder())	activate = True;
				else if (window->GetOrder() > GetOrder())	window->SetFlags(WF_MODAL);

				if (activate && message == WM_ACTIVATEAPP)
				{
					if (wParam)	activate = True;
					else		activate = False;
				}

				if (activate && message == WM_KILLFOCUS)
				{
					if (GetWindow(SetActiveWindow((HWND) backend->GetSystemWindow())) != NIL)	activate = True;
					else										activate = False;
				}

				if (activate)	SetWindowPos((HWND) backend->GetSystemWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				else		SetWindowPos((HWND) backend->GetSystemWindow(), message == WM_KILLFOCUS ? HWND_NOTOPMOST : GetForegroundWindow(), 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
			}

			if (flags & WF_APPTOPMOST)
			{
				Bool	 activate = False;
				HWND	 actWnd = ::GetActiveWindow();

				if (actWnd == (HWND) backend->GetSystemWindow()) break;

				const Window	*window = GetWindow(actWnd);

				if (window == NIL)				activate = False;
				else if (window->type == ToolWindow::classID)	break;
				else						activate = True;

				if (activate && message == WM_ACTIVATEAPP)
				{
					if (wParam)	activate = True;
					else		activate = False;
				}

				if (activate) SetWindowPos((HWND) backend->GetSystemWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
				else
				{
					SetWindowPos((HWND) backend->GetSystemWindow(), GetForegroundWindow(), 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
					SetWindowPos((HWND) backend->GetSystemWindow(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
				}
			}

			if (flags & WF_SYSTEMMODAL && message == WM_KILLFOCUS)
			{
				Bool	 activate = False;
				HWND	 actWnd = GetForegroundWindow();

				if (actWnd == (HWND) backend->GetSystemWindow()) break;

				Window	*window = GetWindow(actWnd);

				if (window == NIL)				activate = True;
				else if (window->type == ToolWindow::classID)	activate = False;
				else if (window->GetOrder() < GetOrder())	activate = True;
				else if (window->GetOrder() > GetOrder())	window->SetFlags(WF_SYSTEMMODAL);

				if (activate)
				{
					SetWindowPos((HWND) backend->GetSystemWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					SetForegroundWindow((HWND) backend->GetSystemWindow());
				}
			}

			break;
		case SM_EXECUTEPEEK:
			onPeek.Emit();

			rVal = Break;

			break;
		case SM_RBUTTONDOWN:
			OpenPopupMenu();

			if (trackMenu != NIL) rVal = Break;

			break;
	}
#endif

	if (rVal == Success())
	{
		for (Int i = GetNOfObjects() - 1; i >= 0; i--)
		{
			Widget	*object = GetNthObject(i);

			if (object == NIL) continue;

			if (object->Process(message, wParam, lParam) == Break)
			{
				rVal = Break;

				break;
			}
		}
	}

	LeaveProtectedRegion();

	return rVal;
}

S::Int S::GUI::Window::Paint(Int message)
{
	if (!IsRegistered())	return Error();
	if (!created)		return Success();
	if (!IsVisible())	return Success();

	if ((updateRect.right - updateRect.left == 0) || (updateRect.bottom - updateRect.top == 0)) return Success();

	EnterProtectedRegion();

	Surface	*surface = GetDrawSurface();

	surface->SetRightToLeft(IsRightToLeft());

	if (updateRect.left < frameWidth)		  updateRect.left   = frameWidth - 1;
	if (updateRect.top < frameWidth)		  updateRect.top    = frameWidth - 1;
	if (GetWidth() - updateRect.right < frameWidth)	  updateRect.right  = GetWidth() - frameWidth + 1;
	if (GetHeight() - updateRect.bottom < frameWidth) updateRect.bottom = GetHeight() - frameWidth + 1;

	Rect	 workArea = System::MultiMonitor::GetVirtualScreenMetrics();

	if (message == SP_UPDATE				&&
	    GetPosition().x		   > workArea.left - 2	&&
	    GetPosition().y		   > workArea.top - 2	&&
	    GetPosition().x + GetSize().cx < workArea.right + 2	&&
	    GetPosition().y + GetSize().cy < workArea.bottom + 2)
	{
		surface->PaintRect(updateRect);
	}
	else
	{
		Bool	 preRTL = IsRightToLeft();

		surface->SetRightToLeft(False);
		surface->StartPaint(updateRect);
		surface->SetRightToLeft(preRTL);

		surface->Box(updateRect, Setup::BackgroundColor, Rect::Filled);

		if (type != ToolWindow::classID)
		{
			Widget	*lastWidget = NIL;
			Int	 bias = 0;
			Int	 topoffset = frameWidth;
			Int	 rightobjcount = 0;
			Int	 leftobjcount = 0;
			Int	 btmobjcount = 0;
			Int	 topobjcount = 0;

			for (Int i = 0; i < GetNOfObjects(); i++)
			{
				Widget	*object = GetNthObject(i);

				if (object->GetOrientation() == OR_TOP)		topobjcount++;
				else if (object->GetOrientation() == OR_BOTTOM)	btmobjcount++;
				else if (object->GetOrientation() == OR_LEFT)	leftobjcount++;
				else if (object->GetOrientation() == OR_RIGHT)	rightobjcount++;

				if (object->GetOrientation() == OR_TOP)
				{
					lastWidget = object;

					if (object->subtype == WO_SEPARATOR)
					{
						bias = -3;

						topoffset += object->GetHeight() + 3;

						Point	 p1 = Point(frameWidth + 1, topoffset - 2);
						Point	 p2 = Point(GetWidth() - frameWidth, p1.y);

						if (icon != NIL) p1.x += 17;

						surface->Bar(p1, p2, OR_HORZ);
					}
					else
					{
						bias = 0;

						topoffset += object->GetHeight();
					}
				}
			}

			if (topobjcount > 0)
			{
				Point	 p1 = Point(frameWidth, innerOffset.top - 2 + bias);
				Point	 p2 = Point(GetWidth() - frameWidth, p1.y);

				if (lastWidget->subtype == WO_NOSEPARATOR) { p1.y -= 3; p2.y -= 3; }

				surface->Bar(p1, p2, OR_HORZ);
				surface->Bar(p1 + Point(0, 2), p2 + Point(0, 2), OR_HORZ);
			}

			if (btmobjcount > 0)
			{
				Point	 p1 = Point(frameWidth, GetHeight() - innerOffset.bottom);
				Point	 p2 = Point(GetWidth() - frameWidth, p1.y);

				surface->Bar(p1, p2, OR_HORZ);
				surface->Bar(p1 + Point(0, 2), p2 + Point(0, 2), OR_HORZ);
			}

			if (leftobjcount > 0)
			{
				Point	 p1 = Point(innerOffset.left - 3, innerOffset.top);
				Point	 p2 = Point(p1.x, GetHeight() - innerOffset.bottom - 2);

				surface->Bar(p1, p2, OR_VERT);
			}

			if (rightobjcount > 0)
			{
				Point	 p1 = Point(GetWidth() - innerOffset.right + 1, innerOffset.top);
				Point	 p2 = Point(p1.x, GetHeight() - innerOffset.bottom - 2);

				surface->Bar(p1, p2, OR_VERT);
			}
		}

		onPaint.Emit();

		for (Int j = 0; j < GetNOfObjects(); j++)
		{
			Widget	*widget = GetNthObject(j);

			if (widget->IsAffected(updateRect) && widget->GetObjectType() != Layer::classID) widget->Paint(SP_PAINT);
		}

		for (Int k = 0; k < GetNOfObjects(); k++)
		{
			Widget	*widget = GetNthObject(k);

			if (widget->IsAffected(updateRect) && widget->GetObjectType() == Layer::classID) widget->Paint(SP_PAINT);
		}

		surface->EndPaint();
	}

	LeaveProtectedRegion();

	return Success();
}

S::Void S::GUI::Window::CalculateOffsets()
{
	Widget	*lastWidget	= NIL;
	Int	 rightobjcount	= 0;
	Int	 leftobjcount	= 0;
	Int	 btmobjcount	= 0;
	Int	 topobjcount	= 0;

	if (GetObjectType() == ToolWindow::classID)	innerOffset = Rect(Point(0, 0), Size(0, 0));
	else						innerOffset = Rect(Point(frameWidth, frameWidth), Size(0, 0));

	for (Int i = 0; i < GetNOfObjects(); i++)
	{
		Widget	*widget = GetNthObject(i);

		if (widget->GetOrientation() == OR_TOP)
		{
			topobjcount++;

			lastWidget = widget;

			widget->SetMetrics(Point(innerOffset.left, innerOffset.top), Size(GetWidth() - innerOffset.left - innerOffset.right, widget->GetHeight()));

			innerOffset.top += widget->GetHeight();

			if (widget->subtype == WO_SEPARATOR) innerOffset.top += 3;
		}
	}

	if (topobjcount > 0)
	{
		innerOffset.top += 3;

		if (lastWidget->subtype == WO_NOSEPARATOR) innerOffset.top += 3;
	}

	for (Int j = 0; j < GetNOfObjects(); j++)
	{
		Widget	*widget = GetNthObject(j);

		if (widget->GetOrientation() == OR_BOTTOM)
		{
			btmobjcount++;

			widget->SetMetrics(Point(innerOffset.left, GetHeight() - innerOffset.bottom - widget->GetHeight()), Size(GetWidth() - innerOffset.left - innerOffset.right, widget->GetHeight()));

			innerOffset.bottom += widget->GetHeight();
		}
	}

	if (btmobjcount > 0) innerOffset.bottom += 4;

	for (Int k = 0; k < GetNOfObjects(); k++)
	{
		Widget	*widget = GetNthObject(k);

		if (widget->GetOrientation() == OR_LEFT)
		{
			leftobjcount++;

			widget->SetMetrics(Point(innerOffset.left, innerOffset.top), Size(widget->GetWidth(), GetHeight() - innerOffset.top - innerOffset.bottom));

			innerOffset.left += widget->GetWidth();
		}
	}

	if (leftobjcount > 0) innerOffset.left += 3;

	for (Int l = 0; l < GetNOfObjects(); l++)
	{
		Widget	*widget = GetNthObject(l);

		if (widget->GetOrientation() == OR_RIGHT)
		{
			rightobjcount++;

			widget->SetMetrics(Point(GetWidth() - innerOffset.right - widget->GetWidth(), innerOffset.top), Size(widget->GetWidth(), GetHeight() - innerOffset.top - innerOffset.bottom));

			innerOffset.right += widget->GetWidth();
		}
	}

	if (rightobjcount > 0) innerOffset.right += 3;

	for (Int m = 0; m < GetNOfObjects(); m++)
	{
		Widget	*widget = GetNthObject(m);

		if (widget->GetOrientation() == OR_CENTER)
		{
			widget->SetMetrics(Point(innerOffset.left, innerOffset.top), Size(GetWidth() - innerOffset.left - innerOffset.right, GetHeight() - innerOffset.top - innerOffset.bottom));
			widget->SetOrientation(OR_CENTER);
		}
	}
}

S::GUI::Point S::GUI::Window::GetMousePosition() const
{
	Point	 position = Input::GetMousePosition();

	if (IsRightToLeft())	position = Point(GetWidth() - (position.x - GetX()) - 1, position.y - GetY());
	else			position -= GetPosition();

	return position * 96 / Font::GetSystemFontSize();
}

S::Bool S::GUI::Window::IsMouseOn(const Rect &rect) const
{
	Surface	*surface = GetDrawSurface();

	if (surface->GetSystemSurface() == NIL) return False;

#ifdef __WIN32__
	Point	 position = Input::GetMousePosition();
	HWND	 window = (HWND) surface->GetSystemSurface();
	HDC	 dc = GetWindowDC(window);
	Bool	 pointVisible = PtVisible(dc, position.x - GetX(), position.y - GetY());

	ReleaseDC(window, dc);

	if (!pointVisible) return False;
#endif

	Point	 mousePos = GetMousePosition();

	if ((mousePos.x >= rect.left) && (mousePos.x < rect.right) && (mousePos.y >= rect.top) && (mousePos.y < rect.bottom))	return True;
	else															return False;
}

S::GUI::Surface *S::GUI::Window::GetDrawSurface() const
{
	return backend->GetDrawSurface();
}

S::Void *S::GUI::Window::GetSystemWindow() const
{
	return backend->GetSystemWindow();
}

S::Void S::GUI::Window::OpenPopupMenu()
{
	if (trackMenu != NIL) ClosePopupMenu();

	Point	 position = GetMousePosition();

	trackMenu = getTrackMenu.Call(position.x, position.y);

	if (trackMenu != NIL)
	{
		trackMenu->CalculateSize();

		trackMenu->SetPosition(position);
		trackMenu->internalRequestClose.Connect(&Window::ClosePopupMenu, this);

		Add(trackMenu);
	}
}

S::Void S::GUI::Window::ClosePopupMenu()
{
	if (trackMenu != NIL)
	{
		trackMenu->internalRequestClose.Disconnect(&Window::ClosePopupMenu, this);

		Remove(trackMenu);

		trackMenu = NIL;
	}
}

S::Int S::GUI::Window::Add(Widget *widget)
{
	if (widget->GetOrientation() == OR_UPPERLEFT || widget->GetOrientation() == OR_UPPERRIGHT || widget->GetOrientation() == OR_LOWERLEFT || widget->GetOrientation() == OR_LOWERRIGHT) return mainLayer->Add(widget);

	if (Widget::Add(widget) == Success())
	{
		if (widget->GetObjectType() == Titlebar::classID)
		{
			if (!Binary::IsFlagSet(widget->GetFlags(), TB_MAXBUTTON)) flags = flags | WF_NORESIZE;
		}
		else if (widget->GetObjectType() == Statusbar::classID)
		{
			SetDefaultStatusText(widget->GetText());
		}

		if (widget->GetOrientation() != OR_FREE) CalculateOffsets();

		return Success();
	}

	return Error();
}

S::Int S::GUI::Window::Remove(Widget *widget)
{
	if (widget->GetOrientation() == OR_UPPERLEFT || widget->GetOrientation() == OR_UPPERRIGHT || widget->GetOrientation() == OR_LOWERLEFT || widget->GetOrientation() == OR_LOWERRIGHT) return mainLayer->Remove(widget);

	if (Widget::Remove(widget) == Success())
	{
		if (widget->GetOrientation() != OR_FREE) CalculateOffsets();

		return Success();
	}

	return Error();
}

S::GUI::Window *S::GUI::Window::GetWindow(Void *sysWindow)
{
	if (sysWindow == NIL) return NIL;

	for (Int i = 0; i < Object::GetNOfObjects(); i++)
	{
		Object			*window = Object::GetNthObject(i);

		if (window == NIL) continue;

		const ObjectType	&objType = window->GetObjectType();

		if (objType == Window::classID || objType == MDI::Window::classID || objType == ToolWindow::classID)
		{
			if (((Window *) window)->GetSystemWindow() == sysWindow) return (Window *) window;
		}
	}

	return NIL;
}
