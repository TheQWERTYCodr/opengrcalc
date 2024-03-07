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

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <format>
#include <initializer_list>
#include <iostream>
#include <random>
#include <stdexcept>
#include <stdint.h>
#include <unistd.h>
#include <utility>

constexpr int maxSlices = 256;
struct CRC32
{
	static uint32_t table[maxSlices][256]; // <maxSlices> KB of ROM is used by the tables
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
	CRC32() : c(0xFFFFFFFF) {}
	template<int n>
	inline uint32_t update(const void*, size_t);
	template<>
	inline uint32_t update<1>(const void *in, size_t len) {
		const uint8_t *buf = static_cast<const uint8_t *>(in);
		for (size_t i = 0; i < len; ++i) {
			c = table[0][(c ^ buf[i]) & 0xFF] ^ (c >> 8);
		}
		return c ^ 0xFFFFFFFF;
	}
	template<>
	inline uint32_t update<2>(const void *in, size_t len) {
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
	template<>
	inline uint32_t update<3>(const void *in, size_t len) {
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
	template<>
	inline uint32_t update<4>(const void *in, size_t len) {
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
	template<int n> requires (n>4)&&(n<=maxSlices)
	inline uint32_t update(const void *in, size_t len) {
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
	inline void init() {
		c = 0xFFFFFFFF;
	}
};
uint32_t CRC32::table[maxSlices][256] = {};



// usage: the following code generates crc for 2 pieces of data
// uint32_t table[256];
// crc32::generate_table(table);
// uint32_t crc = crc32::update(table, 0, data_piece1, len1);
// crc = crc32::update(table, crc, data_piece2, len2);
// output(crc);
#include <cstdio>
std::minstd_rand0 e;
std::uniform_int_distribution<uint8_t> dist;
volatile uint32_t a;
template<int a> constexpr auto helper2 = std::make_pair(a,&CRC32::compute<a>);
template<int ...a> constexpr auto helper = std::array<std::pair<int, decltype(&CRC32::compute<1>)>,std::size({a...})>{helper2<a>...};
template<int a, int b> std::array<std::pair<int, decltype(&CRC32::compute<1>)>,b-a> helper3;
template<int a> constexpr std::array<std::pair<int, decltype(&CRC32::compute<1>)>,1> helper3<a,a+1> = helper<a>;
template<int a, int b> inline consteval auto helper4() {
	std::array<std::pair<int, decltype(&CRC32::compute<1>)>,(b-a)/2> p1 = helper3<a,(a+b)/2>;
	std::array<std::pair<int, decltype(&CRC32::compute<1>)>,b-((a+b)/2)> p2 = helper3<(a+b)/2,b>;
	std::array<std::pair<int, decltype(&CRC32::compute<1>)>,b-a> p3 {};
	std::copy(p1.begin(),p1.end(),p3.begin());
	std::copy(p2.begin(),p2.end(),p3.begin()+((b-a)/2));
	return p3;
}
template<int a, int b> requires (b>a+1) constexpr auto helper3<a,b> = helper4<a,b>();
__attribute__((weak)) int __llvm_profile_reset_counters(void);
#include "sys/random.h"
const int test = 2;
int main(void) {
	CRC32::generate_table();
	const size_t N = 65536;
	const size_t K = 16;
	std::cerr << "allocating memory...\n";
	uint8_t *buf = new uint8_t[N];
	std::chrono::high_resolution_clock clk;
	std::chrono::high_resolution_clock::time_point t0, t1, t2;
	double tmin,tmax,tsum,v1,v2,tmed1,tmed2,w1,w2;
	void *ptr = buf;
	bool flag;
	if (__llvm_profile_reset_counters) __llvm_profile_reset_counters();
	if constexpr (test==0) {
		for (int outer_iter = 0; outer_iter < 8; ++outer_iter) {
			std::cout << std::format("p_{} = \\left[",outer_iter);
			flag = false;
			for (int iter = 0; iter < 8; ++iter) {
				std::cerr << std::format("iter {}:{} running...\n",outer_iter,iter);
				if (getrandom(buf,N,0)<N) throw std::runtime_error(std::format("failed to get random data"));
				for (auto j : helper3<1,128>) {
					t0 = clk.now();
					for (int q = 0; q < K; ++q) a = j.second(ptr,N);
					t1 = clk.now();
					for (int q = 0; q < K; ++q) a = j.second(ptr,N);
					t2 = clk.now();
					v1 = std::chrono::duration<double>(t1-t0).count()/K;
					v2 = std::chrono::duration<double>(t2-t1).count()/K;
					tmin = std::min(v1,v2);
					tmax = std::max(v1,v2);
					tsum = v1+v2;
					tmed1 = tmin;
					tmed2 = tmax;
					for (int i = 2; i < K; i+=2) {
						t0 = clk.now();
						for (int q = 0; q < K; ++q) a = j.second(ptr,N);
						t1 = clk.now();
						for (int q = 0; q < K; ++q) a = j.second(ptr,N);
						t2 = clk.now();
						v1 = std::chrono::duration<double>(t1-t0).count()/K;
						v2 = std::chrono::duration<double>(t2-t1).count()/K;
						w1 = std::min(v1,v2);
						w2 = std::max(v1,v2);
						tmin = std::min(tmin,w1);
						tmax = std::max(tmax,w2);
						tsum += v1+v2;
						if (tmed2<=w1) {
							tmed1=tmed2;
							tmed2=w1;
						} else if (w2<=tmed1) {
							tmed2=tmed1;
							tmed1=w2;
						} else {
							tmed1=std::max(tmed1,w1);
							tmed2=std::min(tmed2,w2);
						}
					}
					if (flag) std::cout << ',';
					flag=true;
					std::cout << std::format("({0},{1:.16f}),({0},{2:.16f}),({0},{3:.16f}),({0},{4:.16f})",j.first,tmin,tmax,(tmed1+tmed2)/2,tsum/K);
					//std::cout << std::format("iter {}; slice by {:3d}: got {:08X}, took min {:3.8f} max {:3.8f} med {:3.8f} avg {:3.8f} seconds\n",iter,j.first,a,tmin,tmax,(tmed1+tmed2)/2,tsum/K);
				}

			}
			std::cout << "\\right]\n";
		}
	} else if constexpr (test==1){
		for (int i = 0; i < 32; ++i) {
			std::cout << std::format("p_{{{}}} = \\left[",i);
			flag = false;
			for (auto k = 0; k < 32; ++k) {
				std::cerr << std::format("iter {}:{}\n",i,k);
				for (auto j : helper3<1,128>) {
					if (getrandom(buf,N,0)<N) throw std::runtime_error(std::format("failed to get random data"));
					t0 = clk.now();
					for (int k = 0; k < 16; ++k) a=j.second(ptr,N);
					t1 = clk.now();
					v1 = std::chrono::duration<double>(t1-t0).count();
					if (flag) std::cout << ',';
					else flag=true;
					std::cout << std::format("({},{:.16f})",j.first,v1);
				}
			}
			std::cout << "\\right]\n";
		}
	} else {
		if (getrandom(buf,N,0)<N) throw std::runtime_error(std::format("failed to get random data"));
		CRC32 x;
		x.update<16>(buf,N);
		a = x.value;
		std::cout << std::format("{:08X}\n",a);
		uint8_t tmp[4] = {0,0,0,0};
		tmp[0]=a&0xFF; a>>=8;
		tmp[1]=a&0xFF; a>>=8;
		tmp[2]=a&0xFF; a>>=8;
		tmp[3]=a&0xFF;
		x.update<4>(tmp,4);
		std::cout << std::format("{:08X}\n",uint32_t(x.value));
		// the value should now equal 0x2144DF1C
	}
	return 0;
}