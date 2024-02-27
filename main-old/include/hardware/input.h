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
uint32_t x;
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