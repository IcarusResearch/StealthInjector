#pragma once

#include "Common.h"

typedef class PortableExecutable {

	static constexpr INT const FILE_SHARE_MODE = FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE;

private:
	HANDLE hFile;
	HANDLE hFileMapping;
	PBYTE pBase;
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNtHeaders;
	PIMAGE_FILE_HEADER pFileHeader;
	PIMAGE_OPTIONAL_HEADER pOptHeader;

	SISTATUS DoLoad();

public:
	SIAPI PortableExecutable() = default;
	SIAPI ~PortableExecutable();
	SIAPI PortableExecutable(const PortableExecutable& other) = default;

	SIAPI SISTATUS LoadFromFile(PCTCH szFilePath);
	SIAPI SISTATUS LoadFromData(PVOID pData);
	SIAPI SISTATUS Reset();

} PortableExecutable, *PPortableExecutable;
