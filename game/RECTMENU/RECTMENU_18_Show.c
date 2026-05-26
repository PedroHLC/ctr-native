#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80046990-0x800469c8.
void RECTMENU_Show(struct RectMenu *m)
{
	RECTMENU_ClearInput();

	sdata->ptrActiveMenu = m;

	m->state &= ~NEEDS_TO_CLOSE;
}
