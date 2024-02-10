#include <array>
#include <cstdint>
#include <map>

typedef uint8_t Color;
class Screen {
	Screen();
	static const int width, height;
	void vSync();
	std::array<Color, 240*320> buffer; // screen resolution should be 320x240, preferably 96dpi
	void write(Color *buf, int x, int y, int w, int h);
	void drawLine(int x1, int y1, int x2, int y2, Color color);
	void drawBox(int x1, int y1, int x2, int y2, Color outline);
	void drawFilledBox(int x1, int y1, int x2, int y2, Color fill);
	void drawFilledBox(int x1, int y1, int x2, int y2, Color fill, Color outline);
	void drawCircle(int x, int y, int radius);
	void drawEllipse(int x1, int y1, int x2, int y2);
};

