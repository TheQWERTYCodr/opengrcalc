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
struct CRC32
{
	constexpr static int maxSlices = 256;
	static uint32_t table[maxSlices][256]; // <maxSlices> KB of ROM (maybe RAM, hopefully not) is used by the table
	inline static void generate_table() {
		uint32_t polynomial = 0xEDB88320;
		table[0][0]=0;
		int i = 128;
		int k = 256;
		uint32_t crc = 1;
		while (i>0) {
			if (crc&1) crc = (crc>>1)^polynomial;
			else crc >>=1;
			for (int j = 0; j < 256; j+=k)
				table[0][j|i]=crc^table[0][j];
			i>>=1;
			k>>=1;
		}
		for (int j = 1; j < maxSlices; ++j) {
			for (int i = 0; i < 256; ++i) {
				uint32_t a = table[j-1][i];
				table[j][i] = (a >> 8) ^ table[0][a&0xFF];
			}
		}
	}
	template<int n = 1>
	inline static uint32_t compute(const void *buf, size_t len)
	{
		CRC32 x;
		x.update<n>(buf,len);
		return x.value;
	}

private:
	uint32_t c;
public:
	struct fakevalue { // fake value which hides a conversion to the real value
		uint32_t &x;
		fakevalue(uint32_t &x) : x(x) {}
		inline operator uint32_t() {
			return x ^ 0xFFFFFFFF;
		}
		inline void operator=(uint32_t y) {
			x = y ^ 0xFFFFFFFF;
		}
	} value {c};
	CRC32();
	template<int n> uint32_t update(const void*, size_t);
	template<> uint32_t update<1>(const void *in, size_t len);
	template<> uint32_t update<2>(const void *in, size_t len);
	template<> uint32_t update<3>(const void *in, size_t len);
	template<> uint32_t update<4>(const void *in, size_t len);
	template<int n> requires (n>4)&&(n<=maxSlices) uint32_t update(const void *in, size_t len);
	inline void init() {c = 0xFFFFFFFF;}
};