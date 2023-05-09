#include "PortableExecutable.h"

#include <iostream>
#include <stdio.h>

// TODO do error handling (wrong magic etc)
SISTATUS PortableExecutable::DoLoad() {
	pDosHeader = (PIMAGE_DOS_HEADER) pBase;
	pNtHeaders = (PIMAGE_NT_HEADERS) pBase + pDosHeader->e_lfanew;
	pFileHeader = &pNtHeaders->FileHeader;
	pOptHeader = &pNtHeaders->OptionalHeader;
	return SISTATUS::SUCCESS;
}

PortableExecutable::~PortableExecutable() {
	Reset();
}

//TODO more error handling (size etc.)
SIAPI SISTATUS PortableExecutable::LoadFromFile(PCTCH szFilePath) {
	hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_MODE, NULL, OPEN_EXISTING, NULL, NULL);
	RETURN_IF_INVALID(hFile, SISTATUS::FILE_OPEN_FAILED);
	hFileMapping = CreateFileMapping(hFile, NULL, SEC_IMAGE | PAGE_READONLY, NULL, NULL, NULL);
	RETURN_IF_NULL(hFileMapping, SISTATUS::IMAGE_MAPPING_FAILED);
	pBase = (PBYTE) MapViewOfFile(hFileMapping, FILE_MAP_READ, NULL, NULL, NULL);
	RETURN_IF_NULL(pBase, SISTATUS::IMAGE_MAPPING_FAILED);
	return DoLoad();
}

//TODO error handling (size etc.)
SIAPI SISTATUS PortableExecutable::LoadFromData(PVOID pData) {
	pBase = (PBYTE) pData;
	return DoLoad();
}

SIAPI SISTATUS PortableExecutable::Reset() {
	UnmapViewOfFile(pBase);
	CloseHandle(hFileMapping);
	CloseHandle(hFile);
	pDosHeader = nullptr;
	pNtHeaders = nullptr;
	pFileHeader = nullptr;
	pOptHeader = nullptr;
	return SISTATUS::SUCCESS;
}

std::shared_ptr<PortableExecutable> PortableExecutable::CreateFromFile(PCTCH szFilePath) {
	PPortableExecutable pPE = new PortableExecutable();
	if (pPE->LoadFromFile(szFilePath) != SISTATUS::SUCCESS) {
		delete pPE;
		return std::shared_ptr<PortableExecutable>(nullptr);
	}
	return std::shared_ptr<PortableExecutable>(pPE);
}

std::shared_ptr<PortableExecutable> PortableExecutable::CreateFromData(PVOID pData) {
	PPortableExecutable pPE = new PortableExecutable();
	if (pPE->LoadFromData(pData) != SISTATUS::SUCCESS) {
		delete pPE;
		return std::shared_ptr<PortableExecutable>(nullptr);
	}
	return std::shared_ptr<PortableExecutable>(pPE);
}
