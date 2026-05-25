#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80030fdc-0x8003105c.
void JitPool_Clear(struct JitPool *AP)
{
	int loopIndex;
	uintptr_t currSlot;

	currSlot = (uintptr_t)AP->ptrPoolData;

	// clear list of free and taken
	LIST_Clear(&AP->free);
	LIST_Clear(&AP->taken);

	for (loopIndex = 0; loopIndex < AP->maxItems; loopIndex++)
	{
		// add all pool items to the free list
		LIST_AddFront(&AP->free, (struct Item *)currSlot);

		// same as & 0xfffffffc, but more optimal,
		// this is aligning down, not up
		currSlot += ((AP->itemSize) >> 2) << 2;
	}
}
