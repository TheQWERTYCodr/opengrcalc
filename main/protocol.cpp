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
#include <any>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <utility>
#include <vector>
#include "crc32.hpp"
struct reset_exception {
	const int layer;
	const char *reason;
};
template<int n>
struct layer {
	void quietReset();
	void reset(const char *reason) {
		quietReset();
		throw reset_exception(n,reason);
	}
};
struct layer0 : public layer<0> {
	void quietReset();
	void read(uint8_t*, int);
	void write(const uint8_t*, int);
};
template<class T>
struct layer1 : public layer<1> {
	T prev;
	int resetCounter = 0;
	int antiResetCounter = 0;
	layer1(T x) : prev(x) {}
	void quietReset(bool propagateDown = true) {
		resetCounter=31; // additional zeros stop communication
		antiResetCounter=31; // avoid sending additional zeros
		if (propagateDown) prev.quietReset();
	}
	void write(const uint8_t *in, const int n) {
		uint8_t *out = new uint8_t[n+(n>>5)]; // place outside try block to avoid problems with memory
		try {
			uint8_t x;
			int j = 0;
			for (int i = 0; i < n; ++i) {
				x = *(in+i);
				if (x==0x00) {
					antiResetCounter++;
					if (antiResetCounter>=32) {
						antiResetCounter=1;
						out[j]=0xFF;
						++j;
					}
					out[j]=x;
					++j;
					continue;
				}
				if ((x==0xFF)&&(antiResetCounter>=31)) {
					out[j]=0xFF;
					++j;
				}
				antiResetCounter=0;
				out[j]=x;
				++j;
			}
			prev.write(out,j);
			delete[] out;
		} catch (reset_exception e) {
			delete[] out;
			quietReset(false);
			throw;
		} catch (std::any x) {
			delete[] out;
			throw;
		}
	}
	void read(uint8_t *out, const int n) {
		uint8_t *in = new uint8_t[n+(n>>5)]; // allocate as much memory as we need to store the input
		try {
			int j = 0;
			int k = 0;
			uint8_t x;
			for (int i = 0; j > 0; i++) {
				if (i>=k) { // we need more data
					prev.read(in+i,n-j); // read the minimum data required to reach the end
					k+=n-j;
				}
				x = *(in+i);
				if (x==0x00) { // if we receive a 0
					resetCounter++; // increment the reset counter
					if (resetCounter>=32) reset("reset counter hit 32"); // we've hit the threshold, reset
					out[j]=0;
					++j;
				} else if ((x==0xFF)&&(resetCounter>=31)) {
					resetCounter=0;
				} else {
					resetCounter=0;
					out[j]=x;
					++j;
				}
			}
			delete[] in;
		} catch (reset_exception e) {
			delete[] in;
			quietReset(false);
			throw;
		} catch (std::any x) {
			delete[] in;
			throw;
		}
	}
	void sendReset() {
		prev.write((const uint8_t*)("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"),48);
		reset("call to sendReset");
	}
};
template<class T>
struct layer2 : public layer<2> {
	T prev;
	layer2(T x) : prev(x) {}
	struct message_t {
		uint16_t size; // messages have a length from 6 to 65542 bytes
		const uint8_t *body;
	};
	uint32_t cksum(const uint8_t *body, int size) {
		return CRC32::compute<4>(body, size);
	}
	void send(message_t x) {
		uint32_t crc = cksum(x.body,x.size);
		
	}
	message_t recv() {
		message_t x;
		uint8_t *header = new uint8_t[6];
		uint32_t id = header[0];
		uint16_t id_ab = (header[0]<<8)|header[1];
	}
}