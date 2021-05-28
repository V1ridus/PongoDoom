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
    unsigned center_offset_y = (gWidth % (scale * SCREENHEIGHT)) / 2;
    unsigned center_offset_x = (gHeight % (scale * SCREENWIDTH)) / 2;

    center_offset = center_offset_y + (center_offset_x * gWidth);

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

void I_FinishUpdate (void)
{
    for (int gy=0; gy<SCREENHEIGHT; gy++)
    {
        for (int gx=0; gx<SCREENWIDTH; gx++)
        {         
           for (int s_y = 0; s_y < scale; s_y++) {
               for (int s_x = 0; s_x < scale; s_x++) {
                   size_t offset = center_offset + (gWidth * (scale * gx + s_x)) + ((SCREENHEIGHT - gy - 1) * scale + s_y);

                   gFramebuffer[offset] = colors[*(screens[0] + gy*SCREENWIDTH+gx)].raw;
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
