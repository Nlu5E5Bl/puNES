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

#ifndef THREAD_DEF_H_
#define THREAD_DEF_H_

#include "common.h"
#include "win.h"

typedef HANDLE thread_t;
#define thread_create(th, funct, par) th = CreateThread(NULL, 0, funct, par, 0, 0)
#define thread_join(th) WaitForSingleObject(th, INFINITE)
#define thread_free(th) CloseHandle(th)

typedef HANDLE thread_mutex_t;
#define thread_mutex_init(mtx) mtx = CreateSemaphore(NULL, 1, 2, NULL)
#define thread_mutex_init_error(mtx) (thread_mutex_init(mtx)) == NULL
#define thread_mutex_lock(mtx) WaitForSingleObject(mtx, INFINITE)
#define thread_mutex_unlock(mtx) ReleaseSemaphore(mtx, 1, NULL)
#define thread_mutex_destroy(mtx) if (mtx) { CloseHandle(mtx); mtx = NULL; }

#define thread_funct(funct, args) DWORD WINAPI funct(args)
#define thread_funct_return() return (0)

enum thread_states {
	TH_UNINITIALIZED,
	TH_FALSE,
	TH_TRUE
};

#endif /* THREAD_DEF_H_ */

