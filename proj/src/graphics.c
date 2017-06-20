#include "graphics.h"
#include "lmlib.h"
#include "macros.h"

/* Private global variables */

static char *video_mem; /* Process address to which VRAM is mapped */
static char *temp_video_mem;

static unsigned h_res; /* Horizontal screen resolution in pixels */
static unsigned v_res; /* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {
	mmap_t map;
	if (lm_alloc(sizeof(vbe_mode_info_t), &map) == NULL) {
		printf("lm_alloc failed!\n");
		return 1;
	}

	struct reg86u reg86;
	reg86.u.b.intno = VBE_BIOS_V_SERV;
	reg86.u.w.ax = VBE_MODE_INFO;
	reg86.u.w.cx = mode;
	reg86.u.w.es = PB2BASE(map.phys);
	reg86.u.w.di = PB2OFF(map.phys);

	if (sys_int86(&reg86) != OK) {
		printf("sys_int86 failed!\n");
		lm_free(&map);
		return 1;
	}
	if (reg86.u.b.ah != 0) {
		printf("Error (AH)\n");
		lm_free(&map);
		return 1;
	}

	*vmi_p = *((vbe_mode_info_t*) map.virtual);
	lm_free(&map);

	return 0;
}

int vg_exit() {
	printf("vg_exit() called \n");
	free(temp_video_mem); //remove allocated memory at the end of the program5.sh

	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */
	reg86.u.b.ah = 0x00; /* Set Video Mode function */
	reg86.u.b.al = 0x03; /* 80x25 text mode*/

	if (sys_int86(&reg86) != OK) {
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	} else
		return 0;
}

void * vg_init(unsigned short mode, unsigned int *width, unsigned int *height) {
	vbe_mode_info_t mode_info;
	if (vbe_get_mode_info(mode, &mode_info)) {
		printf("vbe_get_mode_info failed!\n");
		return;
	}

	/*Allow memory mapping*/
	*width = h_res = mode_info.XResolution;
	*height = v_res = mode_info.YResolution;
	bits_per_pixel = mode_info.BitsPerPixel;

	int r;
	struct mem_range mr;
	unsigned int vram_size = (h_res * v_res * bits_per_pixel / 8); /*VRAM’s size, but you can use the frame-buffer size, instead*/
	printf("h_res * v_res = %d   vram_size: %d \n", (h_res * v_res), vram_size);
	vg_reg86(mode);

	mr.mr_base = mode_info.PhysBasePtr;
	mr.mr_limit = mr.mr_base + vram_size;

	if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
		panic("sys_privctl (ADD_MEM) failed: %d\n", r);
	}

	/*Map memory*/

	video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);
	if (video_mem == MAP_FAILED) {
		panic("couldn’t map video memory");
	}
	temp_video_mem = (char *) malloc(vram_size * sizeof(char));

	return video_mem;
}

int vg_reg86(unsigned long mode) {
	struct reg86u reg86;
	reg86.u.b.intno = 0x10;
	reg86.u.w.ax = 0x4F02;
	reg86.u.w.bx = 1 << 14 | mode;

	if (sys_int86(&reg86) != OK) {
		printf("vg_enter(): sys_int86 call failed \n");
		return -1;
	}
	if (reg86.u.b.ah != 0) {
		printf("Error: calling vg_exit(). \n");
		vg_exit();
		return -1;
	}
	return 0;

}

int vg_pixel(unsigned short x, unsigned short y, unsigned long color) {
	char c = (char) color;
	if (x >= 0 && y >= 0 && x < h_res && y < v_res) {
		(*(temp_video_mem + (y * h_res + x))) = c; //navigate the one dimensionally mapped array
		return 0;
	}
	return 1;
}

int vg_apply() {
	if (temp_video_mem != NULL) {
		memcpy(video_mem, temp_video_mem, (bits_per_pixel * h_res * v_res / 8));
		return 0;
	}
	return 1;
}

int vg_apply_frame(char * frame) {
	if (frame != NULL) {
		memcpy(video_mem, frame, (bits_per_pixel * h_res * v_res / 8));
		return 0;
	}
	return 1;
}
