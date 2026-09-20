/*
 *  Copyright (C) 2010-2026 Fabio Cavallo (aka FHorse)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef UNICODE_DEF_H_
#define UNICODE_DEF_H_

// windows
#include <wchar.h>

typedef wchar_t uTCHAR;

#if defined (_WIN64) || defined (__MINGW64__)
#define ustructstat _stat64i32
#else
#define ustructstat _stat32
#endif

#define ustring wstring

#define uPs(a) "%" a "ls"
// Plain char* argument inside a wide printf.  %hs is the C99 spelling for a
// narrow string in a wide conversion and is the only one that works here:
// this toolchain's swprintf follows MSVC/UCRT semantics, where both %s and %ls
// consume a wchar_t*, so a char* fed to %s is misread as UTF-16 (each pair of
// ASCII bytes becomes one code unit).
#define uPc(a) "%" a "hs"
#define uL(string) L##string
#define uPTCHAR(string) (wchar_t *)string

#define usizeof(string) LENGTH(string)

#define uQString QString::fromWCharArray
#define uQStringCD(string) uPTCHAR(string.constData())

#define uQByteArrayFromString(string) QByteArray((const char *)string.utf16(), (string.length() + 1) * 2)
#define uQByteArrayCD(string) uPTCHAR(string.constData())

#define uvsnprintf vswprintf
#define umemset wmemset
#define ustrcpy wcscpy
#define ustrncpy wcsncpy
#define usnprintf swprintf
#define uprintf wprintf
#define ustrlen wcslen
#define uaccess _waccess
#define ustat _wstat
#define ufprintf fwprintf
#define ufopen _wfopen
#define ufdopen _wfdopen
#define ustrrchr wcsrchr
#define ustrcasecmp gui_utf_strcasecmp
#define ustrcmp wcscmp
#define ustrncmp wcsncmp
#define ustrcat wcscat
#define uremove _wremove
#define uioctl
#define uopen _wopen
#define uchdir _wchdir
#define ugetcwd _wgetcwd
#define umemcpy wmemcpy
#define ustrchr wcschr
#define ustrdup _wcsdup
#define usscanf swscanf_s
#define ustrstr wcsstr

#endif /* UNICODE_DEF_H_ */
