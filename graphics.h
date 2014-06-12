#include <SDL2/SDL.h>
#include "globals.h"

#define within_screen(x,y) (x>=0 && x<windW && y>=0 && y<windH)
#define VERTICAL 10000.0f
#define HORIZONTAL 0.0001f

Uint32 get_pixel(SDL_Surface *surface, int x, int y);
void set_pixel(SDL_Surface *surf, int x, int y, Uint32 pixel);

SDL_Texture *load_image_to_texture(char *filename);

int intersect_p(float x1, float y1, float slope1, float x2, float y2, float slope2, float *xr, float *yr);
void draw_line(SDL_Surface *dest, float x1, float y1, float x2, float y2, float thickness, unsigned int lineColor);
void draw_circle(SDL_Surface *dest, float x, float y, float radius, Uint32 color);
