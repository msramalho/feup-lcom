#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "lmlib.h"

#define LINEAR_MODEL_BIT 14

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {
  mmap_t map;
  if (lm_alloc(sizeof(vbe_mode_info_t), &map)==NULL){
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
  if(reg86.u.b.ah != 0){
	  printf("Error (AH)\n");
	  lm_free(&map);
	  return 1;
  }

  *vmi_p = *((vbe_mode_info_t*) map.virtual);
  lm_free(&map);

  return 0;
}

int vbe_get_ctrl_info(vbe_info_block_t *ctrl_info, unsigned *num_modes, uint16_t **modes_info) {
	mmap_t map;
	if (lm_alloc(sizeof(vbe_mode_info_t), &map)==NULL){
		printf("lm_alloc failed!\n");
		return 1;
	}


	struct reg86u reg86;
	reg86.u.b.intno = VBE_BIOS_V_SERV;
	reg86.u.w.es = PB2BASE(map.phys);
	reg86.u.w.di = PB2OFF(map.phys);
	if (sys_int86(&reg86) != OK) {
		printf("sys_int86 failed!\n");
		lm_free(&map);
		return 1;
	}
	if(reg86.u.b.ah != 0){
		printf("Error (AH)\n");
		lm_free(&map);
		return 1;
	}

	(*ctrl_info) = (*((vbe_info_block_t * ) map.virtual));

	lm_free(&map);
	return 0;
}

void print_vbe_info_block(vbe_info_block_t *ctrl_info){
	printf("VRAM total memory: %lu\n", ctrl_info->total_memory);
}






