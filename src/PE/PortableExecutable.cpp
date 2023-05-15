#include "PortableExecutable.h"

#include <iostream>
#include <stdio.h>

// TODO do error handling (wrong magic etc)
SISTATUS PortableExecutable::DoLoad() {
	pDosHeader = (PIMAGE_DOS_HEADER) pBase;
	pNtHeaders = (PIMAGE_NT_HEADERS) (pBase + pDosHeader->e_lfanew);
	pFileHeader = &pNtHeaders->FileHeader;
	pOptHeader = &pNtHeaders->OptionalHeader;
	size = pOptHeader->SizeOfImage;
	headerSize = pOptHeader->SizeOfHeaders;
	characteristics = pOptHeader->DllCharacteristics;
	pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;

	PIMAGE_SECTION_HEADER pCurrSectionHeader = IMAGE_FIRST_SECTION(pNtHeaders);
	for (UINT i = 0; i < pFileHeader->NumberOfSections; ++i) {
		vecSections.push_back(*pCurrSectionHeader);
		++pCurrSectionHeader;
	}

	return SISTATUS::SUCCESS;
}

PortableExecutable::~PortableExecutable() {
	Reset();
}

//TODO more error handling (size etc.)
SIAPI SISTATUS PortableExecutable::LoadFromFile(PCTCH szFilePath, PCTCH szName) {
	this->szName = szName;
	hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_MODE, NULL, OPEN_EXISTING, NULL, NULL);
	RETURN_IF_INVALID(hFile, SISTATUS::FILE_OPEN_FAILED);
	hFileMapping = CreateFileMapping(hFile, NULL, SEC_IMAGE | PAGE_READONLY, NULL, NULL, NULL);
	RETURN_IF_NULL(hFileMapping, SISTATUS::IMAGE_MAPPING_FAILED);
	pBase = (PBYTE) MapViewOfFile(hFileMapping, FILE_MAP_READ, NULL, NULL, NULL);
	RETURN_IF_NULL(pBase, SISTATUS::IMAGE_MAPPING_FAILED);
	return DoLoad();
}

//TODO error handling (size etc.)
SIAPI SISTATUS PortableExecutable::LoadFromData(PVOID pData, PCTCH szName) {
	this->szName = szName;
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

PCTCH PortableExecutable::GetName() {
	return szName;
}

SIZE_T PortableExecutable::GetSize() {
	return size;
}

SIZE_T PortableExecutable::GetHeaderSize() {
	return headerSize;
}

ULONG_PTR PortableExecutable::GetBase() {
	return (ULONG_PTR) pBase;
}

WORD PortableExecutable::GetCharacteristics() {
	return characteristics;
}

ULONG_PTR PortableExecutable::RelToAbsVA(ULONG_PTR rel) {
	return GetBase() + rel;
}

const std::vector<IMAGE_SECTION_HEADER>& PortableExecutable::GetSections() {
	return vecSections;
}

std::shared_ptr<PortableExecutable> PortableExecutable::CreateFromFile(PCTCH szFilePath, PCTCH szName) {
	PPortableExecutable pPE = new PortableExecutable();
	if (pPE->LoadFromFile(szFilePath, szName) != SISTATUS::SUCCESS) {
		delete pPE;
		return std::shared_ptr<PortableExecutable>(nullptr);
	}
	return std::shared_ptr<PortableExecutable>(pPE);
}

SIZE_T PortableExecutable::GetDirVA(UINT32 uIndex) {
	return RelToAbsVA(pOptHeader->DataDirectory[uIndex].VirtualAddress);
}

ULONG_PTR PortableExecutable::GetDirSize(UINT32 uIndex) {
	return pOptHeader->DataDirectory[uIndex].Size;
}

std::shared_ptr<PortableExecutable> PortableExecutable::CreateFromData(PVOID pData, PCTCH szName) {
	PPortableExecutable pPE = new PortableExecutable();
	if (pPE->LoadFromData(pData, szName) != SISTATUS::SUCCESS) {
		delete pPE;
		return std::shared_ptr<PortableExecutable>(nullptr);
	}
	return std::shared_ptr<PortableExecutable>(pPE);
}
