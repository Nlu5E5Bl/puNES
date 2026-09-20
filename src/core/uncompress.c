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

#include <stdlib.h>
#include <string.h>
#include "info.h"
#include "c++/l7zip/l7z.h"
#include "gui.h"

static uTCHAR *uncompress_storage_index_file_name(uint32_t index);

static BYTE (*uncompress_examine_archive)(_uncompress_archive *archive);
static BYTE (*uncompress_extract_from_archive)(_uncompress_archive *archive, uint32_t selected, BYTE type);
static uTCHAR *(*uncompress_item_file_name)(_uncompress_archive *archive, uint32_t selected, BYTE type);

_uncompress_storage uncstorage;

BYTE uncompress_init(void) {
	l7z_init();
	uncstorage.count = 0;
	uncstorage.item = NULL;
	return (EXIT_OK);
}
void uncompress_quit(void) {
	uint32_t i = 0;

	l7z_quit();

	for (i = 0; i < uncstorage.count; i++) {
		_uncompress_storage_item *sitem = &uncstorage.item[i];

		free(sitem->archive);
		uremove(sitem->file);
		free(sitem->file);
	}
	if (uncstorage.item) {
		free(uncstorage.item);
	}
}

_uncompress_archive *uncompress_archive_alloc(uTCHAR *file, BYTE *rc) {
	_uncompress_archive *archive = NULL;
	uTCHAR *ext = NULL;
	uint32_t size = 0;

	// rintraccio l'ultimo '.' nel nome
	ext = ustrrchr(file, uL('.'));
	if (!ext) {
		(*rc) = UNCOMPRESS_EXIT_IS_NOT_COMP;
		return (NULL);
	}

	if (l7z_present() && (l7z_control_ext(ext) == EXIT_OK)) {
		uncompress_examine_archive = l7z_examine_archive;
		uncompress_extract_from_archive = l7z_extract_from_archive;
		uncompress_item_file_name = l7z_item_file_name;
	} else {
		(*rc) = UNCOMPRESS_EXIT_IS_NOT_COMP;
		return (NULL);
	}

	archive = (_uncompress_archive *)malloc(sizeof(_uncompress_archive));
	if (!archive) {
		(*rc) = UNCOMPRESS_EXIT_ERROR_ON_UNCOMP;
		return (NULL);
	}

	archive->file = NULL;

	size = ustrlen(file) + 1;
	archive->file = (uTCHAR *)malloc(sizeof(uTCHAR) * size);
	if (!archive->file) {
		uncompress_archive_free(archive);
		(*rc) = UNCOMPRESS_EXIT_ERROR_ON_UNCOMP;
		return (NULL);
	}
	umemset(archive->file, 0x00, size);
	ustrcpy(archive->file, file);

	archive->rom.count = 0;
	archive->rom.storage_index = UNCOMPRESS_NO_FILE_SELECTED;

	archive->floppy_disk.count = 0;
	archive->floppy_disk.storage_index = UNCOMPRESS_NO_FILE_SELECTED;

	archive->patch.count = 0;
	archive->patch.storage_index = UNCOMPRESS_NO_FILE_SELECTED;

	archive->list.count = 0;
	archive->list.item = NULL;

	(*rc) = uncompress_examine_archive(archive);
	if ((*rc) != UNCOMPRESS_EXIT_OK) {
		uncompress_archive_free(archive);
		return (NULL);
	}

	return (archive);
}
void uncompress_archive_free(_uncompress_archive *archive) {
	if (!archive) {
		return;
	}
	if (archive->file) {
		free(archive->file);
		archive->file = NULL;
	}
	if (archive->list.item) {
		free(archive->list.item);
		archive->list.item = NULL;
	}
	free(archive);
}
uint32_t uncompress_archive_counter(_uncompress_archive *archive, BYTE type) {
	switch (type) {
		case UNCOMPRESS_TYPE_ROM:
			return (archive->rom.count);
		case UNCOMPRESS_TYPE_FLOPPY_DISK:
			return (archive->floppy_disk.count);
		case UNCOMPRESS_TYPE_PATCH:
			return (archive->patch.count);
		default:
		case UNCOMPRESS_TYPE_ALL:
			return (archive->list.count);
	}
}
BYTE uncompress_archive_extract_file(_uncompress_archive *archive, BYTE type) {
	uint32_t selected = 0;
	BYTE rc = 0;

	switch (uncompress_archive_counter(archive, type)) {
		case 0:
			rc = UNCOMPRESS_EXIT_IS_COMP_BUT_NO_ITEMS;
			break;
		case 1:
			rc = uncompress_extract_from_archive(archive, selected, type);
			break;
		default:
			selected = gui_uncompress_selection_dialog(archive, type);

			if (selected == UNCOMPRESS_NO_FILE_SELECTED) {
				rc = UNCOMPRESS_EXIT_IS_COMP_BUT_NOT_SELECTED;
			} else {
				rc = uncompress_extract_from_archive(archive, selected, type);
			}
			break;
	}
	return (rc);
}
_uncompress_archive_item *uncompress_archive_find_item(_uncompress_archive *archive, uint32_t selected, BYTE type) {
	uint32_t i = 0, index = 0;

	for (i = 0; i < archive->list.count; i++) {
		_uncompress_archive_item *aitem = &archive->list.item[i];

		if ((type != UNCOMPRESS_TYPE_ALL) && !(aitem->type & type)) {
			continue;
		}
		if (index == selected) {
			return (aitem);
		}
		index++;
	}
	return (NULL);
}
uTCHAR *uncompress_archive_extracted_file_name(_uncompress_archive *archive, BYTE type) {
	uint32_t selected = 0;

	switch (type) {
		default:
		case UNCOMPRESS_TYPE_ALL:
		case UNCOMPRESS_TYPE_ROM:
			selected = archive->rom.storage_index;
			break;
		case UNCOMPRESS_TYPE_FLOPPY_DISK:
			selected = archive->floppy_disk.storage_index;
			break;
		case UNCOMPRESS_TYPE_PATCH:
			selected = archive->patch.storage_index;
			break;
	}

	if (selected == UNCOMPRESS_NO_FILE_SELECTED) {
		return (NULL);
	}

	return (uncompress_storage_index_file_name(selected));
}
uTCHAR *uncompress_archive_file_name(_uncompress_archive *archive, uint32_t selected, BYTE type) {
	return (uncompress_item_file_name(archive, selected, type));
}

