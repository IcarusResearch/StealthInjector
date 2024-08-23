#pragma once

#include "Common.h"

typedef class PortableExecutable {

	static constexpr INT const FILE_SHARE_MODE = FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE;

private:
	PCTCH szName;
	HANDLE hFile;
	HANDLE hFileMapping;
	std::unique_ptr<BYTE> pBackedFile;
	PBYTE pRaw;
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNtHeaders;
	PIMAGE_FILE_HEADER pFileHeader;
	PIMAGE_OPTIONAL_HEADER pOptHeader;

	SIZE_T size;
	SIZE_T headerSize;
	WORD characteristics;
	std::vector<IMAGE_SECTION_HEADER> vecSections;
	std::vector<PIMAGE_TLS_CALLBACK> vecTLSCallbacks;

	SISTATUS DoLoad();

public:
	SIAPI PortableExecutable() = default;
	SIAPI ~PortableExecutable();
	SIAPI PortableExecutable(const PortableExecutable& other) = default;

	SIAPI SISTATUS LoadFromFile(PCTCH szFilePath, PCTCH szName);
	SIAPI SISTATUS LoadFromData(PVOID pData, PCTCH szName);
	SIAPI SISTATUS Reset();

	typedef struct BaseRelocationEntry {
		WORD Offset : 12;
		WORD Type : 4;
	} BaseRelocationEntry, * PBaseRelocationEntry;

	typedef struct BaseRelocationBlock {
		ULONG uPageRVA;
		ULONG uBlockSize;
		BaseRelocationEntry entries[1];
	} BaseRelocationBlock, * PBaseRelocationBlock;

	typedef struct Import {
		std::string szName;
		INT16 ordinal; // TODO validate if 16 bits is correct
	} Import, * PImport;

	PCTCH GetName();
	SIZE_T GetSize();
	SIZE_T GetHeaderSize();
	ULONG_PTR GetRaw();
	ULONG_PTR GetBase();
	WORD GetCharacteristics();
	const std::vector<IMAGE_SECTION_HEADER>& GetSections();
	ULONG_PTR GetDirVA(UINT32 uIndex, ULONG_PTR uRel = 0);
	SIZE_T GetDirSize(UINT32 uIndex);
	ULONG_PTR VAResolveExperimental(ULONG_PTR uRel);
	std::vector<PIMAGE_TLS_CALLBACK>& GetTLSCallbacks();
	ULONG_PTR GetEntryPointRVA();
	const PIMAGE_OPTIONAL_HEADER GetOptionalHeader();
	const PIMAGE_FILE_HEADER GetFileHeader();
	const PIMAGE_NT_HEADERS GetNtHeaders();
	const PIMAGE_DOS_HEADER GetDosHeader();
	std::vector<Import> GetImports();

	static std::shared_ptr<PortableExecutable> CreateFromFile(PCTCH szFilePath, PCTCH szName);
	static std::shared_ptr<PortableExecutable> CreateFromData(PVOID pData, PCTCH szName);

} PortableExecutable, *PPortableExecutable;
 