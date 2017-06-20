#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "lmlib.h"
#include "vbe.h"
#include "video_gr.h"
#include "sprite.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR	0xE0000000
#define H_RES           1024
#define V_RES		  	768
#define BITS_PER_PIXEL	8

/* Private global variables */

static char *video_mem;		/* Process address to which VRAM is mapped */
static char *temp_video_mem;

static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

int vg_exit() {

	printf("vg_exit() called \n");
	free(temp_video_mem);//remove allocated memory at the end of the program5.sh

	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */

	reg86.u.b.ah = 0x00;    /* Set Video Mode function */
	reg86.u.b.al = 0x03;    /* 80x25 text mode*/

	if( sys_int86(&reg86) != OK ) {
	  printf("\tvg_exit(): sys_int86() failed \n");
	  return 1;
	} else
	  return 0;
}

void * vg_init(unsigned short mode){
	vbe_mode_info_t mode_info;
	if (vbe_get_mode_info(mode, &mode_info)) {
		printf("vbe_get_mode_info failed!\n");
		return;
	}

	int r;
	struct mem_range mr;
	//unsigned int vram_base = VRAM_PHYS_ADDR;  /*VRAM’s physical addresss*/
	unsigned int vram_size = (H_RES * V_RES * BITS_PER_PIXEL / 8);  /*VRAM’s size, but you can use the frame-buffer size, instead*/
	//void * video_mem;  /*frame-buffer VM address*/

	/*Allow memory mapping*/

	//for now:
	h_res = mode_info.XResolution;
	v_res = mode_info.YResolution;
	bits_per_pixel = mode_info.BitsPerPixel;


	vg_reg86(mode);

	mr.mr_base = mode_info.PhysBasePtr;
	mr.mr_limit = mr.mr_base + vram_size;

	if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))){
		panic("sys_privctl (ADD_MEM) failed: %d\n", r);
	}

	/*Map memory*/

	video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
	if(video_mem == MAP_FAILED){
		panic("couldn’t map video memory");
	}
	printf("vram_size: %d\n", vram_size);

	temp_video_mem = (char *) malloc(vram_size * sizeof(char));

	printf("First address of VRAM: %s, %d, %X, %lu\n", &video_mem, &video_mem, &video_mem, &video_mem);
	printf("PIXEL 0,0's value is: %d, %x \n", *video_mem, *video_mem);

	return video_mem;
}

int vg_reg86(unsigned long mode){
	struct reg86u reg86;
	reg86.u.b.intno = 0x10;
	reg86.u.w.ax = 0x4F02;
	reg86.u.w.bx = 1<<14|mode;

	if( sys_int86(&reg86) != OK ) {
		printf("vg_enter(): sys_int86 call failed \n");
	    return -1;
	}
	if(reg86.u.b.ah != 0){
		printf("Error: calling vg_exit(). \n");
		vg_exit();
		return -1;
	}
	return 0;

}


int vg_pixel(unsigned short x,unsigned short y, unsigned long color){
	char c = (char) color;
	if(x >= 0 && y >= 0 && x < h_res && y < v_res){
		//printf("PIXELF(%d, %d) color 0x%X or %d: y * h_res + x: %d\n", x, y, color, c, y * h_res + x);
		(*(temp_video_mem + (y * h_res + x))) = c; //navigate the one dimensionally mapped array
		return 0;
	}
	return 1;
}

int vg_line(unsigned short xi,unsigned short yi, unsigned short xf,unsigned short yf, unsigned long color){
	short dx;
	short dy, bres1, bres2; //delta x and delta y
	short movx = -1, movy = -1; //direction to go in x and y
	short corr, direction=1;//temp and correction variables for the line algorithm
	short temp;

	if (xi>xf) {//guarantee correct x and y values
		temp = xi; xi = xf; xf = temp;
		temp = yi; yi = yf; yf = temp;
	}
	if (yi>yf) {//guarantee correct x and y values
		temp = xi; xi = xf; xf = temp;
		temp = yi; yi = yf; yf = temp;
	}

	dx = xf-xi;dy = yf-yi;

	if(dx < dy){//iterate over y
		if (dx<0) {
			direction = -1;
			dx=-dx;
		}

		bres1 = 2*dx;
		bres2 = 2*dx - 2*dy;
		corr = 2*dx - dy;
		for (;yi<=yf; yi++){
			vg_pixel(xi, yi, color);
			if (corr <= 0) {
				corr += bres1;
			}else{
				corr += bres2;
				xi += direction;
			}
		}
	}else{//iterate over x
		if (dy<0) {
			direction = -1;
			dy=-dy;
		}

		bres1 = 2*dy;
		bres2 = 2*dy - 2*dx;
		corr = 2*dy - dx;
		for (;xi<=xf; xi++){
			vg_pixel(xi, yi, color);
			if (corr <= 0) {
				corr += bres1;
			}else{
				corr += bres2;
				yi += direction;
			}
		}
	}

	return 0;
}

int vg_pixmap(int xi, int yi, char* pm, int width, int height){//for sprite.cpp
	unsigned short x, y;
	int i = 0;
	for(y = yi; y < yi + height; y++){
		for(x = xi; x < xi + width; x++){
			vg_pixel(x, y, pm[i]);//could use if statement to check for vg_pixel return but it can be too time consuming
			i++;
		}
	}
	return 0;
}
int vg_move_pixmap(int xi, int yi, char* pm, int width, int height, unsigned long pixels_per_tick, unsigned long ticks, unsigned short hor){
	unsigned short x, y;
	int i = 0;

	if(hor){
		xi += (pixels_per_tick/1000) * (ticks - 1);
	}else{
		yi += (pixels_per_tick/1000) * (ticks - 1);
	}
	//printf("\nCleaning, pixels_per_tick:%d, ticks: %d\n", pixels_per_tick, ticks);
	//printf("width: %d, height: %d\n",width, height);
	//printf("xi: %d, yi: %d\n",xi, yi);
	for(y = yi; y < yi + height; y++){
		for(x = xi; x < xi + width; x++){
			if(pm[i] != 0x0){//if it is black ignore
				vg_pixel(x, y, 0x0);//could use if statement to check for vg_pixel return but it can be too time consuming
			}
			i++;
		}
	}
	i = 0;
	//printf("xi: %d, yi: %d\n",xi, yi);

	if(hor){
		xi += pixels_per_tick/1000;
	}else{
		yi += pixels_per_tick/1000;
	}

	for(y = yi; y < yi + height; y++){
		for(x = xi; x < xi + width; x++){
			//printf("Drawing vg_pixel(%d, %d,  pm[%d])\n", x, y, i);
			vg_pixel(x, y, pm[i]);//could use if statement to check for vg_pixel return but it can be too time consuming
			i++;
		}
	}
	return 0;
}


int vg_apply(){
	if(temp_video_mem != NULL){
		memcpy(video_mem, temp_video_mem, (bits_per_pixel * h_res * v_res / 8));
		return 0;
	}
	return 1;
}





















