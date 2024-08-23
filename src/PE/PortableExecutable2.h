#pragma once

#include "Common.h"

typedef class PortableExecutable2 {

private:
    // smart storage
    AutoHandle hFile;
    AutoHandle hMapping;
    AutoMapping mapping;

    // smart managed raw storage
    PBYTE pLocalBase;
    PIMAGE_DOS_HEADER pDosHeader;
    PIMAGE_NT_HEADERS pNTHeaders;
    PIMAGE_FILE_HEADER pFileHeader;
    PIMAGE_OPTIONAL_HEADER pOptHeader;
    std::vector<IMAGE_SECTION_HEADER> vecSections;

    // attributes
    PCTCH szName;
    PCTCH szPath;
    bool bIsMapped;

    PortableExecutable2() = delete;
    PortableExecutable2(PCTCH szName);

    void LoadFromFile(PCTCH szPath);
    void Load();

public:
    static std::shared_ptr<PortableExecutable2> CreateFromFile(PCTCH szFilePath, PCTCH szName);

    // Data types
    typedef struct BaseRelocationEntry {
        WORD Offset : 12;
        WORD Type : 4;
    } BaseRelocationEntry, *PBaseRelocationEntry;

    typedef struct BaseRelocationBlock {
        ULONG uPageRVA;
        ULONG uBlockSize;
        BaseRelocationEntry entries[1];
    } BaseRelocationBlock, *PBaseRelocationBlock;

    typedef struct Import {
        std::string szName;
        INT16 ordinal; // TODO validate if 16 bits is correct
    } Import, *PImport;

    // Modifying functions
    void RelocateTo(ULONG_PTR uBase);

    // Utility functions
    ULONG_PTR RVAToVA(ULONG_PTR uRVA) const;
    SIZE_T GetDirSize(INT32 index) const;
    ULONG_PTR GetDirRVA(INT32 index) const;
    ULONG_PTR GetDirVA(INT32 index) const;
    ULONG_PTR GetDirVAFromBase(INT32 index, ULONG_PTR uBase) const ;

    // Getter
    const ULONG_PTR LocalBase() const;
    const PIMAGE_DOS_HEADER DOSHeader() const;
    const PIMAGE_NT_HEADERS NTHeaders() const;
    const PIMAGE_FILE_HEADER FileHeader() const;
    const PIMAGE_OPTIONAL_HEADER OptionalHeader() const;
    const std::vector<IMAGE_SECTION_HEADER>& Sections() const;
    PCTCH Name() const;
    PCTCH Path() const;
    SIZE_T Size() const;
    bool IsMapped() const;
    ULONG_PTR WantedBase() const;
    bool NeedsRelocation(ULONG_PTR uBase) const;
    std::vector<Import> GetImports() const;

} PortableExecutable2, *PPortableExecutable2;
