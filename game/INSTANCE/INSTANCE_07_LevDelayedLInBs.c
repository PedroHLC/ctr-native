#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80030ed4-0x80030f58.
void INSTANCE_LevDelayedLInBs(struct InstDef *instDef, int numInstances)
{
	for (int i = 0; i < numInstances; i++)
	{
		struct MetaDataMODEL *meta = COLL_LevModelMeta(instDef->model->id);

		if ((meta != NULL) && (meta->LInB != NULL))
			meta->LInB(instDef->ptrInstance);

		instDef++;
	}
}
