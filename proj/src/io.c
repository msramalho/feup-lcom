#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "io.h"

/**
 * https://stackoverflow.com/questions/2156572/c-header-file-with-bitmapped-fonts
 * http://stackoverflow.com/a/23130671
 */
static unsigned char letters[95][13] = {
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00 }, // space :32
		{ 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18,
				0x18, 0x18 }, // ! :33
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x36,
				0x36, 0x36 }, { 0x00, 0x00, 0x00, 0x66, 0x66, 0xff, 0x66, 0x66,
				0xff, 0x66, 0x66, 0x00, 0x00 }, { 0x00, 0x00, 0x18, 0x7e, 0xff,
				0x1b, 0x1f, 0x7e, 0xf8, 0xd8, 0xff, 0x7e, 0x18 },
		{ 0x00, 0x00, 0x0e, 0x1b, 0xdb, 0x6e, 0x30, 0x18, 0x0c, 0x76, 0xdb,
				0xd8, 0x70 }, { 0x00, 0x00, 0x7f, 0xc6, 0xcf, 0xd8, 0x70, 0x70,
				0xd8, 0xcc, 0xcc, 0x6c, 0x38 }, { 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x18, 0x1c, 0x0c, 0x0e },
		{ 0x00, 0x00, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
				0x18, 0x0c }, { 0x00, 0x00, 0x30, 0x18, 0x0c, 0x0c, 0x0c, 0x0c,
				0x0c, 0x0c, 0x0c, 0x18, 0x30 }, { 0x00, 0x00, 0x00, 0x00, 0x99,
				0x5a, 0x3c, 0xff, 0x3c, 0x5a, 0x99, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0xff, 0xff, 0x18, 0x18, 0x18,
				0x00, 0x00 }, { 0x00, 0x00, 0x30, 0x18, 0x1c, 0x1c, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00 }, { 0x00, 0x60, 0x60, 0x30, 0x30, 0x18, 0x18, 0x0c,
				0x0c, 0x06, 0x06, 0x03, 0x03 }, { 0x00, 0x00, 0x3c, 0x66, 0xc3,
				0xe3, 0xf3, 0xdb, 0xcf, 0xc7, 0xc3, 0x66, 0x3c },
		{ 0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78,
				0x38, 0x18 }, { 0x00, 0x00, 0xff, 0xc0, 0xc0, 0x60, 0x30, 0x18,
				0x0c, 0x06, 0x03, 0xe7, 0x7e }, { 0x00, 0x00, 0x7e, 0xe7, 0x03,
				0x03, 0x07, 0x7e, 0x07, 0x03, 0x03, 0xe7, 0x7e },
		{ 0x00, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0xff, 0xcc, 0x6c, 0x3c,
				0x1c, 0x0c }, { 0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0xfe,
				0xc0, 0xc0, 0xc0, 0xc0, 0xff }, { 0x00, 0x00, 0x7e, 0xe7, 0xc3,
				0xc3, 0xc7, 0xfe, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e },
		{ 0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x03,
				0x03, 0xff }, { 0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e,
				0xe7, 0xc3, 0xc3, 0xe7, 0x7e }, { 0x00, 0x00, 0x7e, 0xe7, 0x03,
				0x03, 0x03, 0x7f, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e },
		{ 0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00,
				0x00, 0x00 }, { 0x00, 0x00, 0x30, 0x18, 0x1c, 0x1c, 0x00, 0x00,
				0x1c, 0x1c, 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x06, 0x0c, 0x18,
				0x30, 0x60, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06 },
		{ 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0x00,
				0x00, 0x00 }, { 0x00, 0x00, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03,
				0x06, 0x0c, 0x18, 0x30, 0x60 }, { 0x00, 0x00, 0x18, 0x00, 0x00,
				0x18, 0x18, 0x0c, 0x06, 0x03, 0xc3, 0xc3, 0x7e },
		{ 0x00, 0x00, 0x3f, 0x60, 0xcf, 0xdb, 0xd3, 0xdd, 0xc3, 0x7e, 0x00,
				0x00, 0x00 }, { 0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3,
				0xc3, 0xc3, 0x66, 0x3c, 0x18 }, { 0x00, 0x00, 0xfe, 0xc7, 0xc3,
				0xc3, 0xc7, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe },
		{ 0x00, 0x00, 0x7e, 0xe7, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
				0xe7, 0x7e }, { 0x00, 0x00, 0xfc, 0xce, 0xc7, 0xc3, 0xc3, 0xc3,
				0xc3, 0xc3, 0xc7, 0xce, 0xfc }, { 0x00, 0x00, 0xff, 0xc0, 0xc0,
				0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xc0, 0xff },
		{ 0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0,
				0xc0, 0xff }, { 0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xcf, 0xc0,
				0xc0, 0xc0, 0xc0, 0xe7, 0x7e }, { 0x00, 0x00, 0xc3, 0xc3, 0xc3,
				0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3 },
		{ 0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
				0x18, 0x7e }, { 0x00, 0x00, 0x7c, 0xee, 0xc6, 0x06, 0x06, 0x06,
				0x06, 0x06, 0x06, 0x06, 0x06 }, { 0x00, 0x00, 0xc3, 0xc6, 0xcc,
				0xd8, 0xf0, 0xe0, 0xf0, 0xd8, 0xcc, 0xc6, 0xc3 },
		{ 0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
				0xc0, 0xc0 }, { 0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
				0xdb, 0xff, 0xff, 0xe7, 0xc3 }, { 0x00, 0x00, 0xc7, 0xc7, 0xcf,
				0xcf, 0xdf, 0xdb, 0xfb, 0xf3, 0xf3, 0xe3, 0xe3 },
		{ 0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
				0xe7, 0x7e }, { 0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe,
				0xc7, 0xc3, 0xc3, 0xc7, 0xfe }, { 0x00, 0x00, 0x3f, 0x6e, 0xdf,
				0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x3c },
		{ 0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xfe, 0xc7, 0xc3, 0xc3,
				0xc7, 0xfe }, { 0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0x7e,
				0xe0, 0xc0, 0xc0, 0xe7, 0x7e }, { 0x00, 0x00, 0x18, 0x18, 0x18,
				0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xff },
		{ 0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
				0xc3, 0xc3 }, { 0x00, 0x00, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3,
				0xc3, 0xc3, 0xc3, 0xc3, 0xc3 }, { 0x00, 0x00, 0xc3, 0xe7, 0xff,
				0xff, 0xdb, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3 },
		{ 0x00, 0x00, 0xc3, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x3c, 0x3c, 0x66,
				0x66, 0xc3 }, { 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
				0x3c, 0x3c, 0x66, 0x66, 0xc3 }, { 0x00, 0x00, 0xff, 0xc0, 0xc0,
				0x60, 0x30, 0x7e, 0x0c, 0x06, 0x03, 0x03, 0xff },
		{ 0x00, 0x00, 0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
				0x30, 0x3c }, { 0x00, 0x03, 0x03, 0x06, 0x06, 0x0c, 0x0c, 0x18,
				0x18, 0x30, 0x30, 0x60, 0x60 }, { 0x00, 0x00, 0x3c, 0x0c, 0x0c,
				0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3c },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x66,
				0x3c, 0x18 }, { 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x18, 0x38, 0x30, 0x70 },
		{ 0x00, 0x00, 0x7f, 0xc3, 0xc3, 0x7f, 0x03, 0xc3, 0x7e, 0x00, 0x00,
				0x00, 0x00 }, { 0x00, 0x00, 0xfe, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe,
				0xc0, 0xc0, 0xc0, 0xc0, 0xc0 }, { 0x00, 0x00, 0x7e, 0xc3, 0xc0,
				0xc0, 0xc0, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x7f, 0xc3, 0xc3, 0xc3, 0xc3, 0x7f, 0x03, 0x03, 0x03,
				0x03, 0x03 }, { 0x00, 0x00, 0x7f, 0xc0, 0xc0, 0xfe, 0xc3, 0xc3,
				0x7e, 0x00, 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x30, 0x30, 0x30,
				0x30, 0x30, 0xfc, 0x30, 0x30, 0x30, 0x33, 0x1e },
		{ 0x7e, 0xc3, 0x03, 0x03, 0x7f, 0xc3, 0xc3, 0xc3, 0x7e, 0x00, 0x00,
				0x00, 0x00 }, { 0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
				0xfe, 0xc0, 0xc0, 0xc0, 0xc0 }, { 0x00, 0x00, 0x18, 0x18, 0x18,
				0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x18, 0x00 },
		{ 0x38, 0x6c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x00,
				0x0c, 0x00 }, { 0x00, 0x00, 0xc6, 0xcc, 0xf8, 0xf0, 0xd8, 0xcc,
				0xc6, 0xc0, 0xc0, 0xc0, 0xc0 }, { 0x00, 0x00, 0x7e, 0x18, 0x18,
				0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78 },
		{ 0x00, 0x00, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xfe, 0x00, 0x00,
				0x00, 0x00 }, { 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6,
				0xfc, 0x00, 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x7c, 0xc6, 0xc6,
				0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00 },
		{ 0xc0, 0xc0, 0xc0, 0xfe, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0x00, 0x00,
				0x00, 0x00 }, { 0x03, 0x03, 0x03, 0x7f, 0xc3, 0xc3, 0xc3, 0xc3,
				0x7f, 0x00, 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0xc0, 0xc0, 0xc0,
				0xc0, 0xc0, 0xe0, 0xfe, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0xfe, 0x03, 0x03, 0x7e, 0xc0, 0xc0, 0x7f, 0x00, 0x00,
				0x00, 0x00 }, { 0x00, 0x00, 0x1c, 0x36, 0x30, 0x30, 0x30, 0x30,
				0xfc, 0x30, 0x30, 0x30, 0x00 }, { 0x00, 0x00, 0x7e, 0xc6, 0xc6,
				0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0x00, 0x00,
				0x00, 0x00 }, { 0x00, 0x00, 0xc3, 0xe7, 0xff, 0xdb, 0xc3, 0xc3,
				0xc3, 0x00, 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0xc3, 0x66, 0x3c,
				0x18, 0x3c, 0x66, 0xc3, 0x00, 0x00, 0x00, 0x00 },
		{ 0xc0, 0x60, 0x60, 0x30, 0x18, 0x3c, 0x66, 0x66, 0xc3, 0x00, 0x00,
				0x00, 0x00 }, { 0x00, 0x00, 0xff, 0x60, 0x30, 0x18, 0x0c, 0x06,
				0xff, 0x00, 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x0f, 0x18, 0x18,
				0x18, 0x38, 0xf0, 0x38, 0x18, 0x18, 0x18, 0x0f },
		{ 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
				0x18, 0x18 }, { 0x00, 0x00, 0xf0, 0x18, 0x18, 0x18, 0x1c, 0x0f,
				0x1c, 0x18, 0x18, 0x18, 0xf0 }, { 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x06, 0x8f, 0xf1, 0x60, 0x00, 0x00, 0x00 } };  // :126
