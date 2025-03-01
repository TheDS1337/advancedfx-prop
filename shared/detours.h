#pragma once

#include <windows.h>
#include <vector>

typedef struct MdtMemBlockInfo_s {
	PVOID  BaseAddress;
	SIZE_T RegionSize;
	DWORD  Protect;
} MdtMemBlockInfo;

typedef std::vector<MdtMemBlockInfo> MdtMemBlockInfos;

/// <remarks> len must be at least 5 </remarks>
void *DetourApply(BYTE *orig, BYTE *hook, int len);

/// <remarks> len must be at least 8, (class pointer in ecx is pushed over stack).
///  this one destroys the value of eax, if that is a problem then use
///  DetourVoidClassFunc instead.
/// </remarks>
void *DetourClassFunc(BYTE *src, const BYTE *dst, const int len);


typedef void(*DetourIfacePtr_fn)(void);

/// <remarks>This is somewhat threadsafe, but be aware that outTarget can be already called while you are still in this function.</remarks>
void DetourIfacePtr(DWORD * ptr, void const * hookk, DetourIfacePtr_fn & outTarget);

void * InterceptDllCall(HMODULE hModule, const char * szDllName, const char * szFunctionName, DWORD pNewFunction);

void MdtMemAccessBegin(LPVOID lpAddress, size_t size, MdtMemBlockInfos *mdtMemBlockInfos);
void MdtMemAccessEnd(MdtMemBlockInfos *mdtMemBlockInfos);
LPVOID MdtAllocExecuteableMemory(size_t size);

/// <summary> Replaces a block of 32 bit x86 code with a JMP instruction. </summary>
/// <remarks> countBytes must be at least 5. </remarks>
void Asm32ReplaceWithJmp(void * replaceAt, size_t countBytes, void * jmpTo);
