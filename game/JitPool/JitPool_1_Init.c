#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x8003105c-0x800310d4.
void JitPool_Init(struct JitPool *AP, int maxItems, int itemSize, char *name)
{
	(void)name;

	memset(AP, 0, sizeof(struct JitPool));
	AP->maxItems = maxItems;
	AP->itemSize = itemSize;
	AP->poolSize = maxItems * itemSize;
	AP->ptrPoolData = MEMPACK_AllocMem(AP->poolSize);
	JitPool_Clear(AP);
}