const unsigned char CURSOR[13] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

//--------------------------cursor_t

cursor_t * new_cursor(frame_t * f) {
	cursor_t * c = (cursor_t *) malloc(sizeof(cursor_t));
	c->colX = c->lineY = 0;
	c->f = f;
	c->state = 1;
	c->enabled = 1;
	return c;
}

void delete_cursor(cursor_t * c) {
	delete_frame(c->f);
	free(c);
}

int toggle_cursor(cursor_t * c) {
	if (c->enabled) {
		if (c->state) {
			draw_cursor(c);
		} else {
			fill_frame(c->f, TRANSPARENT);  //clear the frame
		}
		c->state = c->state ? 0 : 1;
	}
	return c->enabled;
}

void draw_cursor(cursor_t * c) {
	fill_frame(c->f, TRANSPARENT);  //clear the frame

	int i, j;
	long offX = 8 + c->colX * 16;
	long offY = 6 + c->lineY * 14;
	for (i = 12; i >= 0; i--) {  //linhas
		unsigned char hex = CURSOR[12 - i];
		for (j = 15; j >= 0; j--) {  //colunas
			if (BIT(15 - j) & hex) {
				frame_pixel(c->f, offX + j, offY + i, c->f->font_color); //black
			} else {
				frame_pixel(c->f, offX + j, offY + i, TRANSPARENT);
			}
		}
	}
}

