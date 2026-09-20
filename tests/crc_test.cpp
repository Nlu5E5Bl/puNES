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

/*
 * emu_crc32_zeroes() used to allocate a variable length array of zeroes on the
 * stack, which is not standard C++ and blows the stack for large lengths. It
 * now folds a fixed 4096 byte block of zeroes instead. This test pins the two
 * implementations to the same answer, across lengths that straddle the block
 * size and across several starting CRCs, so a future change to the chunking
 * cannot silently alter a ROM's reported CRC.
 *
 * Build with -DENABLE_TESTS=ON, run tests/crc_test.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

extern "C" {
uint32_t emu_crc32_zeroes(size_t length, uint32_t previous);
}

// The reference: one CRC over a genuinely all-zero buffer.
uint32_t crc32_fast(const void *data, size_t length, uint32_t previousCrc32 = 0);

int main(void) {
	// 0 and the values around 4096 are the interesting ones: they catch an
	// off-by-one in the chunk boundary.
	static const size_t lens[] = { 1, 2, 255, 4095, 4096, 4097, 8191, 8192, 8193,
	                               65536, 262144, 1048576, 0 };
	static const uint32_t seeds[] = { 0u, 0xFFFFFFFFu, 0x12345678u };
	const size_t maxlen = 1048576;
	unsigned char *zeros = (unsigned char *)calloc(maxlen, 1);
	int fails = 0;
	int cases = 0;
	int s, i;

	if (!zeros) {
		printf("could not allocate the reference buffer\n");
		return 1;
	}

	for (s = 0; s < (int)(sizeof(seeds) / sizeof(seeds[0])); s++) {
		for (i = 0; lens[i]; i++) {
			size_t n = lens[i];
			uint32_t got = emu_crc32_zeroes(n, seeds[s]);
			uint32_t want = crc32_fast(zeros, n, seeds[s]);

			cases++;
			if (got != want) {
				printf("MISMATCH len=%zu seed=0x%08X got=0x%08X want=0x%08X\n",
					n, seeds[s], got, want);
				fails++;
			}
		}
		// A zero length fold must be the identity.
		cases++;
		if (emu_crc32_zeroes(0, seeds[s]) != seeds[s]) {
			printf("MISMATCH len=0 must return the previous CRC\n");
			fails++;
		}
	}

	free(zeros);

	if (fails) {
		printf("FAILED: %d of %d cases\n", fails, cases);
		return 1;
	}
	printf("ok: %d cases, chunked emu_crc32_zeroes matches the single shot CRC\n", cases);
	return 0;
}
