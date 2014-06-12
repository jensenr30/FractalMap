#include "sprites.h"
#include "graphics.h"

void printline(SDL_Surface *screen, int xstart, int ystart, int xend, int yend, int pixel) {
	set_pixel(screen, xstart, ystart, 0xff00|0xff000000);

	set_pixel(screen, xend, yend, 0xff00|0xff000000);
}