//--------------------------frame_t

frame_t * new_frame(unsigned int width, unsigned int height,
		unsigned char background, unsigned char font_color) {
	frame_t * f = (frame_t *) malloc(sizeof(frame_t));
	f->width = width;
	f->height = height;
	f->background = background;
	f->font_color = font_color;
	f->frame = (char *) malloc(sizeof(char) * (height * width));  //linear
	fill_frame(f, TRANSPARENT);  //clear the frame
	return f;
}

int frame_pixel(frame_t * f, unsigned short x, unsigned short y,
		unsigned long color) {
	char c = (char) color;
	if (x >= 0 && y >= 0 && x < f->width && y < f->height) {
		(*((f->frame) + (y * f->width + x))) = c; //navigate the one dimensionally mapped array
		return 0;
	}
	return 1;
}

int frame_pixmap(frame_t * f, int xi, int yi, char* pm, int width, int height) { //for sprite.cpp
	unsigned short x, y;
	int i = 0;
	for (y = yi; y < yi + height; y++) {
		for (x = xi; x < xi + width; x++) {
			frame_pixel(f, x, y, pm[i]);
			i++;
		}
	}
	return 0;
}

void delete_frame(frame_t * f) {
	free(f->frame);
	free(f);
}

void frame_apply(frame_t * f) {
	vg_apply_frame(f->frame);
}

