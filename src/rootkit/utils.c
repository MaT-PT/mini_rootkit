#include "utils.h"
#include <ntifs.h>
#include <ntstrsafe.h>
#include <wdm.h>
// Include minwindef.h after ntifs.h and wdm.h to avoid redefinition errors
#include <minwindef.h>
#include "structures.h"

static HIDDEN_PID gHiddenPidList[HIDDEN_PID_LIST_SIZE];

NTSTATUS AddHiddenPid(IN DWORD dwPid, IN CONST _PEPROCESS pProc) {
    for (DWORD i = 0; i < HIDDEN_PID_LIST_SIZE; ++i) {
        if (gHiddenPidList[i].ProcessId == 0) {
            gHiddenPidList[i].ProcessId = dwPid;
            gHiddenPidList[i].pProc = pProc;
            return STATUS_SUCCESS;
        }
    }
    return STATUS_INSUFFICIENT_RESOURCES;
}

_PEPROCESS GetHiddenPidEprocess(IN DWORD dwPid) {
    for (DWORD i = 0; i < HIDDEN_PID_LIST_SIZE; ++i) {
        if (gHiddenPidList[i].ProcessId == dwPid) {
            return gHiddenPidList[i].pProc;
        }
    }
    return NULL;
}

NTSTATUS RemoveHiddenPid(IN DWORD dwPid) {
    for (DWORD i = 0; i < HIDDEN_PID_LIST_SIZE; ++i) {
        if (gHiddenPidList[i].ProcessId == dwPid) {
            for (; i < HIDDEN_PID_LIST_SIZE - 1 && gHiddenPidList[i].ProcessId != 0; ++i) {
                gHiddenPidList[i] = gHiddenPidList[i + 1];
            }
            if (i == HIDDEN_PID_LIST_SIZE - 1) {
                gHiddenPidList[i].ProcessId = 0;
                gHiddenPidList[i].pProc = NULL;
            }
            return STATUS_SUCCESS;
        }
    }
    return STATUS_NOT_FOUND;
}

VOID PrintSidInfo(IN CONST PSID_AND_ATTRIBUTES pUserAndGroups, IN ULONG ulUserAndGroupCount) {
    PISID pSid;
    CHAR szSid[256];
    CHAR* pszSidEnd = NULL;

    for (ULONG i = 0; i < ulUserAndGroupCount; ++i) {
        pSid = (PISID)pUserAndGroups[i].Sid;
        ULONGLONG ullAuthority = GetAuthorityValue(&pSid->IdentifierAuthority);
        RtlStringCchPrintfExA(szSid, sizeof(szSid), &pszSidEnd, NULL, STRSAFE_FILL_BEHIND_NULL,
                              "S-%hhd-%llu", pSid->Revision, ullAuthority);
        for (UCHAR k = 0; k < pSid->SubAuthorityCount; ++k) {
            RtlStringCchPrintfExA(pszSidEnd, sizeof(szSid) - (pszSidEnd - szSid), &pszSidEnd, NULL,
                                  STRSAFE_FILL_BEHIND_NULL, "-%lu", pSid->SubAuthority[k]);
        }
        if (i == 0) {
            MyDbgPrint("User SID: %s", szSid);
        } else {
            MyDbgPrint("Group %2lu: %s", i, szSid);
        }
    }
}
