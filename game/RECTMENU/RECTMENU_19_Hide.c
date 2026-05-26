#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800469c8-0x800469dc.
void RECTMENU_Hide(struct RectMenu *m)
{
	m->state |= 0x1000;
}