int merge_frames(frame_t * result, const frame_t * below, const frame_t * top,
		char transparent) {
	if (result->width != below->width || result->width != top->width
			|| result->height != below->height
			|| result->height != top->height) {
		return 0;
	}
	unsigned int i, j;
	for (i = 0; i < result->height; i++) { //merges two screens into one
		for (j = 0; j < result->width; j++) {
			if (top->frame[i * result->width + j] != transparent) {
				result->frame[i * result->width + j] = top->frame[i
						* result->width + j];
			} else if (below->frame[i * result->width + j] != transparent) {
				result->frame[i * result->width + j] = below->frame[i
						* result->width + j];
			} else {
				result->frame[i * result->width + j] = result->background;
			}
		}
	}
	return 1;
}

void fill_frame(frame_t * f, unsigned int color) {
	char c = (char) color;
	int i, j;
	for (i = 0; i < f->height; i++) {
		for (j = 0; j < f->width; j++) {
			(*(f->frame + (i * f->width + j))) = c;
		}
	}
}

int copy_frame(frame_t * destination, const frame_t * origin) {
	if (destination->width != origin->width
			|| destination->width != origin->width) {
		return 0;
	}
	int i, j;
	for (i = 0; i < destination->height; i++) {
		for (j = 0; j < destination->width; j++) {
			destination->frame[i * destination->width + j] = origin->frame[i
					* origin->width + j];
		}
	}
	return 1;
}
int frame_line(frame_t * f, unsigned short xi, unsigned short yi,
		unsigned short xf, unsigned short yf, unsigned long color) {
	short dx;
	short dy, bres1, bres2; //delta x and delta y
	short movx = -1, movy = -1; //direction to go in x and y
	short corr, direction = 1; //temp and correction variables for the line algorithm
	short temp;

	if (xi > xf) { //guarantee correct x and y values
		temp = xi;
		xi = xf;
		xf = temp;
		temp = yi;
		yi = yf;
		yf = temp;
	}
	if (yi > yf) { //guarantee correct x and y values
		temp = xi;
		xi = xf;
		xf = temp;
		temp = yi;
		yi = yf;
		yf = temp;
	}

	dx = xf - xi;
	dy = yf - yi;

	if (dx < dy) { //iterate over y
		if (dx < 0) {
			direction = -1;
			dx = -dx;
		}

		bres1 = 2 * dx;
		bres2 = 2 * dx - 2 * dy;
		corr = 2 * dx - dy;
		for (; yi <= yf; yi++) {
			frame_pixel(f, xi, yi, color);
			if (corr <= 0) {
				corr += bres1;
			} else {
				corr += bres2;
				xi += direction;
			}
		}
	} else { //iterate over x
		if (dy < 0) {
			direction = -1;
			dy = -dy;
		}

		bres1 = 2 * dy;
		bres2 = 2 * dy - 2 * dx;
		corr = 2 * dy - dx;
		for (; xi <= xf; xi++) {
			frame_pixel(f, xi, yi, color);
			if (corr <= 0) {
				corr += bres1;
			} else {
				corr += bres2;
				yi += direction;
			}
		}
	}

	return 0;
}
//--------------------------screen_t
screen_t * new_screen(unsigned int width, unsigned int height,
		unsigned char background, unsigned char font_color) {
	screen_t * s = (screen_t *) malloc(sizeof(screen_t));
	s->pos = 0;
	s->offset_top = 0;
	s->size = 0;
	s->clipboard = ' ';
	s->f = new_frame(width, height, background, font_color); //frame for the chars
	s->cursor = new_cursor(new_frame(width, height, background, font_color)); //frame for the cursor
	return s;
}

