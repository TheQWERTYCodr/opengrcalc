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

#include <array>
#include <bitset>
#include <cstdint>
#include <type_traits>
struct Button {
	enum : uint32_t {
		ctrl,
		shift,
		alt,
		modifier_lock,
		menu,
		back,
		
		BTN_COUNT
	};
	operator uint32_t() {
		return (uint32_t)(*this);
	}
};
const unsigned long i = sizeof(Button);
// state of a button, only ever referenced, 
template<uint n>
struct ButtonState {bool active, changed;};
struct ButtonArray {
	static std::bitset<(unsigned int)Button::BTN_COUNT> active;
	static std::bitset<(unsigned int)Button::BTN_COUNT> changed;
	static bool pressed(Button &x);
	static bool released(Button &x);
	static void poll();
	static void poll(Button &hint);
};
static auto btnarray_size = sizeof(ButtonArray);