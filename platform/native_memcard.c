#include <platform/native_memcard.h>

#include <stdio.h>
#include <string.h>

static const char *NativeMemcard_PathFromDeviceName(const char *save_name)
{
	if (strncmp(save_name, "bu00:", 5) == 0)
		return save_name + 5;

	if (strncmp(save_name, "bu01:", 5) == 0)
		return save_name + 5;

	return save_name;
}

int NativeMemcard_FileExists(const char *save_name)
{
	const char *path = NativeMemcard_PathFromDeviceName(save_name);
	FILE *file = fopen(path, "rb");

	if (file == NULL)
		return 0;

	fclose(file);
	return 1;
}

enum NativeMemcardResult NativeMemcard_RemoveFile(const char *save_name)
{
	const char *path = NativeMemcard_PathFromDeviceName(save_name);

	return remove(path) == 0 ? NATIVE_MEMCARD_OK : NATIVE_MEMCARD_NOT_FOUND;
}

enum NativeMemcardResult NativeMemcard_ReadSaveData(const char *save_name, unsigned char *dst, int byte_count, int data_offset)
{
	const char *path = NativeMemcard_PathFromDeviceName(save_name);
	FILE *file;
	size_t read_bytes;

	file = fopen(path, "rb");
	if (file == NULL)
		return NATIVE_MEMCARD_NOT_FOUND;

	if (fseek(file, data_offset, SEEK_SET) != 0)
	{
		fclose(file);
		return NATIVE_MEMCARD_IO_ERROR;
	}

	read_bytes = fread(dst, 1, byte_count, file);
	fclose(file);

	return read_bytes == (size_t)byte_count ? NATIVE_MEMCARD_OK : NATIVE_MEMCARD_IO_ERROR;
}

enum NativeMemcardResult NativeMemcard_WriteSaveData(const char *save_name, const void *icon, int icon_byte_count, const unsigned char *src, int byte_count)
{
	const char *path = NativeMemcard_PathFromDeviceName(save_name);
	FILE *file;
	size_t wrote_icon;
	size_t wrote_data;

	file = fopen(path, "wb");
	if (file == NULL)
		return NATIVE_MEMCARD_OPEN_FAILED;

	wrote_icon = fwrite(icon, 1, icon_byte_count, file);
	wrote_data = fwrite(src, 1, byte_count, file);
	fclose(file);

	if ((wrote_icon != (size_t)icon_byte_count) || (wrote_data != (size_t)byte_count))
		return NATIVE_MEMCARD_IO_ERROR;

	return NATIVE_MEMCARD_OK;
}