void delete_screen(screen_t * s) {
	delete_frame(s->f);
	delete_cursor(s->cursor);
	free(s->stream);
	free(s);
}

void clear_stream(screen_t * s) {
	if (s->size > 0) {
		free(s->stream);
		s->stream = (char *) malloc(sizeof(char) * 0);
		s->size = 0;
		s->offset_top = 0;
		s->pos = 0;
	}
}

void load_string(screen_t * s, char * string) {
	clear_stream(s);
	const unsigned int chunk_size = 50;	//to optimize realloc
	unsigned int i;

	while (string[i] != '\0') {
		if (i >= s->size) {
			s->size += chunk_size;
			s->stream = realloc(s->stream, sizeof(char) * s->size);
		}
		s->stream[i] = string[i];
		i++;
	}
	s->stream = realloc(s->stream, sizeof(char) * i);
	s->size = i;
	s->pos = s->size;
}

long find_previous_paragraph(screen_t * s, long from_pos) {
	long i = from_pos;
	if (s->stream[i] == '\n') {
		i--;
	}
	while (i >= 0 && s->stream[i] != '\n') {
		i--;
	}
	return i;
}

long find_next_paragraph(screen_t * s, long from_pos) {
	long i = from_pos;
	if (s->stream[i] == '\n') {
		i++;
	}
	return find_next_paragraph_ignore_current(s, i);
}

long find_next_paragraph_ignore_current(screen_t * s, long from_pos) {
	long i = from_pos;
	while (i < s->size && s->stream[i] != '\n') {
		i++;
	}
	return i;
}

void update_pos(screen_t * s, unsigned long direction_key) {
	if (direction_key == KEY_UP) {
		if (s->cursor->lineY == 0 && s->offset_top > 0) {
			s->offset_top--;
			fill_frame(s->f, TRANSPARENT);
		}
		if (s->pos == 0) {
			return;
		}
		long last = find_previous_paragraph(s, s->pos);	//position of the current line's '\'

		if (last == -1) {	//first line
			if (s->pos >= SCREEN_CHARS_HOR - 1) {	//first line is wrapped
				long full_lines = (long) ((s->pos - last) / SCREEN_CHARS_HOR)
						- 1;
				long dist_start_line = s->pos
						- (full_lines + 1) * SCREEN_CHARS_HOR + full_lines;
				s->pos = last + SCREEN_CHARS_HOR * full_lines - full_lines
						+ dist_start_line + 3;	//correction
			} else {
				s->pos = 0;
			}
		} else {	//not the first line
			if ((s->pos - last) >= SCREEN_CHARS_HOR) {//if this line is WRAPPED because it is too long
				s->pos = s->pos - SCREEN_CHARS_HOR + 2;
			} else {
				long sLast = find_previous_paragraph(s, last);//the '\n' of the previous line (second last)
				if ((last - sLast) >= SCREEN_CHARS_HOR - 1) {
					long full_lines = (long) ((last - sLast) / SCREEN_CHARS_HOR);
					long new_pos = sLast + SCREEN_CHARS_HOR * full_lines
							+ s->pos - last - full_lines * 2;
					if (new_pos >= last && sLast >= 0) {
						s->pos = last;
					} else {
						s->pos = new_pos;
					}
				} else if ((sLast + (s->pos - last)) > last) {
					s->pos = last;
				} else {
					s->pos = (sLast + (s->pos - last));
				}
			}
		}
	} else if (direction_key == KEY_LEFT) {
		if (s->pos > 0) {
			s->pos--;
		}
	} else if (direction_key == KEY_RIGHT) {
		if (s->pos < s->size) {
			s->pos++;
		}
	} else if (direction_key == KEY_DOWN) {
		if (s->pos == s->size) {
			return;
		}
		long last = find_previous_paragraph(s, s->pos);	//position of the current line's '\'
		long next = find_next_paragraph_ignore_current(s, s->pos);//position of the next line's '\'
		if (next == s->size) {		//next line
			if ((next - s->pos) >= SCREEN_CHARS_HOR) {	//next line is wrapped
				s->pos += SCREEN_CHARS_HOR - 2;
			} else {
				s->pos = s->size;
			}
		} else {		//not the last line
			if ((next - s->pos) >= SCREEN_CHARS_HOR - 1) {//if this line is WRAPPED because it is too long
				s->pos += SCREEN_CHARS_HOR - 2;
			} else {
				long lines = (long) ((next - last) / SCREEN_CHARS_HOR);
				if ((next - last >= SCREEN_CHARS_HOR)
						&& (long) (s->pos - last - lines * SCREEN_CHARS_HOR
								+ SCREEN_CHARS_HOR + 2 * (lines - 1))
								< (long) SCREEN_CHARS_HOR) {
					s->pos = next;
				} else {
					s->pos = min(find_next_paragraph(s, next),
							next + (s->pos - last) - lines * SCREEN_CHARS_HOR
									+ 2 * lines);
				}
			}
		}
	}
}

