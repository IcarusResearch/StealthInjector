#include "PortableExecutable.h"

#include <iostream>
#include <stdio.h>
#include <fstream>

// TODO do error handling (wrong magic etc)
SISTATUS PortableExecutable::DoLoad() {
	pDosHeader = (PIMAGE_DOS_HEADER) pRaw;
	pNtHeaders = (PIMAGE_NT_HEADERS) (pRaw + pDosHeader->e_lfanew);
	pFileHeader = &pNtHeaders->FileHeader;
	pOptHeader = &pNtHeaders->OptionalHeader;
	size = pOptHeader->SizeOfImage;
	headerSize = pOptHeader->SizeOfHeaders;
	characteristics = pOptHeader->DllCharacteristics;

	PIMAGE_SECTION_HEADER pCurrSectionHeader = IMAGE_FIRST_SECTION(pNtHeaders);
	for (UINT i = 0; i < pFileHeader->NumberOfSections; ++i) {
		vecSections.push_back(*pCurrSectionHeader);
		++pCurrSectionHeader;
	}

	// TODO something is very wrong here
	PIMAGE_TLS_DIRECTORY pTLSDirectory = (PIMAGE_TLS_DIRECTORY) GetDirVA(IMAGE_DIRECTORY_ENTRY_TLS);
	PIMAGE_TLS_CALLBACK* callbacks = (PIMAGE_TLS_CALLBACK*) (GetRaw() + pTLSDirectory->AddressOfCallBacks);

	for (ULONG uCbIdx = 0; callbacks[uCbIdx] != NULL; ++uCbIdx) {
		PIMAGE_TLS_CALLBACK callback = callbacks[uCbIdx];
	}

	return SISTATUS::SUCCESS;
}

PortableExecutable::~PortableExecutable() {
	Reset();
}

//TODO more error handling (size etc.)
SIAPI SISTATUS PortableExecutable::LoadFromFile(PCTCH szFilePath, PCTCH szName) {
	this->szName = szName;
	/*hFile = CreateFile(szFilePath, FILE_GENERIC_READ, FILE_SHARE_MODE, NULL, OPEN_EXISTING, NULL, NULL);
	RETURN_IF_INVALID(hFile, SISTATUS::FILE_OPEN_FAILED);
	hFileMapping = CreateFileMapping(hFile, NULL, SEC_IMAGE | PAGE_READONLY, NULL, NULL, NULL);
	RETURN_IF_NULL(hFileMapping, SISTATUS::IMAGE_MAPPING_FAILED);
	pRaw = (PBYTE) MapViewOfFile(hFileMapping, FILE_MAP_READ, NULL, NULL, NULL); // this somehow only contains 0x400 bytes that are non stupid
	for (int i = 0; i < 0x450; i++) {
		std::cout << std::hex << std::uppercase << (INT32) *(pRaw + i) << " ";
	}
	std::cout << "\n";
	RETURN_IF_NULL(pRaw, SISTATUS::IMAGE_MAPPING_FAILED);
	*/
	std::ifstream fileIn;
	fileIn.open(szFilePath, std::ios::ate | std::ios::binary);
	RETURN_IF_NULL(!!fileIn, SISTATUS::FILE_OPEN_FAILED);
	std::streampos fileSize = fileIn.tellg();
	pBackedFile = std::unique_ptr<BYTE>(new BYTE[fileSize]);
	pRaw = pBackedFile.get();
	fileIn.seekg(std::ios::beg);
	fileIn.read((PCHAR) pRaw, fileSize);
	return DoLoad();
}

//TODO error handling (size etc.)
SIAPI SISTATUS PortableExecutable::LoadFromData(PVOID pData, PCTCH szName) {
	this->szName = szName;
	pRaw = (PBYTE) pData;
	return DoLoad();
}

SIAPI SISTATUS PortableExecutable::Reset() {
	UnmapViewOfFile(pRaw);
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

ULONG_PTR PortableExecutable::GetRaw() {
	return (ULONG_PTR) pRaw;
}

ULONG_PTR PortableExecutable::GetBase() {
	return (ULONG_PTR) pOptHeader->ImageBase;
}

WORD PortableExecutable::GetCharacteristics() {
	return characteristics;
}

const std::vector<IMAGE_SECTION_HEADER>& PortableExecutable::GetSections() {
	return vecSections;
}

ULONG_PTR PortableExecutable::VAResolveExperimental(ULONG_PTR uRel) {
	for (const IMAGE_SECTION_HEADER& section : vecSections) {
		if (section.VirtualAddress < uRel && uRel < section.VirtualAddress + section.Misc.VirtualSize) {
			return (ULONG_PTR) (((ULONG_PTR) pRaw) + uRel - section.VirtualAddress + section.PointerToRawData);
		}
	}
	return 0;
}

std::shared_ptr<PortableExecutable> PortableExecutable::CreateFromFile(PCTCH szFilePath, PCTCH szName) {
	PPortableExecutable pPE = new PortableExecutable();
	if (pPE->LoadFromFile(szFilePath, szName) != SISTATUS::SUCCESS) {
		delete pPE;
		return std::shared_ptr<PortableExecutable>(nullptr);
	}
	return std::shared_ptr<PortableExecutable>(pPE);
}

std::vector<PIMAGE_TLS_CALLBACK>& PortableExecutable::GetTLSCallbacks() {
	return vecTLSCallbacks;
}

ULONG_PTR PortableExecutable::GetEntryPointRVA() {
	return (ULONG_PTR) pOptHeader->AddressOfEntryPoint;
}

const PIMAGE_OPTIONAL_HEADER PortableExecutable::GetOptionalHeader(){
	return pOptHeader;
}

const PIMAGE_FILE_HEADER PortableExecutable::GetFileHeader() {
	return pFileHeader;
}

const PIMAGE_NT_HEADERS PortableExecutable::GetNtHeaders() {
	return pNtHeaders;
}

const PIMAGE_DOS_HEADER PortableExecutable::GetDosHeader() {
	return pDosHeader;
}

std::vector<PortableExecutable::Import> PortableExecutable::GetImports() {
	// DISCLAIMER: I WILL NEVER SUPPORT DELAYED IMPORTS
	std::vector<Import> vecImports;
	PIMAGE_IMPORT_DESCRIPTOR pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR) (VAResolveExperimental(pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress));

	// there are no imports
	if (!pImpDesc || !GetDirSize(IMAGE_DIRECTORY_ENTRY_IMPORT)) {
		return vecImports;
	}

	while (pImpDesc->Name) {
		//std::string szYeet = std::string((char*)pImpDesc->Name);
		//wchar_t y[2048] = {0};
		//MultiByteToWideChar(CP_ACP, 0, szYeet, );
		//std::cout << "DEPENDENCY: " << szYeet << "\n";
		std::cout << "ADDR: " << std::hex << (ULONG_PTR)pImpDesc->Name << "\n";
		++pImpDesc;
	}

	return vecImports;
}

SIZE_T PortableExecutable::GetDirVA(UINT32 uIndex, ULONG_PTR uRel) {
	if (!uRel) {
		uRel = GetRaw();
	}
	return uRel + pOptHeader->DataDirectory[uIndex].VirtualAddress;
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
