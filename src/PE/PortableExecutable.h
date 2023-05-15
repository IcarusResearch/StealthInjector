#pragma once

#include "Common.h"

typedef class PortableExecutable {

	static constexpr INT const FILE_SHARE_MODE = FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE;

private:
	PCTCH szName;
	HANDLE hFile;
	HANDLE hFileMapping;
	PBYTE pBase;
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNtHeaders;
	PIMAGE_FILE_HEADER pFileHeader;
	PIMAGE_OPTIONAL_HEADER pOptHeader;

	SIZE_T size;
	SIZE_T headerSize;
	WORD characteristics;
	std::vector<IMAGE_SECTION_HEADER> vecSections;

	SISTATUS DoLoad();

public:
	SIAPI PortableExecutable() = default;
	SIAPI ~PortableExecutable();
	SIAPI PortableExecutable(const PortableExecutable& other) = default;

	SIAPI SISTATUS LoadFromFile(PCTCH szFilePath, PCTCH szName);
	SIAPI SISTATUS LoadFromData(PVOID pData, PCTCH szName);
	SIAPI SISTATUS Reset();

	PCTCH GetName();
	SIZE_T GetSize();
	SIZE_T GetHeaderSize();
	ULONG_PTR GetBase();
	WORD GetCharacteristics();
	ULONG_PTR RelToAbsVA(ULONG_PTR rel);
	const std::vector<IMAGE_SECTION_HEADER>& GetSections();
	ULONG_PTR GetDirVA(UINT32 uIndex);
	SIZE_T GetDirSize(UINT32 uIndex);

	static std::shared_ptr<PortableExecutable> CreateFromFile(PCTCH szFilePath, PCTCH szName);
	static std::shared_ptr<PortableExecutable> CreateFromData(PVOID pData, PCTCH szName);

	typedef struct BaseRelocationEntry {
		WORD Type : 4;
		WORD Offset : 12;
	} BaseRelocationEntry, *PBaseRelocationEntry;

	typedef struct BaseRelocationBlock {
		ULONG uPageRVA;
		ULONG uBlockSize;
		BaseRelocationEntry entries[1];
	} BaseRelocationBlock, *PBaseRelocationBlock;

} PortableExecutable, *PPortableExecutable;
