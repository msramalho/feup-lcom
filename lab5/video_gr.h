#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H

#include "sprite.h"
/** @defgroup video_gr video_gr
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */

/**
 * @brief Initializes the video module in graphics mode
 * 
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen, 
 *  and the number of colors
 * 
 * @param mode 16-bit VBE mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */

typedef struct pixel{
	int x;
	int y;
} pixel_;

 /**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 * 
 * @return 0 upon success, non-zero upon failure
 */
int vg_exit(void);

void * vg_init(unsigned short mode);

int vg_reg86(unsigned long mode);

int vg_pixel(unsigned short x,unsigned short y, unsigned long color);

int vg_pixmap(int xi, int yi, char* pix, int width, int height);

//int vg_move_pixmap(Sprite * s, float pixels_per_tick, unsigned long ticks, unsigned short hor);
/*int vg_move_pixmap(int xi, int yi, char* pm, int width, int height, float pixels_per_tick,
		unsigned long ticks, unsigned short hor);*/
/**
 * Based on Bresenham's line algorithm
 */
int vg_line(unsigned short xi,unsigned short yi, unsigned short xf,unsigned short yf, unsigned long color);

int vg_apply();

/** @} end of video_gr */
 
#endif /* __VIDEO_GR_H */
