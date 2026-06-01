#include <common.h>
#include <platform/native_memcard.h>

static u8 s_memcardNativeInfoSeen[2];

// NOTE(aalhendi): ctr-native adapts host-backed card operations here; the
// retail implementations stay in their numbered MEMCARD files for non-native
// builds.
void MEMCARD_GetFreeBytes(int slotIdx)
{
	(void)slotIdx;
	sdata->memoryCard_SizeRemaining = 0x1e000;
}

u8 MEMCARD_GetInfo(int slotIdx)
{
	// NOTE(aalhendi): Native treats the host save directory as an inserted card;
	// PSX updates free space while handling the async info event that native skips.
	MEMCARD_GetFreeBytes(slotIdx);

	// NOTE(aalhendi): Report the host directory as a new card once; repeated
	// NEWCARD results make RefreshCard reload the profile forever.
	if (s_memcardNativeInfoSeen[slotIdx & 1] == 0)
	{
		s_memcardNativeInfoSeen[slotIdx & 1] = 1;
		return MC_RETURN_NEWCARD;
	}

	return MC_RETURN_IOE;
}

u8 MEMCARD_Format(int slotIdx)
{
	(void)slotIdx;
	return MC_RETURN_IOE;
}

int MEMCARD_IsFile(int slotIdx, char *save_name)
{
	(void)slotIdx;

	return NativeMemcard_FileExists(save_name) ? MC_RETURN_IOE : MC_RETURN_NODATA;
}

char *MEMCARD_FindFirstGhost(int slotIdx, char *srcString)
{
	(void)slotIdx;
	(void)srcString;
	return NULL;
}

char *MEMCARD_FindNextGhost(void)
{
	return NULL;
}

u8 MEMCARD_EraseFile(int slotIdx, char *srcString)
{
	(void)slotIdx;
	return NativeMemcard_RemoveFile(srcString) == NATIVE_MEMCARD_OK ? MC_RETURN_IOE : MC_RETURN_NODATA;
}

int MEMCARD_HandleEvent(void)
{
	// Native MEMCARD operations complete synchronously in this adapter. If the
	// retail polling path reaches here, report a timeout instead of touching PSX
	// card event APIs.
	return MC_RETURN_TIMEOUT;
}

u8 MEMCARD_Load(int slotIdx, char *name, u8 *ptrMemcard, int memcardFileSize, u32 loadFlags)
{
	enum NativeMemcardResult nativeResult;
	int checksumResult;

	(void)slotIdx;
	(void)loadFlags;

	nativeResult = NativeMemcard_ReadSaveData(name, ptrMemcard, memcardFileSize, 0x100);
	if (nativeResult == NATIVE_MEMCARD_NOT_FOUND)
		return MC_RETURN_NODATA;

	if (nativeResult != NATIVE_MEMCARD_OK)
		return MC_RETURN_TIMEOUT;

	sdata->crc16_checkpoint_byteIndex = 0;
	sdata->crc16_checkpoint_status = 0;
	do
	{
		checksumResult = MEMCARD_ChecksumLoad(ptrMemcard, memcardFileSize);
	} while (checksumResult == MC_RETURN_PENDING);

	return checksumResult == MC_RETURN_IOE ? MC_RETURN_IOE : MC_RETURN_TIMEOUT;
}

u8 MEMCARD_Save(int slotIdx, char *name, char *icon, u8 *ptrMemcard, int memcardFileSize, u32 saveFlags)
{
	enum NativeMemcardResult nativeResult;

	(void)slotIdx;
	(void)icon;
	(void)saveFlags;

	sdata->crc16_checkpoint_byteIndex = 0;
	sdata->crc16_checkpoint_status = 0;
	MEMCARD_ChecksumSave(ptrMemcard, memcardFileSize);

	nativeResult = NativeMemcard_WriteSaveData(name, &data.memcardIcon_Ghost[0], 0x100, ptrMemcard, memcardFileSize);
	if (nativeResult == NATIVE_MEMCARD_OPEN_FAILED)
		return MC_RETURN_FULL;

	if (nativeResult != NATIVE_MEMCARD_OK)
		return MC_RETURN_TIMEOUT;

	return MC_RETURN_IOE;
}
