 /* The smooth Class Library
  * Copyright (C) 1998-2003 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the "Artistic License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#define MAKEUNICODESTR(x) L##x

#include <smooth/filedlg.h>
#include <smooth/stk.h>
#include <smooth/window.h>

#include <commdlg.h>

S::DialogFileSelection::DialogFileSelection()
{
	flags = 0;
	mode = 0;
}

S::DialogFileSelection::~DialogFileSelection()
{
}

S::Int S::DialogFileSelection::ShowDialog()
{
	static OPENFILENAMEW	 ofnw;
	static OPENFILENAMEA	 ofna;
	wchar_t			*filterw = new wchar_t [32768];
	char			*filtera = new char [32768];
	wchar_t			*bufferw = new wchar_t [32768];
	char			*buffera = new char [32768];
	bool			 result;
	Int			 retValue = Success;

	for (Int i = 0; i < 32768; i++) bufferw[i] = 0;
	for (Int j = 0; j < 32768; j++) buffera[j] = 0;

	if (parentWindow != NIL)
	{
		ofnw.hwndOwner = parentWindow->hwnd;
		ofna.hwndOwner = parentWindow->hwnd;
	}
	else
	{
		ofnw.hwndOwner = NIL;
		ofna.hwndOwner = NIL;
	}

	ofnw.lStructSize	= sizeof(OPENFILENAMEW);
	ofnw.nFilterIndex	= 1;
	ofnw.lpstrFile		= bufferw;
	ofnw.nMaxFile		= 32786;
	ofnw.lpstrFileTitle	= NIL;
	ofnw.lpstrInitialDir	= NIL;
	ofnw.lpstrTitle		= caption;
	ofnw.Flags		= OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | flags;
	ofnw.lpstrDefExt	= defExt;

	ofna.lStructSize	= sizeof(OPENFILENAMEA);
	ofna.nFilterIndex	= 1;
	ofna.lpstrFile		= buffera;
	ofna.nMaxFile		= 32786;
	ofna.lpstrFileTitle	= NIL;
	ofna.lpstrInitialDir	= NIL;
	ofna.lpstrTitle		= caption;
	ofna.Flags		= OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | flags;
	ofna.lpstrDefExt	= defExt;

	Int	 bpos = 0;

	for (int k = 0; k < filters.GetNOfEntries(); k++)
	{
		for (int l = 0; l < filterNames.GetNthEntry(k).Length(); l++)
		{
			filterw[bpos] = filterNames.GetNthEntry(k)[l];
			filtera[bpos] = filterNames.GetNthEntry(k)[l];

			bpos++;
		}

		filterw[bpos] = 0;
		filtera[bpos] = 0;

		bpos++;

		for (int m = 0; m < filters.GetNthEntry(k).Length(); m++)
		{
			filterw[bpos] = filters.GetNthEntry(k)[m];
			filtera[bpos] = filters.GetNthEntry(k)[m];

			bpos++;
		}

		filterw[bpos] = 0;
		filtera[bpos] = 0;

		bpos++;
	}

	filterw[bpos++] = 0;
	filtera[bpos++] = 0;

	ofnw.lpstrFilter = filterw;
	ofna.lpstrFilter = filtera;

	if (mode == SFM_OPEN)
	{
		ofnw.Flags |= OFN_FILEMUSTEXIST;
		ofna.Flags |= OFN_FILEMUSTEXIST;

		if (SMOOTH::Setup::enableUnicode)	result = GetOpenFileNameW(&ofnw);
		else					result = GetOpenFileNameA(&ofna);
	}
	else
	{
		if (SMOOTH::Setup::enableUnicode)	result = GetSaveFileNameW(&ofnw);
		else					result = GetSaveFileNameA(&ofna);
	}

	if (result)
	{
		Int	 n;
		Int	 pos = 0;
		String	 dir;
		String	 file;
		wchar_t	*buffer2w = new wchar_t [1024];
		char	*buffer2a = new char [1024];

		if (SMOOTH::Setup::enableUnicode)
		{
			for (n = 0; n < 32768; n++)
			{
				buffer2w[pos++] = bufferw[n];

				if (bufferw[n] == 0)
				{
					dir.Copy(buffer2w);

					break;
				}
			}

			n++;
			pos = 0;

			for (; n < 32768; n++)
			{
				buffer2w[pos++] = bufferw[n];

				if (bufferw[n] == 0)
				{
					file = file.Copy(dir).Append("\\").Append(buffer2w);

					if (file[file.Length() - 1] == '\\') file[file.Length() - 1] = 0;

					files.AddEntry(file);

					pos = 0;

					if (bufferw[n + 1] == 0) break;
				}
			}
		}
		else
		{
			for (n = 0; n < 32768; n++)
			{
				buffer2a[pos++] = buffera[n];

				if (buffera[n] == 0)
				{
					dir.Copy(buffer2a);

					break;
				}
			}

			n++;
			pos = 0;

			for (; n < 32768; n++)
			{
				buffer2a[pos++] = buffera[n];

				if (buffera[n] == 0)
				{
					file = file.Copy(dir).Append("\\").Append(buffer2a);

					if (file[file.Length() - 1] == '\\') file[file.Length() - 1] = 0;

					files.AddEntry(file);

					pos = 0;

					if (buffera[n + 1] == 0) break;
				}
			}
		}

		delete [] buffer2w;
		delete [] buffer2a;
	}
	else
	{
		retValue = Error;
	}

	delete [] bufferw;
	delete [] buffera;
	delete [] filterw;
	delete [] filtera;

	return retValue;
}

S::Int S::DialogFileSelection::AddFilter(String name, String filter)
{
	filterNames.AddEntry(name);
	filters.AddEntry(filter);

	return Success;
}

S::Int S::DialogFileSelection::SetFlags(Int newFlags)
{
	flags |= newFlags;

	return Success;
}

S::Int S::DialogFileSelection::SetMode(Int newMode)
{
	mode = newMode;

	return Success;
}

S::Int S::DialogFileSelection::SetDefaultExtension(String newDefExt)
{
	defExt = newDefExt;

	return Success;
}

S::Int S::DialogFileSelection::GetNumberOfFiles()
{
	return files.GetNOfEntries();
}

S::String S::DialogFileSelection::GetFileName()
{
	if (files.GetNOfEntries() > 0)	return files.GetFirstEntry();
	else				return NIL;
}

S::String S::DialogFileSelection::GetNthFileName(Int n)
{
	if (files.GetNOfEntries() > n)	return files.GetNthEntry(n);
	else				return NIL;
}