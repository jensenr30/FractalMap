#include <SDL2/SDL.h>

Uint32 get_pixel(SDL_Surface *surface, int x, int y);
void set_pixel(SDL_Surface *surf, int x, int y, Uint32 pixel);

SDL_Texture *load_image_to_texture(char *filename);

short draw_rect(SDL_Surface *dest, int x, int y, int w, int h, int borderThickness, Uint32 colorBorder, Uint32 colorFill, char doFill);

// this returns a even mixture of color1 and color2
#define color_mix(color1, color2) ( ((((0xff000000&color1)>>1)+((0xff000000&color2)>>1))&0xff000000) | ((((0xff0000&color1)+(0xff0000&color2))>>1)&0xff0000) | ((((0xff00&color1)+(0xff00&color2))>>1)&0xff00) | ((((0xff&color1)+(0xff&color2))>>1)&0xff) )

short print_camera_view(SDL_Surface *dest, struct cameraData *cam);
