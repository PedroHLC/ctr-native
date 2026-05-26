#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800469dc-0x800469f0.
int RECTMENU_BoolHidden(struct RectMenu *m)
{
	return ((m->state & NEEDS_TO_CLOSE) != 0);
}
