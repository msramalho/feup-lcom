#include <minix/syslib.h>
#include <minix/drivers.h>

#include "sprite.h"

#include "video_gr.h"
/*
typedef struct {
  int x,y;             //< current sprite position
  int width, height;   //< sprite dimensions
  int xspeed, yspeed;  //< current speeds in the x and y direction
  char *map;           //< the sprite pixmap (use read_xpm())
} Sprite;
*/






Sprite * create_sprite(char *pic[], unsigned int x, unsigned int y){
	Sprite *s = (Sprite *) malloc(sizeof(Sprite));
	s->x = x;
	s->y = y;
	s->xspeed = s->yspeed = 0;
	s->map = (char * ) read_xpm(pic, &(s->width), &(s->height));

	if(vg_pixmap(x, y, s->map, s->width, s->height) != 0){
		return NULL;
	}
	return s;
}


int animate_sprite(Sprite *fig, char *base){
	//todo if necessary
}

void destroy_sprite(Sprite *fig, char *base){
	//todo if necessary
}

void move_cursor(Sprite *fig, int xstep, int ystep, char *base){
	//todo if necessary
}