void add_char(screen_t * s, char c) {
	s->stream = realloc(s->stream, sizeof(char) * (s->size + 1));

	if (s->pos == s->size) {		//only realloc necessary
		s->stream[s->pos] = c;
	} else {
		int i;
		for (i = s->size; i > s->pos; i--) {
			s->stream[i] = s->stream[i - 1];
		}
		s->stream[s->pos] = c;
	}
	s->pos++;
	s->size++;
}

int delete_char(screen_t * s, KbdAction ka) {
	if (ka == BACKSPACE) {
		if (s->pos > 0 && s->size > 0) {
			unsigned int i;
			for (i = s->pos - 1; i < (s->size - 1); i++) {
				s->stream[i] = s->stream[i + 1];
			}
			s->stream = realloc(s->stream, sizeof(char) * (s->size - 1));
			s->size--;

			if (get_first_shown_pos(s) < s->pos && s->offset_top > 0) {
				s->offset_top--;
			}
			s->pos--;
			return 1;
		}
	} else if (ka == DELETE) {
		if (s->size > 0 && s->pos < s->size) {
			unsigned int i;
			for (i = s->pos; i < (s->size - 1); i++) {
				s->stream[i] = s->stream[i + 1];
			}
			s->stream = realloc(s->stream, sizeof(char) * (s->size - 1));
			s->size--;
			return 1;
		}
	}
	return 0;
}
int get_first_shown_pos(screen_t * s) {
	int i = 0, counter = s->offset_top;
	while (counter > 0 && i < s->size) {		//offset n lines
		if (s->stream[i] == '\n') {
			counter--;
			if (counter == 0) {
				break;
			}
		}
		i++;
	}
	if (i != 0) {		//fix offset
		i--;
	}
	if (s->offset_top > 0 && i > 0 && s->stream[i - 1] != '\n') {
		i = find_previous_paragraph(s, i);
	}

	return i < 0 ? 0 : i;
}

void redraw_screen(screen_t * s) {
	fill_frame(s->f, TRANSPARENT);
	int i, j;

	i = get_first_shown_pos(s);
	if (i > s->pos && s->offset_top > 0) {
		s->offset_top--;
		redraw_screen(s);
		return;
	}

	//i is pointing to after the nth paragraph
	unsigned int lineY = 0, colX = 0;
	unsigned char cursor_has_been_set = 0;
	j = 1;
	if (i == s->pos) {
		cursor_has_been_set = 1;
		s->cursor->lineY = lineY;
		s->cursor->colX = colX;
	}

	for (; i < s->size; i++) {		//percorre as linhas
		if (j % SCREEN_CHARS_HOR == 0) {		//word wrap
			lineY++;
			colX = 1;
			j = 2;
			if (lineY >= MAX_LINES) {			//passed the end of the screen
				if (!cursor_has_been_set && s->cursor->lineY >= 0
						&& s->pos > i) {
					s->offset_top++;
					redraw_screen(s);
				}
				return;
			}
		}
		if ((j + 1) % SCREEN_CHARS_HOR == 0 && i == (s->pos - 1)) {
			cursor_has_been_set = 1;
			s->cursor->lineY = lineY + 1;
			s->cursor->colX = 1;
		}
		if (s->stream[i] == '\n') {			//paragraph
			lineY++;
			colX = 0;
			j = 1;
			if (lineY >= MAX_LINES) {			//passed the end of the screen
				if (!cursor_has_been_set && s->cursor->lineY >= 0
						&& s->pos > i) {
					s->offset_top++;
					redraw_screen(s);
				}
				return;
			}
		} else {
			draw_char(s->f, 8 + colX * 16,			//x
			6 + lineY * 14,			//y
			s->stream[i]);
			colX++;
			j++;
		}
		if (i == (s->pos - 1) && cursor_has_been_set == 0) {//found the cursor
			s->cursor->lineY = lineY;
			s->cursor->colX = colX;
		}
	}
	if (s->pos == 0) {
		s->cursor->colX = 0;
	}
}

