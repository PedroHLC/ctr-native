#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80030aa8-0x80030ad4.
void INSTANCE_Death(struct Instance *inst)
{
	JitPool_Remove(&sdata->gGT->JitPools.instance, (struct Item *)inst);
}
