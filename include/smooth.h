 /* The smooth Class Library
  * Copyright (C) 1998-2006 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#if (defined UNICODE && !defined _UNICODE) || (defined _UNICODE && !defined UNICODE)
#error Either none or both of UNICODE and _UNICODE must be defined to compile SMOOTH applications!
#endif

#ifndef _H_OBJSMOOTH__
#define _H_OBJSMOOTH__

#define __SMOOTH__

#include "smooth/definitions.h"
#include "smooth/loop.h"
#include "smooth/version.h"

#include "smooth/templates/array.h"
#include "smooth/templates/buffer.h"
#include "smooth/templates/extended.h"
#include "smooth/templates/nonblocking.h"
#include "smooth/templates/pointer.h"

#include "smooth/templates/callbacks.h"
#include "smooth/templates/signals.h"
#include "smooth/templates/signalsr.h"
#include "smooth/templates/slots.h"
#include "smooth/templates/slotsr.h"

#include "smooth/basic/input.h"
#include "smooth/basic/object.h"
#include "smooth/basic/setup.h"

#include "smooth/errors/error.h"
#include "smooth/errors/success.h"

#include "smooth/errors/fs/endoffile.h"
#include "smooth/errors/fs/filenotfound.h"

#include "smooth/errors/misc/permissiondenied.h"

#include "smooth/types/bool.h"
#include "smooth/types/float.h"
#include "smooth/types/int.h"
#include "smooth/types/void.h"

#include "smooth/system/console.h"
#include "smooth/system/dynamicloader.h"
#include "smooth/system/multimonitor.h"
#include "smooth/system/system.h"
#include "smooth/system/timer.h"

#include "smooth/files/directory.h"
#include "smooth/files/file.h"

#include "smooth/io/instream.h"
#include "smooth/io/outstream.h"

#include "smooth/threads/mutex.h"
#include "smooth/threads/semaphore.h"
#include "smooth/threads/thread.h"

#include "smooth/graphics/forms/line.h"
#include "smooth/graphics/forms/point.h"
#include "smooth/graphics/forms/rect.h"
#include "smooth/graphics/forms/size.h"

#include "smooth/graphics/bitmap.h"
#include "smooth/graphics/color.h"
#include "smooth/graphics/font.h"
#include "smooth/graphics/surface.h"

#include "smooth/graphics/imageloader/imageloader.h"

#include "smooth/i18n/i18n.h"

#include "smooth/misc/binary.h"
#include "smooth/misc/config.h"
#include "smooth/misc/datetime.h"
#include "smooth/misc/math.h"
#include "smooth/misc/memory.h"
#include "smooth/misc/string.h"

#include "smooth/gui/application/application.h"
#include "smooth/gui/application/background.h"

#include "smooth/gui/dialogs/colordlg.h"
#include "smooth/gui/dialogs/dirdlg.h"
#include "smooth/gui/dialogs/filedlg.h"
#include "smooth/gui/dialogs/fontdlg.h"
#include "smooth/gui/dialogs/messagebox.h"
#include "smooth/gui/dialogs/splashscreen.h"
#include "smooth/gui/dialogs/tipodaydlg.h"

#include "smooth/gui/mdi/client.h"
#include "smooth/gui/mdi/window.h"

#include "smooth/gui/widgets/layer.h"
#include "smooth/gui/widgets/widget.h"

#include "smooth/gui/widgets/basic/activearea.h"
#include "smooth/gui/widgets/basic/arrows.h"
#include "smooth/gui/widgets/basic/button.h"
#include "smooth/gui/widgets/basic/checkbox.h"
#include "smooth/gui/widgets/basic/client.h"
#include "smooth/gui/widgets/basic/divider.h"
#include "smooth/gui/widgets/basic/editbox.h"
#include "smooth/gui/widgets/basic/groupbox.h"
#include "smooth/gui/widgets/basic/hyperlink.h"
#include "smooth/gui/widgets/basic/image.h"
#include "smooth/gui/widgets/basic/multiedit.h"
#include "smooth/gui/widgets/basic/optionbox.h"
#include "smooth/gui/widgets/basic/progressbar.h"
#include "smooth/gui/widgets/basic/scrollbar.h"
#include "smooth/gui/widgets/basic/slider.h"
#include "smooth/gui/widgets/basic/statusbar.h"
#include "smooth/gui/widgets/basic/tabwidget.h"
#include "smooth/gui/widgets/basic/text.h"
#include "smooth/gui/widgets/basic/titlebar.h"

#include "smooth/gui/widgets/hotspot/hotspot.h"
#include "smooth/gui/widgets/hotspot/simplebutton.h"

#include "smooth/gui/widgets/multi/list/list.h"
#include "smooth/gui/widgets/multi/list/listbox.h"
#include "smooth/gui/widgets/multi/list/combobox.h"

#include "smooth/gui/widgets/multi/menu/menu.h"
#include "smooth/gui/widgets/multi/menu/menubar.h"
#include "smooth/gui/widgets/multi/menu/micromenu.h"
#include "smooth/gui/widgets/multi/menu/popupmenu.h"

#include "smooth/gui/widgets/special/cursor.h"
#include "smooth/gui/widgets/special/dragcontrol.h"
#include "smooth/gui/widgets/special/droparea.h"
#include "smooth/gui/widgets/special/shortcut.h"
#include "smooth/gui/widgets/special/tooltip.h"

#include "smooth/gui/window/toolwindow.h"
#include "smooth/gui/window/window.h"

#include "smooth/xml/attribute.h"
#include "smooth/xml/document.h"
#include "smooth/xml/node.h"

#include "smooth/xml/xul/renderer.h"

#endif
