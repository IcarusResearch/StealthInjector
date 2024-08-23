#include "PortableExecutable2.h"

PortableExecutable2::PortableExecutable2(PCTCH szName) : szName(szName) {}

void PortableExecutable2::LoadFromFile(PCTCH szPath) {
	this->szPath = szPath;
	hFile = CreateFileW(szPath, FILE_GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	CheckError(hFile, SISTATUS::FILE_OPEN_FAILED, "TODO NEED TO SUPPORT WIDE STRINGS");
	hMapping = CreateFileMapping(hFile(), NULL, SEC_IMAGE | PAGE_READONLY, 0, 0, NULL);
	CheckError(hMapping, SISTATUS::IMAGE_MAPPING_FAILED, "TODO NEED TO SUPPORT WIDE STRINGS");
	mapping = MapViewOfFile(hMapping(), FILE_MAP_READ, 0, 0, 0);
	CheckError(mapping, SISTATUS::IMAGE_MAPPING_FAILED, "TODO NEED TO SUPPORT WIDE STRINGS222");
	pLocalBase = (PBYTE) mapping();
	std::cout << "MAPPING AT " << (ULONG_PTR) pLocalBase << "\n";
	bIsMapped = true;
	Load();
}

void PortableExecutable2::Load() {
	pDosHeader = (PIMAGE_DOS_HEADER) pLocalBase;

	// Validate DOS Header
	if (!pDosHeader || pDosHeader->e_magic != 0x5A4D) {
		throw SIException::Create(SISTATUS::INVALID_IMAGE_FORMAT, "TODO", "Invalid DOS header");
	}

	// Validate NT Headers
	pNTHeaders = (PIMAGE_NT_HEADERS) (pLocalBase + pDosHeader->e_lfanew);
	if (!pNTHeaders || pNTHeaders->Signature != 0x00004550) {
		throw SIException::Create(SISTATUS::INVALID_IMAGE_FORMAT, "TODO", "Invalid NT headers");
	}
	pFileHeader = (PIMAGE_FILE_HEADER) &pNTHeaders->FileHeader;
	pOptHeader = (PIMAGE_OPTIONAL_HEADER) &pNTHeaders->OptionalHeader;

	// Load sections
	PIMAGE_SECTION_HEADER pCurrSectionHeader = IMAGE_FIRST_SECTION(pNTHeaders);
	for (UINT i = 0; i < pFileHeader->NumberOfSections; ++i) {
		vecSections.push_back(*pCurrSectionHeader);
		std::cout << "FOUND SECTION " << pCurrSectionHeader->Name << " WITH RVA " << (ULONG_PTR) pCurrSectionHeader->VirtualAddress << "\n";
		DWORD dwDontCare;
		// this should allow us to write to the locally loaded image independent of the actual preferred memory access flags
		VirtualProtect((LPVOID) (LocalBase() + pCurrSectionHeader->VirtualAddress), pCurrSectionHeader->SizeOfRawData, PAGE_READWRITE, &dwDontCare);
		++pCurrSectionHeader;
	}
}

std::shared_ptr<PortableExecutable2> PortableExecutable2::CreateFromFile(PCTCH szFilePath, PCTCH szName) {
	std::shared_ptr<PortableExecutable2> pPE = std::unique_ptr<PortableExecutable2>(new PortableExecutable2(szName));
	pPE->LoadFromFile(szFilePath);
	return pPE;
}

const ULONG_PTR PortableExecutable2::LocalBase() const {
	return (ULONG_PTR) pLocalBase;
}

void PortableExecutable2::RelocateTo(ULONG_PTR uBase) {
	ULONG_PTR uBaseDelta = uBase - LocalBase();
	if (uBaseDelta) {
		ULONG_PTR uDirStartAddr = GetDirVA(IMAGE_DIRECTORY_ENTRY_BASERELOC);
		SIZE_T dirSize = GetDirSize(IMAGE_DIRECTORY_ENTRY_BASERELOC);
		ULONG_PTR uDirEndAddr = uDirStartAddr + dirSize;
		PortableExecutable2::PBaseRelocationBlock pRelocBlock = (PortableExecutable2::PBaseRelocationBlock) uDirStartAddr;
		if (pRelocBlock) {
			while ((ULONG_PTR) pRelocBlock < uDirEndAddr && pRelocBlock->uBlockSize) {
				ULONG uEntries = (pRelocBlock->uBlockSize - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
				std::cout << "ENTRIES: " << uEntries << "\n";
				for (UINT uEntryIdx = 0; uEntryIdx < uEntries; ++uEntryIdx) {
					PortableExecutable2::BaseRelocationEntry currEntry = pRelocBlock->entries[uEntryIdx];
					if (currEntry.Type == IMAGE_REL_BASED_HIGHLOW || currEntry.Type == IMAGE_REL_BASED_DIR64) {
						ULONG_PTR uRelocAtRVA = currEntry.Offset + pRelocBlock->uPageRVA;
						std::cout << "WANT TO RELOCATE AT " << std::hex << uRelocAtRVA << " AND CHANGE " << *((PULONG_PTR) (LocalBase() + uRelocAtRVA)) << " TO " << *((PULONG_PTR) (LocalBase() + uRelocAtRVA)) + uBaseDelta << "\n";
						*((PULONG_PTR) (LocalBase() + uRelocAtRVA)) += uBaseDelta;
						continue;
					}
					if (currEntry.Type == IMAGE_REL_BASED_ABSOLUTE) {
						continue;
					}
					throw SIException::Create(SISTATUS::INVALID_RELOCATION_TYPE);
				}
				pRelocBlock = (PortableExecutable2::PBaseRelocationBlock) (((ULONG_PTR) pRelocBlock) + pRelocBlock->uBlockSize);
			}
		}
	}
}

const PIMAGE_DOS_HEADER PortableExecutable2::DOSHeader() const {
	return pDosHeader;
}

const PIMAGE_NT_HEADERS PortableExecutable2::NTHeaders() const {
	return pNTHeaders;
}

const PIMAGE_FILE_HEADER PortableExecutable2::FileHeader() const {
	return pFileHeader;
}

const PIMAGE_OPTIONAL_HEADER PortableExecutable2::OptionalHeader() const {
	return pOptHeader;
}

ULONG_PTR PortableExecutable2::RVAToVA(ULONG_PTR uRVA) const {
	if (bIsMapped) {
		return LocalBase() + uRVA;
	}
	throw SIException::Create(SISTATUS::UNSUPPORTED_OPERATION, "PortableExecutable#RVAToVA");
}

const std::vector<IMAGE_SECTION_HEADER>& PortableExecutable2::Sections() const {
	return vecSections;
}

SIZE_T PortableExecutable2::GetDirSize(INT32 index) const {
	return pOptHeader->DataDirectory[index].Size;
}

bool PortableExecutable2::IsMapped() const {
	return bIsMapped;
}

PCTCH PortableExecutable2::Name() const {
	return szName;
}

ULONG_PTR PortableExecutable2::GetDirRVA(INT32 index) const {
	return pOptHeader->DataDirectory[index].VirtualAddress;
}

PCTCH PortableExecutable2::Path() const {
	return szPath;
}

ULONG_PTR PortableExecutable2::GetDirVA(INT32 index) const {
	return RVAToVA(GetDirRVA(index));
}

ULONG_PTR PortableExecutable2::GetDirVAFromBase(INT32 index, ULONG_PTR uBase) const {
	return uBase + GetDirRVA(index);
}

ULONG_PTR PortableExecutable2::WantedBase() const {
	return pOptHeader->ImageBase;
}

SIZE_T PortableExecutable2::Size() const {
	return pOptHeader->SizeOfImage;
}

std::vector<PortableExecutable2::Import> PortableExecutable2::GetImports() const {
	std::vector<Import> vecImports;
	PIMAGE_IMPORT_DESCRIPTOR pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR) GetDirVA(IMAGE_DIRECTORY_ENTRY_IMPORT);
	while (pImpDesc->Name) {
		std::cout << "Found import " << (char*) RVAToVA(pImpDesc->Name) << "\n";
		PULONG_PTR pThunk;
		//pImpDesc->
		++pImpDesc;
	}
	return vecImports;
}

bool PortableExecutable2::NeedsRelocation(ULONG_PTR uBase) const {
	return WantedBase() != uBase;
}
