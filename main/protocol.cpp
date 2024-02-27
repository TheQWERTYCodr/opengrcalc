/*
The ESP32-C6 is the primary processor, the RP2040 is the secondary processor.

The primary processor directly controls:
- the buttons
- the microSD card slot
- the radios (WiFi, Bluetooth, etc.)
- the bulk of the calculator's functions

The secondary processor directly controls:
- the screen
- the USB-C port

The processors are connected over UART. Their communication is described in this document.
*/

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <utility>
#include <vector>

namespace layer0 {
	void read(const uint8_t*, int);
	void write(const uint8_t*, int);
	int resetCounter = 0;
	int antiResetCounter = 0;
	void reset() {
		resetCounter=0;
		antiResetCounter=0;
		throw std::runtime_error("reset"); // easiest way to signal a reset here is to throw an error
	}
	void put(const uint8_t *in, int n) {
		try {
			uint8_t *out = new uint8_t[n+(n>>5)];
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
			write(out,j);
			delete[] out;
		} catch (std::runtime_error e) {
			if (std::strcmp(e.what(),"error")==0) {
				
			}
		}
	}
	void get(const uint8_t *out, int n) {
		uint8_t *in = new uint8_t[n+(n>>5)]; // allocate as much memory as we need to store the input
		int j = n;
		int k = 0;
		uint8_t x;
		for (int i = 0; j > 0; i++) {
			if (i>=k) { // we need more data
				read(in+i,j); // read the minimum data required to reach the end
				k+=j;
			}
			x = *(in+i);
			if (x==0x00) { // if we receive a 0
				resetCounter++; // increment the reset counter
				if (resetCounter>=32) reset(); // we've hit the threshold, send a reset signal to the above layer
				
			} else if ((x==0xFF)&&(resetCounter>=31)) {
				resetCounter=0;
			} else {
				resetCounter=0;
				return x;
			}
		}
	}
	void sendReset() {
		write((const uint8_t*)("\0\0\0\0\0\0\0"),8);
	}
}
namespace layer1 {

}