void change_cursor_to(screen_t *s, int newLine, int newCol) {
	int i, j;
	i = get_first_shown_pos(s);

	unsigned int lineY = 0, colX = 0, lastCol = 0;
	unsigned char cursor_has_been_set = 0;
	j = 1;
	for (; i < s->size; i++) {			//percorre as linhas
		if (lineY == newLine) {
			if (colX == newCol) {
				s->cursor->lineY = lineY;
				s->cursor->colX = colX;
				s->pos = i;
				return;
			}
			lastCol = colX;
		} else if (lineY > newLine) {
			s->cursor->lineY = lineY;
			s->cursor->colX = lastCol;
			s->pos = i;
			return;
		}

		if (j % SCREEN_CHARS_HOR == 0) {			//word wrap
			lineY++;
			colX = 1;
			j = 2;
			if (lineY >= MAX_LINES) {			//passed the end of the screen
				return;
			}
		}
		if (lineY == newLine) {
			if (colX == newCol) {
				s->cursor->lineY = lineY;
				s->cursor->colX = colX;
				s->pos = i;
				return;
			}
			lastCol = colX;
		} else if (lineY > newLine) {
			s->cursor->lineY = lineY;
			s->cursor->colX = lastCol;
			s->pos = i;
			return;
		}
		if (s->stream[i] == '\n') {			//paragraph
			lineY++;
			colX = 0;
			j = 1;
		} else {
			colX++;
			j++;
		}
	}
}

void draw_char(frame_t * f, unsigned int offX, unsigned int offY, int ascii) {

	if (ascii == 0) {
		ascii = 32;			//'\0' is displayed as ' '
	}
	int i, j;
	for (i = 12; i >= 0; i--) {			//linhas
		unsigned char hex = letters[ascii - 32][12 - i];
		for (j = 15; j >= 0; j--) {			//colunas
			if (BIT(15 - j) & hex) {
				frame_pixel(f, offX + j, offY + i, f->font_color);
			} else {
				frame_pixel(f, offX + j, offY + i, f->background);		//black
			}
		}
	}
}

int screen_string_comp(screen_t * s, char * string, unsigned int string_size) {
	if (s->size != string_size) {
		return 0;
	}
	unsigned int i;
	for (i = 0; i < string_size; i++) {
		if (s->stream[i] != string[i]) {
			return 0;
		}
	}
	return 1;
}

file_t * new_file(char * filename, screen_t * s) {
	file_t * f = (file_t *) malloc(sizeof(file_t));
	f->filename = filename;
	f->screen = s;
	f->saved = 1;
	return f;
}

void delete_file(file_t * f) {
	free(f->filename);
	delete_screen(f->screen);
	free(f);
}

int write_file(file_t * f) {
	if (!f->saved) {
		FILE * fp = fopen(f->filename, "w");
		if (fp == NULL) {
			return 0;
		}
		unsigned long i;
		for (i = 0; i < f->screen->size; i++) {
			fprintf(fp, "%c", f->screen->stream[i]);
		}
		fclose(fp);
		f->saved = 1;
	}
	return 1;
}

int read_file(file_t * f) {
	FILE *fp;
	fp = fopen(f->filename, "r");
	if (fp == NULL) {
		return 0;
	}
	int c;
	while ((c = getc(fp)) != EOF) {
		add_char(f->screen, (char) c);
	}
	fclose(fp);
	return 1;
}

