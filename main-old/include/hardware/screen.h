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

