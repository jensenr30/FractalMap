#include "block.h"
#include "camera.h"
#include "graphics.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string.h>
#include "globals.h"
#include "utilities.h"


/// this returns the pixel data of a pixel at a certain point on a surface (color and alpha in an Uint32)
Uint32 get_pixel(SDL_Surface *surface, int x, int y){
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	
    switch(bpp) {
    case 1:
        return *p;
        break;
	
    case 2:
        return *(Uint16 *)p;
        break;
	
    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;
	
    case 4:
        return *(Uint32 *)p;
        break;
	
    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

/// this sets's a pixel's data (color and alpha)
void set_pixel(SDL_Surface *surf, int x, int y, Uint32 pixel){
    Uint32 *p = (Uint32 *)( surf->pixels + (surf->pitch * y) + x*surf->format->BytesPerPixel );
	*p = pixel;
}


/// this function will draw a rectangle at (x,y) with width, w, and height, h.
// this function is bounded (meaning it will not accidentally draw outside of the surface).
// This function should not crash the program due to segmentation fault errors.
// dest is the destination SDL_Surface that the image will be printed to.
// colorBorder is the color of the border of the rectangle (inside the bounds of x,y w,h)
// colorFill is the color of the rectangle inside the border.
// if doFill is 0, then this function will only print the border
// returns 0 success.
// returns 1 on invalid dest
short draw_rect(SDL_Surface *dest, int x, int y, int w, int h, int borderThickness, Uint32 colorBorder, Uint32 colorFill, char doFill){
	
	// check for a NULL destination surface pointer.
	if(dest == NULL){
		error("draw_rect() was passed an invalid dest pointer. dest = NULL");
		return 1;
	}
	
	// calculate starting/stopping positions
	int i, j, istart = x, jstart = y, istop = x+w, jstop = y+h, temp;
	// swap istart and istop if istop is less than istart
	if(istop < istart){
		temp = istop;
		istop = istart;
		istart = temp;
	}
	// swap jstart and jstop if jstop is less than jstart
	if(jstop < jstart){
		temp = jstop;
		jstop = jstart;
		jstart = temp;
	}
	// bound the 
	if(istart < 0) istart = 0;
	if(istop > dest->w) istop = dest->w;
	if(jstart < 0) jstart = 0;
	if(jstop > dest->h) jstop = dest->h;
	
	// draw the rectangle
	for(i=istart; i<istop; i++){
		for(j=jstart; j<jstop; j++){
			// at every i,j index, figure out if that pixel is part of the border.
			if(abs(i-x) < borderThickness || abs((x+w)-i) < borderThickness || abs(j-y) < borderThickness || abs((y+h)-j) < borderThickness){
				// if it is, set it to the color of the border.
				set_pixel(dest, i, j, colorBorder);
			}
			else{
				// otherwise, set it to the fill color, but only if we are supposed to do so.
				if(doFill)set_pixel(dest, i, j, colorFill);
			}
		}
	}
	return 0;
}



/// returns a pointer to the loaded texture.
SDL_Texture *load_image_to_texture(char *filename){
	
	//attempt to load the image
	SDL_Surface *tempSurface = IMG_Load(filename);
	
	// if the image was not loaded to a surface correctly,
	if(tempSurface == NULL){
		// create an error message to write to file.
		char message[512];
		strcpy(message, "error in load_image(): could not load file to surface \"");
		strcat(message, filename);
		strcat(message, "\"");
		error(message);
		// and return NULL.
		return NULL;
	}
	
	// try to convert the surface into a texture
	SDL_Texture *tempTexture = SDL_CreateTextureFromSurface(myRenderer, tempSurface);
	
	// if the texture was not created from surface correctly,
	if(tempTexture == NULL){
		// write and error message to the error log
		error("error in load_image(): could not create texture from surface");
		// and return NULL.
		return NULL;
	}
	
	//  if the image loaded correctly, return a pointer to the texture that contains it.
	return tempTexture;
}