int file_exists(char * filename) {
	struct stat sb;

	if (stat(filename, &sb) == 0 && S_ISDIR(sb.st_mode)) {//the path is a directory and not a file
		return 0;
	}

	FILE *fp;
	fp = fopen(filename, "r");
	if (fp == NULL) {
		return 0;
	}
	fclose(fp);
	return 1;
}

int create_new_file(file_t * f) {
	if (file_exists(f->filename)) {			//if this file already exists
		return -1;
	}

	FILE *fp = fopen(f->filename, "ab+");
	if (fp == NULL) {
		return 0;
	}
	fclose(fp);
	return 1;
}
char * stream_to_cstring(screen_t * s) {
	char * string = (char *) malloc(sizeof(char) * s->size + 1);
	memcpy(string, s->stream, s->size);
	string[s->size] = '\0';
	return string;
}

unsigned char get_next_color(unsigned char color) {
	if ((int) color < 15) {
		color++;
	} else {
		color = 0;
	}
	return color;
}

unsigned char get_previous_color(unsigned char color) {
	if ((int) color > 0) {
		color--;
	} else {
		color = 15;
	}
	return color;
}

message_t * new_message(unsigned int width, unsigned int height,
		unsigned char background, unsigned char font_color,
		unsigned int duration, unsigned int duration_vibrate, char * message) {
	message_t * m = (message_t *) malloc(sizeof(message_t));
	m->original_duration = m->duration = duration;
	m->original_vibration = m->duration_vibrate = duration_vibrate;
	m->message = message;
	m->state = 0;
	m->f = new_frame(width, height, background, font_color);//frame for the chars
	return m;
}

void delete_message(message_t * m) {
	delete_frame(m->f);
	free(m);
}

void reset_message(message_t * m, char * message) {
	m->message = message;
	m->duration = m->original_duration;
	m->duration_vibrate = m->original_vibration;
}

int tick_message(message_t *m) {
	if (m->duration > 0) {
		m->duration--;
		if (m->duration == 0) {
			hide_message(m);
			return 0;
		} else {
			if (m->duration_vibrate > 0) {
				m->duration_vibrate--;
				m->state++;
				m->state %= 2;
			} else {
				m->state = 0;
			}

			fill_frame(m->f, TRANSPARENT);
			draw_message(m);
		}
	}
	return 1;
}

void hide_message(message_t *m) {
	m->duration = m->duration_vibrate = 0;
	fill_frame(m->f, TRANSPARENT);
}

void draw_message(message_t * m) {
	int size = cstring_size(m->message);
	m->state *= 6;			//larger vibration

	//fill the square with background
	int i, j;
	for (i = (SCREEN_CHARS_HOR - size) * 16 - 8 + m->state;
			i < (SCREEN_CHARS_HOR) * 16 + m->state; i++) {
		for (j = 6; j < 36; j++) {
			frame_pixel(m->f, i, j, m->f->background);
		}
	}

	//draw the letters
	i = size - 1;
	int right_offset = SCREEN_CHARS_HOR - 1;
	while (i >= 0) {
		right_offset--;
		draw_char(m->f, 8 + right_offset * 16 + m->state,			//x
		6 + 10,			//y is always the first line
		m->message[i]);
		i--;
	}

	//draw the margins
	frame_line(m->f, (SCREEN_CHARS_HOR - size) * 16 - 8 + m->state, 6,
			(SCREEN_CHARS_HOR) * 16 + m->state, 6, m->f->font_color);
	frame_line(m->f, (SCREEN_CHARS_HOR - size) * 16 - 8 + m->state, 36,
			(SCREEN_CHARS_HOR) * 16 + m->state, 36, m->f->font_color);
	frame_line(m->f, (SCREEN_CHARS_HOR - size) * 16 - 8 + m->state, 6,
			(SCREEN_CHARS_HOR - size) * 16 - 8 + m->state, 36,
			m->f->font_color);
	frame_line(m->f, (SCREEN_CHARS_HOR) * 16 + m->state, 6,
			(SCREEN_CHARS_HOR) * 16 + m->state, 36, m->f->font_color);

	m->state /= 6;			//go back to orignal value
}

int cstring_size(char * string) {
	int i = 0;
	while (string[i] != '\0') {
		i++;
	}
	return i;
}

