/*******************************************************************************

Copyright 2024 opengrcalc (opengrcalc@gmail.com)

This file is part of opengrcalc.

opengrcalc is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

opengrcalc is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
opengrcalc. If not, see <https://www.gnu.org/licenses/>.

*******************************************************************************/

#include <cstddef>
#include <cstdint>
#include "crc32.hpp"
uint32_t CRC32::table[maxSlices][256] = {}; // <maxSlices> KB of ROM (maybe RAM, hopefully not) is used by the table

CRC32::CRC32() : c(0xFFFFFFFF) {}
template<> uint32_t CRC32::update<1>(const void *in, size_t len) {
	const uint8_t *buf = static_cast<const uint8_t *>(in);
	for (size_t i = 0; i < len; ++i) {
		c = table[0][(c ^ buf[i]) & 0xFF] ^ (c >> 8);
	}
	return c ^ 0xFFFFFFFF;
}
template<> uint32_t CRC32::update<2>(const void *in, size_t len) {
	const uint8_t *cur = static_cast<const uint8_t *>(in);
	while (len>=2) {
		uint32_t c2 = c;
		c = table[1][*cur++ ^ (c2&0xFF)]; c2>>=8;
		c ^= table[0][*cur++ ^ (c2&0xFF)]; c2>>=8;
		c ^= c2;
		len-=2;
	}
	return update<1>(cur,len);
}
template<> uint32_t CRC32::update<3>(const void *in, size_t len) {
	const uint8_t *cur = static_cast<const uint8_t *>(in);
	while (len>=3) {
		uint32_t c2 = c;
		c = table[2][*cur++ ^ (c2&0xFF)]; c2>>=8;
		c ^= table[1][*cur++ ^ (c2&0xFF)]; c2>>=8;
		c ^= table[0][*cur++ ^ (c2&0xFF)]; c2>>=8;
		c ^= c2;
		len-=3;
	}
	return update<1>(cur,len);
}
template<> uint32_t CRC32::update<4>(const void *in, size_t len) {
	const uint8_t *cur = static_cast<const uint8_t *>(in);
	while (len>=4) {
		uint32_t c2 = c;
		c  = table[3][*cur++ ^ (c2&0xFF)]; c2>>=8;
		c ^= table[2][*cur++ ^ (c2&0xFF)]; c2>>=8;
		c ^= table[1][*cur++ ^ (c2&0xFF)]; c2>>=8;
		c ^= table[0][*cur++ ^ (c2&0xFF)];
		len-=4;
	}
	return update<1>(cur,len);
}
template<int n> requires (n>4)&&(n<=CRC32::maxSlices) uint32_t CRC32::update(const void *in, size_t len) {
	const uint8_t *cur = static_cast<const uint8_t *>(in);
	while (len>=n) {
		uint32_t c2 = c;
		c  = table[n-1][*cur++ ^ (c2&0xFF)]; c2>>=8;
		c ^= table[n-2][*cur++ ^ (c2&0xFF)]; c2>>=8;
		c ^= table[n-3][*cur++ ^ (c2&0xFF)]; c2>>=8;
		c ^= table[n-4][*cur++ ^ (c2&0xFF)];
		for (int i = n-5; i >= 0; --i)
			c ^= table[i][*cur++];
		len-=n;
	}
	return update<1>(cur,len);
}