uTCHAR *uncompress_storage_archive_name(uTCHAR *file) {
	uint32_t i = 0;

	for (i = 0; i < uncstorage.count; i++) {
		_uncompress_storage_item *sitem = &uncstorage.item[i];

		if (!ustrcmp(file, sitem->file)) {
			return (sitem->archive);
		}
	}

	return (NULL);
}
uint32_t uncompress_storage_add_to_list(_uncompress_archive *archive, _uncompress_archive_item *aitem, uTCHAR *file) {
	_uncompress_storage_item *sitem = NULL, *si = NULL;
	BYTE found = FALSE;
	uint32_t i = 0;

	for (i = 0; i < uncstorage.count; i++) {
		si = &uncstorage.item[i];

		if (!ustrcmp(file, si->file)) {
			found = TRUE;
			break;
		}
	}

	if (!found) {
		_uncompress_storage_item *item = NULL;

		item = (_uncompress_storage_item *)realloc(uncstorage.item, (uncstorage.count + 1) * sizeof(_uncompress_storage_item));
		if (item) {
			uncstorage.item = item;
			sitem = &uncstorage.item[uncstorage.count];
			memset(sitem, 0x00, sizeof(_uncompress_storage_item));
			i = uncstorage.count;
			uncstorage.count++;
		}
	} else {
		sitem = si;
	}

	sitem->type = aitem->type;
	sitem->archive = emu_ustrncpy(sitem->archive, archive->file);
	sitem->file = emu_ustrncpy(sitem->file, file);
	sitem->index = aitem->index;

	return (i);
}

static uTCHAR *uncompress_storage_index_file_name(uint32_t index) {
	_uncompress_storage_item *sitem = &uncstorage.item[index];

	if (index >= uncstorage.count) {
		return (NULL);
	}
	return (sitem->file);
}

