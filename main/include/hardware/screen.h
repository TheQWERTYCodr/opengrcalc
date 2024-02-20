#include <array>
#include <cstdint>
#include <map>

typedef uint8_t Color;
class Screen { // screen is written to using a table of 256 
	Screen();
	static const int width, height;
	void vSync();
	void write(Color *buf, int x, int y, int w, int h);
	void drawLine(int x1, int y1, int x2, int y2, Color color);
	void drawBox(int x1, int y1, int x2, int y2, Color outline);
	void drawFilledBox(int x1, int y1, int x2, int y2, Color fill);
	void drawFilledBox(int x1, int y1, int x2, int y2, Color fill, Color outline);
	void drawCircle(int x, int y, int radius);
	void drawEllipse(int x1, int y1, int x2, int y2);
	void drawBitmap();
};

