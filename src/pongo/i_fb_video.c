#include "i_video.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include "v_video.h"

#include <pongo.h>

static long int screensize = 0;
static char *fbp = 0;
static uint8_t* gameScreen;

static unsigned scale = 0;
static unsigned center_offset = 0;
static size_t horizontal_width = 0;

void I_InitGraphics (void)
{
    // Displaying horizontally, so flipped 90 degrees
    unsigned scale_w = gHeight / SCREENWIDTH;
    unsigned scale_h = gWidth / SCREENHEIGHT;

    if (scale_w > scale_h) {
        scale = scale_h;
    }
    else {
        scale = scale_w;
    }
    
    unsigned center_offset_y = (gRowPixels % (scale * SCREENHEIGHT)) / 2;
    unsigned center_offset_x = (gHeight % (scale * SCREENWIDTH)) / 2;

    center_offset = center_offset_y + (center_offset_x * gRowPixels);

    // Background black
    memset(gFramebuffer, 0, gHeight * gRowPixels * sizeof(uint32_t));
    cache_clean_and_invalidate(gFramebuffer, gHeight * gRowPixels * sizeof(uint32_t));
}


void I_ShutdownGraphics(void)
{
}

void I_StartFrame (void)
{

}
__attribute__((packed))
struct Color
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
};

union ColorInt
{
    struct Color col;
    uint32_t raw;
};

static union ColorInt colors[256];

// Takes full 8 bit values.
void I_SetPalette (byte* palette)
{
    byte c;
    // set the X colormap entries
    for (int i=0 ; i<256 ; i++)
    {
        c = gammatable[usegamma][*palette++];
        colors[i].col.r = (c<<8) + c;
        c = gammatable[usegamma][*palette++];
        colors[i].col.g = (c<<8) + c;
        c = gammatable[usegamma][*palette++];
        colors[i].col.b = (c<<8) + c;
    }
}

void I_UpdateNoBlit (void)
{

}

uint16_t colorTo16bit(struct Color col)
{
    return  (col.r >> 3) << 11 | (col.g >> 2) << 5 | (col.b >> 3);
}

//TODO, need a proper way to check this, this metric 
// probably wont be an accurate check on most devices :o
int is_argb8888() {
    return gWidth == gRowPixels;
}

// Certain devices i.e. iPhone 7 use ARGB2101010 format instead
// of ARGB8888 i.e. iPhone SE
uint32_t ARGB_8_8_8_8_To_ARGB_2_10_10_10(struct Color col) {
    uint32_t red  = col.r << 2; 
    uint32_t green = col.g << 2;
    uint32_t blue = col.b << 2;

    return red << 20 | green << 10 | blue;
}



void I_FinishUpdate (void)
{
    if (is_argb8888()) {
        for (int gy=0; gy<SCREENHEIGHT; gy++)
        {
            for (int gx=0; gx<SCREENWIDTH; gx++)
            {         
                for (int s_y = 0; s_y < scale; s_y++) {
                    for (int s_x = 0; s_x < scale; s_x++) {
                        size_t offset = center_offset + (gRowPixels * (scale * gx + s_x)) + ((SCREENHEIGHT - gy - 1) * scale + s_y);
                        gFramebuffer[offset] = colors[*(screens[0] + gy*SCREENWIDTH+gx)].raw;
                    }
                }
            }
        }
    }
    else { // ARGB 2-10-10-10?
        for (int gy=0; gy<SCREENHEIGHT; gy++)
        {
            for (int gx=0; gx<SCREENWIDTH; gx++)
            {         
                for (int s_y = 0; s_y < scale; s_y++) {
                    for (int s_x = 0; s_x < scale; s_x++) {
                        size_t offset = center_offset + (gRowPixels * (scale * gx + s_x)) + ((SCREENHEIGHT - gy - 1) * scale + s_y);
                        gFramebuffer[offset] = ARGB_8_8_8_8_To_ARGB_2_10_10_10(colors[*(screens[0] + gy*SCREENWIDTH+gx)].col);
                    }
                }
            }
        }

    }
    cache_clean_and_invalidate(gFramebuffer, gHeight * gRowPixels * sizeof(uint32_t));
}

void I_ReadScreen (byte* scr)
{
    memcpy(scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}
