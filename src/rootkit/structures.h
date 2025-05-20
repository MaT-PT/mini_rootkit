#ifndef __STRUCTURES_H__
#define __STRUCTURES_H__

#include <ntdef.h>
#include <ntifs.h>
#include <wdm.h>
// Include minwindef.h after ntifs.h and wdm.h to avoid redefinition errors
#include <minwindef.h>

/*
nt!_RTL_RB_TREE
   +0x000 Root             : Ptr64 _RTL_BALANCED_NODE
   +0x008 Encoded          : Pos 0, 1 Bit
   +0x008 Min              : Ptr64 _RTL_BALANCED_NODE
*/
typedef struct _RTL_RB_TREE {
    PRTL_BALANCED_NODE Root;  // 0x000
    union {
        UCHAR Encoded : 1;       // 0x008
        PRTL_BALANCED_NODE Min;  // 0x008
    };
} RTL_RB_TREE, *PRTL_RB_TREE;

/*
nt!_KAB_UM_PROCESS_TREE
   +0x000 Tree             : _RTL_RB_TREE
   +0x010 TreeLock         : Uint8B
*/
typedef struct _KAB_UM_PROCESS_TREE {
    RTL_RB_TREE Tree;    // 0x000
    ULONGLONG TreeLock;  // 0x010
} KAB_UM_PROCESS_TREE, *PKAB_UM_PROCESS_TREE;

/*
nt!_KAB_UM_PROCESS_CONTEXT
   +0x000 Trees            : Ptr64 _KAB_UM_PROCESS_TREE
   +0x008 TreeCount        : Uint4B
*/
typedef struct _KAB_UM_PROCESS_CONTEXT {
    PKAB_UM_PROCESS_TREE Trees;  // 0x000
    ULONG TreeCount;             // 0x008
} KAB_UM_PROCESS_CONTEXT, *PKAB_UM_PROCESS_CONTEXT;

/*
typedef union _KEXECUTE_OPTIONS {
    UCHAR ExecuteDisable : 1;                   // 0x000
    UCHAR ExecuteEnable : 1;                    // 0x000
    UCHAR DisableThunkEmulation : 1;            // 0x000
    UCHAR Permanent : 1;                        // 0x000
    UCHAR ExecuteDispatchEnable : 1;            // 0x000
    UCHAR ImageDispatchEnable : 1;              // 0x000
    UCHAR DisableExceptionChainValidation : 1;  // 0x000
    UCHAR Spare : 1;                            // 0x000
    volatile UCHAR ExecuteOptions;              // 0x000
    UCHAR ExecuteOptionsNV;                     // 0x000
*/
typedef union _KEXECUTE_OPTIONS {
    UCHAR ExecuteDisable : 1;                   // 0x000
    UCHAR ExecuteEnable : 1;                    // 0x000
    UCHAR DisableThunkEmulation : 1;            // 0x000
    UCHAR Permanent : 1;                        // 0x000
    UCHAR ExecuteDispatchEnable : 1;            // 0x000
    UCHAR ImageDispatchEnable : 1;              // 0x000
    UCHAR DisableExceptionChainValidation : 1;  // 0x000
    UCHAR Spare : 1;                            // 0x000
    volatile UCHAR ExecuteOptions;              // 0x000
    UCHAR ExecuteOptionsNV;                     // 0x000
} KEXECUTE_OPTIONS, *PKEXECUTE_OPTIONS;

/*
nt!_KGROUP_MASK
   +0x000 Masks            : [2] Uint8B
*/
typedef struct _KGROUP_MASK {
    ULONGLONG Masks[2];  // 0x000
} KGROUP_MASK, *PKGROUP_MASK;

/*
nt!_KI_PROCESS_CONCURRENCY_COUNT
   +0x000 Fraction         : Pos 0, 18 Bits
   +0x000 Count            : Pos 18, 14 Bits
   +0x000 AllFields        : Uint4B
*/
typedef union _KI_PROCESS_CONCURRENCY_COUNT {
    ULONG Fraction : 18;  // 0x000
    ULONG Count : 14;     // 0x000
    ULONG AllFields;      // 0x000
} KI_PROCESS_CONCURRENCY_COUNT, *PKI_PROCESS_CONCURRENCY_COUNT;

/*
nt!_KI_IDEAL_PROCESSOR_SET_BREAKPOINTS
   +0x000 Low              : _KI_PROCESS_CONCURRENCY_COUNT
   +0x004 High             : _KI_PROCESS_CONCURRENCY_COUNT
*/
typedef struct _KI_IDEAL_PROCESSOR_SET_BREAKPOINTS {
    KI_PROCESS_CONCURRENCY_COUNT Low;   // 0x000
    KI_PROCESS_CONCURRENCY_COUNT High;  // 0x004
} KI_IDEAL_PROCESSOR_SET_BREAKPOINTS, *PKI_IDEAL_PROCESSOR_SET_BREAKPOINTS;

/*
nt!_KAFFINITY_EX
   +0x000 Count            : Uint2B
   +0x002 Size             : Uint2B
   +0x004 Reserved         : Uint4B
   +0x008 Bitmap           : [1] Uint8B
   +0x008 StaticBitmap     : [32] Uint8B
*/
typedef struct _KAFFINITY_EX {
    USHORT Count;    // 0x000
    USHORT Size;     // 0x002
    ULONG Reserved;  // 0x004
    union {
        ULONGLONG Bitmap[1];         // 0x008
        ULONGLONG StaticBitmap[32];  // 0x008
    };
} KAFFINITY_EX, *PKAFFINITY_EX;

/*
nt!_KI_IDEAL_PROCESSOR_ASSIGNMENT_BLOCK
   +0x000 ExpectedConcurrencyCount : _KI_PROCESS_CONCURRENCY_COUNT
   +0x004 Breakpoints      : _KI_IDEAL_PROCESSOR_SET_BREAKPOINTS
   +0x00c AssignmentFlags  : <unnamed-tag>
   +0x010 ThreadSeed       : [32] Uint2B
   +0x050 IdealProcessor   : [32] Uint2B
   +0x090 IdealNode        : [32] Uint2B
   +0x0d0 IdealProcessorSets : _KAFFINITY_EX
*/
typedef struct _KI_IDEAL_PROCESSOR_ASSIGNMENT_BLOCK {
    KI_PROCESS_CONCURRENCY_COUNT ExpectedConcurrencyCount;  // 0x000
    KI_IDEAL_PROCESSOR_SET_BREAKPOINTS Breakpoints;         // 0x004
    union {
        ULONG ConcurrencyCountFixed : 1;  // 0x00c
        ULONG AllFlags;                   // 0x00c
    } AssignmentFlags;                    // 0x00c
    USHORT ThreadSeed[32];                // 0x010
    USHORT IdealProcessor[32];            // 0x050
    USHORT IdealNode[32];                 // 0x090
    KAFFINITY_EX IdealProcessorSets;      // 0x0d0
} KI_IDEAL_PROCESSOR_ASSIGNMENT_BLOCK, *PKI_IDEAL_PROCESSOR_ASSIGNMENT_BLOCK;

/*
nt!_KSTACK_COUNT
   +0x000 Value            : Int4B
   +0x000 State            : Pos 0, 3 Bits
   +0x000 StackCount       : Pos 3, 29 Bits
*/
typedef union _KSTACK_COUNT {
    LONG Value;             // 0x000
    ULONG State : 3;        // 0x000
    ULONG StackCount : 29;  // 0x000
} KSTACK_COUNT, *PKSTACK_COUNT;

/*
nt!_KSCHEDULING_GROUP_POLICY
   +0x000 Value            : Uint4B
   +0x000 Weight           : Uint2B
   +0x000 MinRate          : Uint2B
   +0x002 MaxRate          : Uint2B
   +0x004 AllFlags         : Uint4B
   +0x004 Type             : Pos 0, 1 Bit
   +0x004 Disabled         : Pos 1, 1 Bit
   +0x004 RankBias         : Pos 2, 1 Bit
   +0x004 PerProcessorLimits : Pos 3, 1 Bit
   +0x004 Spare1           : Pos 4, 28 Bits
*/
typedef struct _KSCHEDULING_GROUP_POLICY {
    union {
        ULONG Value;    // 0x000
        USHORT Weight;  // 0x000
        struct {
            USHORT MinRate;  // 0x000
            USHORT MaxRate;  // 0x002
        };
    };
    union {
        ULONG AllFlags;  // 0x004
        struct {
            ULONG Type : 1;                // 0x004
            ULONG Disabled : 1;            // 0x004
            ULONG RankBias : 1;            // 0x004
            ULONG PerProcessorLimits : 1;  // 0x004
            ULONG Spare1 : 28;             // 0x004
        };
    };
} KSCHEDULING_GROUP_POLICY, *PKSCHEDULING_GROUP_POLICY;

/*
nt!_KSCB
   +0x000 GenerationCycles : Uint8B
   +0x008 MinQuotaCycleTarget : Uint8B
   +0x010 MaxQuotaCycleTarget : Uint8B
   +0x018 RankCycleTarget  : Uint8B
   +0x020 LongTermCycles   : Uint8B
   +0x028 LastReportedCycles : Uint8B
   +0x030 OverQuotaHistory : Uint8B
   +0x038 ReadyTime        : Uint8B
   +0x040 InsertTime       : Uint8B
   +0x048 PerProcessorList : _LIST_ENTRY
   +0x058 QueueNode        : _RTL_BALANCED_NODE
   +0x070 Inserted         : Pos 0, 1 Bit
   +0x070 MaxOverQuota     : Pos 1, 1 Bit
   +0x070 MinOverQuota     : Pos 2, 1 Bit
   +0x070 RankBias         : Pos 3, 1 Bit
   +0x070 UnconstrainedMaxQuota : Pos 4, 1 Bit
   +0x070 UnconstrainedMinQuota : Pos 5, 1 Bit
   +0x070 ShareRankOwner   : Pos 6, 1 Bit
   +0x070 PerProcHardCap   : Pos 7, 1 Bit
   +0x070 Referenced       : Pos 8, 1 Bit
   +0x070 PrcbLockFlags    : Uint2B
   +0x072 Depth            : UChar
   +0x073 Pad1             : UChar
   +0x074 OwningProcessorIndex : Uint4B
   +0x078 ReadySummary     : Uint2B
   +0x07a Pad2             : Uint2B
   +0x07c Rank             : Uint4B
   +0x080 ShareRank        : Ptr64 Uint4B
   +0x088 OwnerShareRank   : Uint4B
   +0x090 ReadyListHead    : [16] _LIST_ENTRY
   +0x190 ChildScbQueue    : _RTL_RB_TREE
   +0x1a0 Parent           : Ptr64 _KSCB
   +0x1a8 Root             : Ptr64 _KSCB
   +0x1b0 TracingListEntry : _SINGLE_LIST_ENTRY
*/
typedef struct _KSCB KSCB, *PKSCB;
struct _KSCB {
    ULONGLONG GenerationCycles;           // 0x000
    ULONGLONG MinQuotaCycleTarget;        // 0x008
    ULONGLONG MaxQuotaCycleTarget;        // 0x010
    ULONGLONG RankCycleTarget;            // 0x018
    ULONGLONG LongTermCycles;             // 0x020
    ULONGLONG LastReportedCycles;         // 0x028
    volatile ULONGLONG OverQuotaHistory;  // 0x030
    ULONGLONG ReadyTime;                  // 0x038
    ULONGLONG InsertTime;                 // 0x040
    LIST_ENTRY PerProcessorList;          // 0x048
    RTL_BALANCED_NODE QueueNode;          // 0x058
    union {
        struct {
            USHORT Inserted : 1;               // 0x070
            USHORT MaxOverQuota : 1;           // 0x070
            USHORT MinOverQuota : 1;           // 0x070
            USHORT RankBias : 1;               // 0x070
            USHORT UnconstrainedMaxQuota : 1;  // 0x070
            USHORT UnconstrainedMinQuota : 1;  // 0x070
            USHORT ShareRankOwner : 1;         // 0x070
            USHORT PerProcHardCap : 1;         // 0x070
            USHORT Referenced : 1;             // 0x070
        };
        USHORT PrcbLockFlags;  // 0x070
    };
    UCHAR Depth;                         // 0x072
    UCHAR Pad1;                          // 0x073
    ULONG OwningProcessorIndex;          // 0x074
    USHORT ReadySummary;                 // 0x078
    USHORT Pad2;                         // 0x07a
    ULONG Rank;                          // 0x07c
    volatile PULONG ShareRank;           // 0x080
    volatile ULONG OwnerShareRank;       // 0x088
    LIST_ENTRY ReadyListHead[16];        // 0x090
    RTL_RB_TREE ChildScbQueue;           // 0x190
    PKSCB Parent;                        // 0x1a0
    PKSCB Root;                          // 0x1a8
    SINGLE_LIST_ENTRY TracingListEntry;  // 0x1b0
};

/*
nt!_KSCHEDULING_GROUP
   +0x000 Policy           : _KSCHEDULING_GROUP_POLICY
   +0x008 RelativeWeight   : Uint4B
   +0x00c ChildMinRate     : Uint4B
   +0x010 ChildMinWeight   : Uint4B
   +0x014 ChildTotalWeight : Uint4B
   +0x018 QueryHistoryTimeStamp : Uint8B
   +0x020 NotificationCycles : Int8B
   +0x028 MaxQuotaLimitCycles : Int8B
   +0x030 MaxQuotaCyclesRemaining : Int8B
   +0x038 SchedulingGroupList : _LIST_ENTRY
   +0x038 Sibling          : _LIST_ENTRY
   +0x048 NotificationDpc  : Ptr64 _KDPC
   +0x050 ChildList        : _LIST_ENTRY
   +0x060 Parent           : Ptr64 _KSCHEDULING_GROUP
   +0x080 PerProcessor     : [1] _KSCB
*/
typedef struct _KSCHEDULING_GROUP KSCHEDULING_GROUP, *PKSCHEDULING_GROUP;
struct _KSCHEDULING_GROUP {
    KSCHEDULING_GROUP_POLICY Policy;            // 0x000
    ULONG RelativeWeight;                       // 0x008
    ULONG ChildMinRate;                         // 0x00c
    ULONG ChildMinWeight;                       // 0x010
    ULONG ChildTotalWeight;                     // 0x014
    ULONGLONG QueryHistoryTimeStamp;            // 0x018
    LONGLONG NotificationCycles;                // 0x020
    LONGLONG MaxQuotaLimitCycles;               // 0x028
    volatile LONGLONG MaxQuotaCyclesRemaining;  // 0x030
    union {
        LIST_ENTRY SchedulingGroupList;  // 0x038
        LIST_ENTRY Sibling;              // 0x038
    };
    PKDPC NotificationDpc;      // 0x048
    LIST_ENTRY ChildList;       // 0x050
    PKSCHEDULING_GROUP Parent;  // 0x060
    KSCB PerProcessor[1];       // 0x080
};

/*
nt!_RTL_TICK_LOCK
   +0x000 AsULong64        : Uint8B
   +0x000 Busy             : Pos 0, 1 Bit
   +0x000 Ticks            : Pos 1, 63 Bits
*/
typedef union _RTL_TICK_LOCK {
    ULONGLONG AsULong64;   // 0x000
    ULONGLONG Busy : 1;    // 0x000
    ULONGLONG Ticks : 63;  // 0x000
} RTL_TICK_LOCK, *PRTL_TICK_LOCK;

/*
nt!_EX_PUSH_LOCK
   +0x000 Locked           : Pos 0, 1 Bit
   +0x000 Waiting          : Pos 1, 1 Bit
   +0x000 Waking           : Pos 2, 1 Bit
   +0x000 MultipleShared   : Pos 3, 1 Bit
   +0x000 Shared           : Pos 4, 60 Bits
   +0x000 Value            : Uint8B
   +0x000 Ptr              : Ptr64 Void
*/
typedef struct _EX_PUSH_LOCK {
    union {
        struct {
            ULONGLONG Locked : 1;          // 0x000
            ULONGLONG Waiting : 1;         // 0x000
            ULONGLONG Waking : 1;          // 0x000
            ULONGLONG MultipleShared : 1;  // 0x000
            ULONGLONG Shared : 60;         // 0x000
        };
        ULONGLONG Value;  // 0x000
        PVOID Ptr;        // 0x000
    };
} _EX_PUSH_LOCK, *_PEX_PUSH_LOCK;

/*
nt!_KI_AVAILABLE_CPUS_WORK_STATE
   +0x000 WorkerExecuting  : Pos 0, 1 Bit
   +0x000 WorkPresent      : Pos 1, 1 Bit
   +0x000 Reserved         : Pos 2, 30 Bits
   +0x000 AllFields        : Uint4B
*/
typedef union _KI_AVAILABLE_CPUS_WORK_STATE {
    ULONG WorkerExecuting : 1;  // 0x000
    ULONG WorkPresent : 1;      // 0x000
    ULONG Reserved : 30;        // 0x000
    ULONG AllFields;            // 0x000
} KI_AVAILABLE_CPUS_WORK_STATE, *PKI_AVAILABLE_CPUS_WORK_STATE;

/*
nt!_KI_AVAILABLE_CPUS_WORK_ITEM
   +0x000 ExWorkItem       : _WORK_QUEUE_ITEM
   +0x020 RundownRef       : _EX_RUNDOWN_REF
   +0x028 WorkState        : _KI_AVAILABLE_CPUS_WORK_STATE
   +0x030 Callback         : Ptr64     void
   +0x038 Context          : Ptr64 Void
*/
typedef struct _KI_AVAILABLE_CPUS_WORK_ITEM {
    WORK_QUEUE_ITEM ExWorkItem;              // 0x000
    EX_RUNDOWN_REF RundownRef;               // 0x020
    KI_AVAILABLE_CPUS_WORK_STATE WorkState;  // 0x028
    VOID (*Callback)(PVOID arg1);            // 0x030
    PVOID Context;                           // 0x038
} KI_AVAILABLE_CPUS_WORK_ITEM, *PKI_AVAILABLE_CPUS_WORK_ITEM;

/*nt!_KPROCESS_AVAILABLE_CPU_STATE
   +0x000 SequenceNumber   : _RTL_TICK_LOCK
   +0x008 CpuSetSequenceNumber : Uint8B
   +0x010 ForceParkSequenceNumber : Uint8B
   +0x018 Affinity         : Ptr64 _KAFFINITY_EX
   +0x020 SubscriptionLock : _EX_PUSH_LOCK
   +0x028 SubscriptionList : _LIST_ENTRY
   +0x038 WorkItem         : _KI_AVAILABLE_CPUS_WORK_ITEM
*/
typedef struct _KPROCESS_AVAILABLE_CPU_STATE {
    RTL_TICK_LOCK SequenceNumber;          // 0x000
    ULONGLONG CpuSetSequenceNumber;        // 0x008
    ULONGLONG ForceParkSequenceNumber;     // 0x010
    PKAFFINITY_EX Affinity;                // 0x018
    _EX_PUSH_LOCK SubscriptionLock;        // 0x020
    LIST_ENTRY SubscriptionList;           // 0x028
    KI_AVAILABLE_CPUS_WORK_ITEM WorkItem;  // 0x038
} KPROCESS_AVAILABLE_CPU_STATE, *PKPROCESS_AVAILABLE_CPU_STATE;

/*
nt!_EX_FAST_REF
   +0x000 Object           : Ptr64 Void
   +0x000 RefCnt           : Pos 0, 4 Bits
   +0x000 Value            : Uint8B
*/
typedef struct _EX_FAST_REF {
    union {
        HANDLE Object;         // 0x000
        ULONGLONG RefCnt : 4;  // 0x000
        ULONGLONG Value;       // 0x000
    };
} EX_FAST_REF, *PEX_FAST_REF;

/*
nt!_RTL_AVL_TREE
   +0x000 Root             : Ptr64 _RTL_BALANCED_NODE
*/
typedef struct _RTL_AVL_TREE {
    PRTL_BALANCED_NODE Root;  // 0x000
} RTL_AVL_TREE, *PRTL_AVL_TREE;

typedef struct _PAGEFAULT_HISTORY {
    ULONG CurrentIndex;                         // 0x000
    ULONG MaxIndex;                             // 0x004
    ULONG SpinLock;                             // 0x008
    PVOID Reserved;                             // 0x00c
    PROCESS_WS_WATCH_INFORMATION WatchInfo[1];  // 0x010
} PAGEFAULT_HISTORY, *PPAGEFAULT_HISTORY;

typedef struct _EPROCESS_QUOTA_ENTRY {
    SIZE_T Usage;   // 0x000
    SIZE_T Limit;   // 0x008
    SIZE_T Peak;    // 0x010
    SIZE_T Return;  // 0x018
} EPROCESS_QUOTA_ENTRY, *PEPROCESS_QUOTA_ENTRY;

typedef struct _EPROCESS_QUOTA_BLOCK {
    EPROCESS_QUOTA_ENTRY QuotaEntry[3];  // 0x000
    LIST_ENTRY QuotaList;                // 0x060
    ULONG ReferenceCount;                // 0x070
    ULONG ProcessCount;                  // 0x074
} EPROCESS_QUOTA_BLOCK, *PEPROCESS_QUOTA_BLOCK;

/*
nt!_HANDLE_TABLE_ENTRY_INFO
   +0x000 AuditMask        : Uint4B
   +0x004 MaxRelativeAccessMask : Uint4B
*/
typedef struct _HANDLE_TABLE_ENTRY_INFO {
    ULONG AuditMask;              // 0x000
    ULONG MaxRelativeAccessMask;  // 0x004
} HANDLE_TABLE_ENTRY_INFO, *PHANDLE_TABLE_ENTRY_INFO;

/*
nt!_EXHANDLE
   +0x000 TagBits          : Pos 0, 2 Bits
   +0x000 Index            : Pos 2, 30 Bits
   +0x000 GenericHandleOverlay : Ptr64 Void
   +0x000 Value            : Uint8B
*/
typedef struct _EXHANDLE {
    union {
        struct {
            ULONG TagBits : 2;  // 0x000
            ULONG Index : 30;   // 0x000
        };
        PVOID GenericHandleOverlay;  // 0x000
        ULONGLONG Value;             // 0x000
    };
} EXHANDLE, *PEXHANDLE;

/*
nt!_HANDLE_TABLE_ENTRY
   +0x000 VolatileLowValue : Int8B
   +0x000 LowValue         : Int8B
   +0x000 InfoTable        : Ptr64 _HANDLE_TABLE_ENTRY_INFO
   +0x008 HighValue        : Int8B
   +0x008 NextFreeHandleEntry : Ptr64 _HANDLE_TABLE_ENTRY
   +0x008 LeafHandleValue  : _EXHANDLE
   +0x000 RefCountField    : Int8B
   +0x000 Unlocked         : Pos 0, 1 Bit
   +0x000 RefCnt           : Pos 1, 16 Bits
   +0x000 Attributes       : Pos 17, 3 Bits
   +0x000 ObjectPointerBits : Pos 20, 44 Bits
   +0x008 GrantedAccessBits : Pos 0, 25 Bits
   +0x008 NoRightsUpgrade  : Pos 25, 1 Bit
   +0x008 Spare1           : Pos 26, 6 Bits
   +0x00c Spare2           : Uint4B
*/
typedef union _HANDLE_TABLE_ENTRY HANDLE_TABLE_ENTRY, *PHANDLE_TABLE_ENTRY;
union _HANDLE_TABLE_ENTRY {
    volatile LONGLONG VolatileLowValue;  // 0x000
    LONGLONG LowValue;                   // 0x000
    struct {
        PHANDLE_TABLE_ENTRY_INFO volatile InfoTable;  // 0x000
        LONGLONG HighValue;                           // 0x008
        PHANDLE_TABLE_ENTRY NextFreeHandleEntry;      // 0x008
        EXHANDLE LeafHandleValue;                     // 0x008
    };
    LONGLONG RefCountField;    // 0x000
    ULONGLONG Unlocked : 1;    // 0x000
    ULONGLONG RefCnt : 16;     // 0x000
    ULONGLONG Attributes : 3;  // 0x000
    struct {
        ULONGLONG ObjectPointerBits : 44;  // 0x000
        ULONG GrantedAccessBits : 25;      // 0x008
        ULONG NoRightsUpgrade : 1;         // 0x008
        ULONG Spare1 : 6;                  // 0x008
    };
    ULONG Spare2;  // 0x00c
};

/*
nt!_HANDLE_TABLE_FREE_LIST
   +0x000 FreeListLock     : _EX_PUSH_LOCK
   +0x008 FirstFreeHandleEntry : Ptr64 _HANDLE_TABLE_ENTRY
   +0x010 LastFreeHandleEntry : Ptr64 _HANDLE_TABLE_ENTRY
   +0x018 HandleCount      : Int4B
   +0x01c HighWaterMark    : Uint4B
*/
struct _HANDLE_TABLE_FREE_LIST {
    _EX_PUSH_LOCK FreeListLock;                // 0x000
    PHANDLE_TABLE_ENTRY FirstFreeHandleEntry;  // 0x008
    PHANDLE_TABLE_ENTRY LastFreeHandleEntry;   // 0x010
    LONG HandleCount;                          // 0x018
    ULONG HighWaterMark;                       // 0x01c
} HANDLE_TABLE_FREE_LIST, *PHANDLE_TABLE_FREE_LIST;

/*
nt!_HANDLE_TRACE_DB_ENTRY
   +0x000 ClientId         : _CLIENT_ID
   +0x010 Handle           : Ptr64 Void
   +0x018 Type             : Uint4B
   +0x020 StackTrace       : [16] Ptr64 Void
*/
typedef struct _HANDLE_TRACE_DB_ENTRY {
    CLIENT_ID ClientId;    // 0x000
    PVOID Handle;          // 0x010
    ULONG Type;            // 0x018
    PVOID StackTrace[16];  // 0x020
} HANDLE_TRACE_DB_ENTRY, *PHANDLE_TRACE_DB_ENTRY;

/*
nt!_HANDLE_TRACE_DEBUG_INFO
   +0x000 RefCount         : Int4B
   +0x004 TableSize        : Uint4B
   +0x008 BitMaskFlags     : Uint4B
   +0x010 CloseCompactionLock : _FAST_MUTEX
   +0x048 CurrentStackIndex : Uint4B
   +0x050 TraceDb          : [1] _HANDLE_TRACE_DB_ENTRY
*/
typedef struct _HANDLE_TRACE_DEBUG_INFO {
    LONG RefCount;                     // 0x000
    ULONG TableSize;                   // 0x004
    ULONG BitMaskFlags;                // 0x008
    FAST_MUTEX CloseCompactionLock;    // 0x010
    ULONG CurrentStackIndex;           // 0x048
    HANDLE_TRACE_DB_ENTRY TraceDb[1];  // 0x050
} HANDLE_TRACE_DEBUG_INFO, *PHANDLE_TRACE_DEBUG_INFO;

/*
nt!_HANDLE_TABLE
   +0x000 NextHandleNeedingPool : Uint4B
   +0x004 ExtraInfoPages   : Int4B
   +0x008 TableCode        : Uint8B
   +0x010 QuotaProcess     : Ptr64 _EPROCESS
   +0x018 HandleTableList  : _LIST_ENTRY
   +0x028 UniqueProcessId  : Uint4B
   +0x02c Flags            : Uint4B
   +0x02c StrictFIFO       : Pos 0, 1 Bit
   +0x02c EnableHandleExceptions : Pos 1, 1 Bit
   +0x02c Rundown          : Pos 2, 1 Bit
   +0x02c Duplicated       : Pos 3, 1 Bit
   +0x02c RaiseUMExceptionOnInvalidHandleClose : Pos 4, 1 Bit
   +0x030 HandleContentionEvent : _EX_PUSH_LOCK
   +0x038 HandleTableLock  : _EX_PUSH_LOCK
   +0x040 FreeLists        : [1] _HANDLE_TABLE_FREE_LIST
   +0x040 ActualEntry      : [32] UChar
   +0x060 DebugInfo        : Ptr64 _HANDLE_TRACE_DEBUG_INFO
*/
typedef struct _HANDLE_TABLE {
    ULONG NextHandleNeedingPool;   // 0x000
    LONG ExtraInfoPages;           // 0x004
    volatile ULONGLONG TableCode;  // 0x008
    PEPROCESS QuotaProcess;        // 0x010
    LIST_ENTRY HandleTableList;    // 0x018
    ULONG UniqueProcessId;         // 0x028
    union {
        ULONG Flags;  // 0x02c
        struct {
            UCHAR StrictFIFO : 1;                            // 0x02c
            UCHAR EnableHandleExceptions : 1;                // 0x02c
            UCHAR Rundown : 1;                               // 0x02c
            UCHAR Duplicated : 1;                            // 0x02c
            UCHAR RaiseUMExceptionOnInvalidHandleClose : 1;  // 0x02c
        };
    };
    _EX_PUSH_LOCK HandleContentionEvent;  // 0x030
    _EX_PUSH_LOCK HandleTableLock;        // 0x038
    union {
        struct _HANDLE_TABLE_FREE_LIST FreeLists[1];  // 0x040
        struct {
            UCHAR ActualEntry[32];               // 0x040
            PHANDLE_TRACE_DEBUG_INFO DebugInfo;  // 0x060
        };
    };
} HANDLE_TABLE, *PHANDLE_TABLE;

/*
nt!_SYSTEM_DLL_TYPE
   PsNativeSystemDll = 0n0
   PsWowX86SystemDll = 0n1
   PsWowChpeX86SystemDll = 0n2
   PsChpeV2SystemDll = 0n3
   PsVsmEnclaveRuntimeDll = 0n4
   PsTrustedAppsRuntimeDll = 0n5
   PsSystemDllTotalTypes = 0n6
*/
typedef enum _SYSTEM_DLL_TYPE {
    PsNativeSystemDll = 0,
    PsWowX86SystemDll = 1,
    PsWowChpeX86SystemDll = 2,
    PsChpeV2SystemDll = 3,
    PsVsmEnclaveRuntimeDll = 4,
    PsTrustedAppsRuntimeDll = 5,
    PsSystemDllTotalTypes = 6
} SYSTEM_DLL_TYPE, *PSYSTEM_DLL_TYPE;

typedef struct _EWOW64PROCESS {
    PVOID Peb;                  // 0x000
    SYSTEM_DLL_TYPE NtdllType;  // 0x008
} EWOW64PROCESS, *PEWOW64PROCESS;

/*
nt!_EWOW64PROCESS
   +0x000 Peb              : Ptr64 Void
   +0x008 NtdllType        : _SYSTEM_DLL_TYPE
*/
typedef struct _SE_AUDIT_PROCESS_CREATION_INFO {
    POBJECT_NAME_INFORMATION ImageFileName;  // 0x000
} SE_AUDIT_PROCESS_CREATION_INFO, *PSE_AUDIT_PROCESS_CREATION_INFO;

/*
nt!_MMSUPPORT_FLAGS
   +0x000 WorkingSetType   : Pos 0, 4 Bits
   +0x000 Reserved0        : Pos 4, 2 Bits
   +0x000 MaximumWorkingSetHard : Pos 6, 1 Bit
   +0x000 MinimumWorkingSetHard : Pos 7, 1 Bit
   +0x001 Reserved1        : Pos 0, 1 Bit
   +0x001 TrimmerState     : Pos 1, 2 Bits
   +0x001 LinearAddressProtected : Pos 3, 1 Bit
   +0x001 PageStealers     : Pos 4, 4 Bits
   +0x000 u1               : Uint2B
   +0x002 MemoryPriority   : UChar
   +0x003 WsleDeleted      : Pos 0, 1 Bit
   +0x003 SvmEnabled       : Pos 1, 1 Bit
   +0x003 ForceAge         : Pos 2, 1 Bit
   +0x003 ForceTrim        : Pos 3, 1 Bit
   +0x003 CommitReleaseState : Pos 4, 2 Bits
   +0x003 Reserved2        : Pos 6, 2 Bits
   +0x003 u2               : UChar
   +0x000 EntireFlags      : Uint4B
*/
typedef struct _MMSUPPORT_FLAGS {
    union {
        struct {
            UCHAR WorkingSetType : 4;          // 0x000
            UCHAR Reserved0 : 2;               // 0x000
            UCHAR MaximumWorkingSetHard : 1;   // 0x000
            UCHAR MinimumWorkingSetHard : 1;   // 0x000
            UCHAR Reserved1 : 1;               // 0x001
            UCHAR TrimmerState : 2;            // 0x001
            UCHAR LinearAddressProtected : 1;  // 0x001
            UCHAR PageStealers : 4;            // 0x001
        };
        struct {
            USHORT u1;             // 0x000
            UCHAR MemoryPriority;  // 0x002
            union {
                struct {
                    UCHAR WsleDeleted : 1;         // 0x003
                    UCHAR SvmEnabled : 1;          // 0x003
                    UCHAR ForceAge : 1;            // 0x003
                    UCHAR ForceTrim : 1;           // 0x003
                    UCHAR CommitReleaseState : 2;  // 0x003
                    UCHAR Reserved2 : 2;           // 0x003
                };
                UCHAR u2;  // 0x003
            };
        };
        ULONG EntireFlags;  // 0x000
    };
} MMSUPPORT_FLAGS, *PMMSUPPORT_FLAGS;

/*
nt!_MMPTE_HARDWARE
   +0x000 Valid            : Pos 0, 1 Bit
   +0x000 Dirty1           : Pos 1, 1 Bit
   +0x000 Owner            : Pos 2, 1 Bit
   +0x000 WriteThrough     : Pos 3, 1 Bit
   +0x000 CacheDisable     : Pos 4, 1 Bit
   +0x000 Accessed         : Pos 5, 1 Bit
   +0x000 Dirty            : Pos 6, 1 Bit
   +0x000 LargePage        : Pos 7, 1 Bit
   +0x000 Global           : Pos 8, 1 Bit
   +0x000 CopyOnWrite      : Pos 9, 1 Bit
   +0x000 Unused           : Pos 10, 1 Bit
   +0x000 Write            : Pos 11, 1 Bit
   +0x000 PageFrameNumber  : Pos 12, 40 Bits
   +0x000 ReservedForSoftware : Pos 52, 4 Bits
   +0x000 WsleAge          : Pos 56, 4 Bits
   +0x000 WsleProtection   : Pos 60, 3 Bits
   +0x000 NoExecute        : Pos 63, 1 Bit
*/
typedef struct _MMPTE_HARDWARE {
    ULONGLONG Valid : 1;                // 0x000
    ULONGLONG Dirty1 : 1;               // 0x000
    ULONGLONG Owner : 1;                // 0x000
    ULONGLONG WriteThrough : 1;         // 0x000
    ULONGLONG CacheDisable : 1;         // 0x000
    ULONGLONG Accessed : 1;             // 0x000
    ULONGLONG Dirty : 1;                // 0x000
    ULONGLONG LargePage : 1;            // 0x000
    ULONGLONG Global : 1;               // 0x000
    ULONGLONG CopyOnWrite : 1;          // 0x000
    ULONGLONG Unused : 1;               // 0x000
    ULONGLONG Write : 1;                // 0x000
    ULONGLONG PageFrameNumber : 40;     // 0x000
    ULONGLONG ReservedForSoftware : 4;  // 0x000
    ULONGLONG WsleAge : 4;              // 0x000
    ULONGLONG WsleProtection : 3;       // 0x000
    ULONGLONG NoExecute : 1;            // 0x000
} MMPTE_HARDWARE, *PMMPTE_HARDWARE;

/*
nt!_MMPTE_PROTOTYPE
   +0x000 Valid            : Pos 0, 1 Bit
   +0x000 DemandFillProto  : Pos 1, 1 Bit
   +0x000 HiberVerifyConverted : Pos 2, 1 Bit
   +0x000 ReadOnly         : Pos 3, 1 Bit
   +0x000 SwizzleBit       : Pos 4, 1 Bit
   +0x000 Protection       : Pos 5, 5 Bits
   +0x000 Prototype        : Pos 10, 1 Bit
   +0x000 Combined         : Pos 11, 1 Bit
   +0x000 Unused1          : Pos 12, 4 Bits
   +0x000 ProtoAddress     : Pos 16, 48 Bits
*/
typedef struct _MMPTE_PROTOTYPE {
    ULONGLONG Valid : 1;                 // 0x000
    ULONGLONG DemandFillProto : 1;       // 0x000
    ULONGLONG HiberVerifyConverted : 1;  // 0x000
    ULONGLONG ReadOnly : 1;              // 0x000
    ULONGLONG SwizzleBit : 1;            // 0x000
    ULONGLONG Protection : 5;            // 0x000
    ULONGLONG Prototype : 1;             // 0x000
    ULONGLONG Combined : 1;              // 0x000
    ULONGLONG Unused1 : 4;               // 0x000
    LONGLONG ProtoAddress : 48;          // 0x000
} MMPTE_PROTOTYPE, *PMMPTE_PROTOTYPE;

/*
nt!_MMPTE_SOFTWARE
   +0x000 Valid            : Pos 0, 1 Bit
   +0x000 PageFileReserved : Pos 1, 1 Bit
   +0x000 PageFileAllocated : Pos 2, 1 Bit
   +0x000 ColdPage         : Pos 3, 1 Bit
   +0x000 SwizzleBit       : Pos 4, 1 Bit
   +0x000 Protection       : Pos 5, 5 Bits
   +0x000 Prototype        : Pos 10, 1 Bit
   +0x000 Transition       : Pos 11, 1 Bit
   +0x000 PageFileLow      : Pos 12, 4 Bits
   +0x000 UsedPageTableEntries : Pos 16, 10 Bits
   +0x000 ShadowStack      : Pos 26, 1 Bit
   +0x000 OnStandbyLookaside : Pos 27, 1 Bit
   +0x000 Unused           : Pos 28, 4 Bits
   +0x000 PageFileHigh     : Pos 32, 32 Bits
*/
typedef struct _MMPTE_SOFTWARE {
    ULONGLONG Valid : 1;                  // 0x000
    ULONGLONG PageFileReserved : 1;       // 0x000
    ULONGLONG PageFileAllocated : 1;      // 0x000
    ULONGLONG ColdPage : 1;               // 0x000
    ULONGLONG SwizzleBit : 1;             // 0x000
    ULONGLONG Protection : 5;             // 0x000
    ULONGLONG Prototype : 1;              // 0x000
    ULONGLONG Transition : 1;             // 0x000
    ULONGLONG PageFileLow : 4;            // 0x000
    ULONGLONG UsedPageTableEntries : 10;  // 0x000
    ULONGLONG ShadowStack : 1;            // 0x000
    ULONGLONG OnStandbyLookaside : 1;     // 0x000
    ULONGLONG Unused : 4;                 // 0x000
    ULONGLONG PageFileHigh : 32;          // 0x000
} MMPTE_SOFTWARE, *PMMPTE_SOFTWARE;

/*
nt!_MMPTE_TIMESTAMP
   +0x000 MustBeZero       : Pos 0, 1 Bit
   +0x000 Unused           : Pos 1, 3 Bits
   +0x000 SwizzleBit       : Pos 4, 1 Bit
   +0x000 Protection       : Pos 5, 5 Bits
   +0x000 Prototype        : Pos 10, 1 Bit
   +0x000 Transition       : Pos 11, 1 Bit
   +0x000 PageFileLow      : Pos 12, 4 Bits
   +0x000 Reserved         : Pos 16, 16 Bits
   +0x000 GlobalTimeStamp  : Pos 32, 32 Bits
*/
typedef struct _MMPTE_TIMESTAMP {
    ULONGLONG MustBeZero : 1;        // 0x000
    ULONGLONG Unused : 3;            // 0x000
    ULONGLONG SwizzleBit : 1;        // 0x000
    ULONGLONG Protection : 5;        // 0x000
    ULONGLONG Prototype : 1;         // 0x000
    ULONGLONG Transition : 1;        // 0x000
    ULONGLONG PageFileLow : 4;       // 0x000
    ULONGLONG Reserved : 16;         // 0x000
    ULONGLONG GlobalTimeStamp : 32;  // 0x000
} MMPTE_TIMESTAMP, *PMMPTE_TIMESTAMP;

/*
nt!_MMPTE_TRANSITION
   +0x000 Valid            : Pos 0, 1 Bit
   +0x000 Write            : Pos 1, 1 Bit
   +0x000 OnStandbyLookaside : Pos 2, 1 Bit
   +0x000 IoTracker        : Pos 3, 1 Bit
   +0x000 SwizzleBit       : Pos 4, 1 Bit
   +0x000 Protection       : Pos 5, 5 Bits
   +0x000 Prototype        : Pos 10, 1 Bit
   +0x000 Transition       : Pos 11, 1 Bit
   +0x000 PageFrameNumber  : Pos 12, 40 Bits
   +0x000 Unused           : Pos 52, 12 Bits
*/
typedef struct _MMPTE_TRANSITION {
    ULONGLONG Valid : 1;               // 0x000
    ULONGLONG Write : 1;               // 0x000
    ULONGLONG OnStandbyLookaside : 1;  // 0x000
    ULONGLONG IoTracker : 1;           // 0x000
    ULONGLONG SwizzleBit : 1;          // 0x000
    ULONGLONG Protection : 5;          // 0x000
    ULONGLONG Prototype : 1;           // 0x000
    ULONGLONG Transition : 1;          // 0x000
    ULONGLONG PageFrameNumber : 40;    // 0x000
    ULONGLONG Unused : 12;             // 0x000
} MMPTE_TRANSITION, *PMMPTE_TRANSITION;

/*
nt!_MMPTE_SUBSECTION
   +0x000 Valid            : Pos 0, 1 Bit
   +0x000 Unused0          : Pos 1, 2 Bits
   +0x000 OnStandbyLookaside : Pos 3, 1 Bit
   +0x000 SwizzleBit       : Pos 4, 1 Bit
   +0x000 Protection       : Pos 5, 5 Bits
   +0x000 Prototype        : Pos 10, 1 Bit
   +0x000 ColdPage         : Pos 11, 1 Bit
   +0x000 Unused2          : Pos 12, 3 Bits
   +0x000 ExecutePrivilege : Pos 15, 1 Bit
   +0x000 SubsectionAddress : Pos 16, 48 Bits
*/
typedef struct _MMPTE_SUBSECTION {
    ULONGLONG Valid : 1;               // 0x000
    ULONGLONG Unused0 : 2;             // 0x000
    ULONGLONG OnStandbyLookaside : 1;  // 0x000
    ULONGLONG SwizzleBit : 1;          // 0x000
    ULONGLONG Protection : 5;          // 0x000
    ULONGLONG Prototype : 1;           // 0x000
    ULONGLONG ColdPage : 1;            // 0x000
    ULONGLONG Unused2 : 3;             // 0x000
    ULONGLONG ExecutePrivilege : 1;    // 0x000
    LONGLONG SubsectionAddress : 48;   // 0x000
} MMPTE_SUBSECTION, *PMMPTE_SUBSECTION;

/*
nt!_MMPTE_LIST
   +0x000 Valid            : Pos 0, 1 Bit
   +0x000 OneEntry         : Pos 1, 1 Bit
   +0x000 filler0          : Pos 2, 2 Bits
   +0x000 SwizzleBit       : Pos 4, 1 Bit
   +0x000 Protection       : Pos 5, 5 Bits
   +0x000 Prototype        : Pos 10, 1 Bit
   +0x000 Transition       : Pos 11, 1 Bit
   +0x000 filler1          : Pos 12, 13 Bits
   +0x000 NextEntry        : Pos 25, 39 Bits
*/
typedef struct _MMPTE_LIST {
    ULONGLONG Valid : 1;       // 0x000
    ULONGLONG OneEntry : 1;    // 0x000
    ULONGLONG filler0 : 2;     // 0x000
    ULONGLONG SwizzleBit : 1;  // 0x000
    ULONGLONG Protection : 5;  // 0x000
    ULONGLONG Prototype : 1;   // 0x000
    ULONGLONG Transition : 1;  // 0x000
    ULONGLONG filler1 : 13;    // 0x000
    ULONGLONG NextEntry : 39;  // 0x000
} MMPTE_LIST, *PMMPTE_LIST;

/*
nt!_MMPTE_ACTIVE_PAGE_TABLE_LINKS
   +0x000 Flink            : Pos 0, 40 Bits
   +0x000 BlinkHigh        : Pos 40, 24 Bits
*/
typedef struct _MMPTE_ACTIVE_PAGE_TABLE_LINKS {
    ULONGLONG Flink : 40;      // 0x000
    ULONGLONG BlinkHigh : 24;  // 0x000
} MMPTE_ACTIVE_PAGE_TABLE_LINKS, *PMMPTE_ACTIVE_PAGE_TABLE_LINKS;

/*
nt!_MMPTE
   +0x000 u                : <unnamed-tag>
*/
typedef struct _MMPTE {
    union {
        ULONGLONG Long;                                // 0x000
        MMPTE_HARDWARE Hard;                           // 0x000
        MMPTE_PROTOTYPE Proto;                         // 0x000
        MMPTE_SOFTWARE Soft;                           // 0x000
        MMPTE_TIMESTAMP TimeStamp;                     // 0x000
        MMPTE_TRANSITION Trans;                        // 0x000
        MMPTE_SUBSECTION Subsect;                      // 0x000
        MMPTE_LIST List;                               // 0x000
        MMPTE_ACTIVE_PAGE_TABLE_LINKS PageTableLinks;  // 0x000
    } u;                                               // 0x000
} MMPTE, *PMMPTE;

/*
nt!_MI_ACTIVE_PFN
   +0x000 Leaf             : <unnamed-tag>
   +0x000 PageTable        : <unnamed-tag>
   +0x000 EntireActiveField : Uint8B
*/
typedef struct _MI_ACTIVE_PFN {
    union {
        struct {
            ULONGLONG Tradable : 1;        // 0x000
            ULONGLONG NonPagedBuddy : 43;  // 0x000
            ULONGLONG Spare : 20;          // 0x000
        } Leaf;                            // 0x000
        struct {
            ULONGLONG Tradable : 1;                // 0x000
            ULONGLONG NonPagedBuddy : 31;          // 0x000
            ULONGLONG UsedPageTableEntries : 10;   // 0x000
            ULONGLONG WsleAge : 3;                 // 0x000
            ULONGLONG OldestWsleLeafEntries : 10;  // 0x000
            ULONGLONG OldestWsleLeafAge : 3;       // 0x000
            ULONGLONG Spare : 6;                   // 0x000
        } PageTable;                               // 0x000
        ULONGLONG EntireActiveField;               // 0x000
    };
} MI_ACTIVE_PFN, *PMI_ACTIVE_PFN;

/*
nt!_MIPFNFLINK
   +0x000 NextSlistPfn     : Ptr64 _SLIST_ENTRY
   +0x000 Next             : Ptr64 Void
   +0x000 Flink            : Pos 0, 40 Bits
   +0x000 NodeFlinkLow     : Pos 40, 24 Bits
   +0x000 EntireField      : Uint8B
   +0x000 Active           : _MI_ACTIVE_PFN
*/
typedef struct _MIPFNFLINK {
    union {
        PSLIST_ENTRY NextSlistPfn;  // 0x000
        PVOID Next;                 // 0x000
        struct {
            ULONGLONG Flink : 40;         // 0x000
            ULONGLONG NodeFlinkLow : 24;  // 0x000
        };
        ULONGLONG EntireField;  // 0x000
        MI_ACTIVE_PFN Active;   // 0x000
    };
} MIPFNFLINK, *PMIPFNFLINK;

/*
nt!_MIPFNBLINK
   +0x000 Blink            : Pos 0, 40 Bits
   +0x000 NodeBlinkLow     : Pos 40, 19 Bits
   +0x000 TbFlushStamp     : Pos 59, 3 Bits
   +0x000 PageBlinkDeleteBit : Pos 62, 1 Bit
   +0x000 PageBlinkLockBit : Pos 63, 1 Bit
   +0x000 ShareCount       : Pos 0, 62 Bits
   +0x000 PageShareCountDeleteBit : Pos 62, 1 Bit
   +0x000 PageShareCountLockBit : Pos 63, 1 Bit
   +0x000 EntireField      : Int8B
   +0x000 LockNotUsed      : Pos 0, 62 Bits
   +0x000 DeleteBit        : Pos 62, 1 Bit
   +0x000 LockBit          : Pos 63, 1 Bit
*/
typedef struct _MIPFNBLINK {
    union {
        struct {
            ULONGLONG Blink : 40;                   // 0x000
            ULONGLONG NodeBlinkLow : 19;            // 0x000
            ULONGLONG TbFlushStamp : 3;             // 0x000
            ULONGLONG PageBlinkDeleteBit : 1;       // 0x000
            ULONGLONG PageBlinkLockBit : 1;         // 0x000
            ULONGLONG ShareCount : 62;              // 0x000
            ULONGLONG PageShareCountDeleteBit : 1;  // 0x000
            ULONGLONG PageShareCountLockBit : 1;    // 0x000
        };
        LONGLONG EntireField;  // 0x000
        struct {
            ULONGLONG LockNotUsed : 62;  // 0x000
            ULONGLONG DeleteBit : 1;     // 0x000
            ULONGLONG LockBit : 1;       // 0x000
        };
    };
} MIPFNBLINK, *PMIPFNBLINK;

/*
nt!_MMPFNENTRY1
   +0x000 PageLocation     : Pos 0, 3 Bits
   +0x000 WriteInProgress  : Pos 3, 1 Bit
   +0x000 Modified         : Pos 4, 1 Bit
   +0x000 ReadInProgress   : Pos 5, 1 Bit
   +0x000 CacheAttribute   : Pos 6, 2 Bits
*/
typedef struct _MMPFNENTRY1 {
    UCHAR PageLocation : 3;     // 0x000
    UCHAR WriteInProgress : 1;  // 0x000
    UCHAR Modified : 1;         // 0x000
    UCHAR ReadInProgress : 1;   // 0x000
    UCHAR CacheAttribute : 2;   // 0x000
} MMPFNENTRY1, *PMMPFNENTRY1;

/*
nt!_MMPFNENTRY3
   +0x000 Priority         : Pos 0, 3 Bits
   +0x000 OnProtectedStandby : Pos 3, 1 Bit
   +0x000 InPageError      : Pos 4, 1 Bit
   +0x000 SystemChargedPage : Pos 5, 1 Bit
   +0x000 RemovalRequested : Pos 6, 1 Bit
   +0x000 ParityError      : Pos 7, 1 Bit
*/
typedef struct _MMPFNENTRY3 {
    UCHAR Priority : 3;            // 0x000
    UCHAR OnProtectedStandby : 1;  // 0x000
    UCHAR InPageError : 1;         // 0x000
    UCHAR SystemChargedPage : 1;   // 0x000
    UCHAR RemovalRequested : 1;    // 0x000
    UCHAR ParityError : 1;         // 0x000
} MMPFNENTRY3, *PMMPFNENTRY3;

/*
nt!_MI_PFN_FLAGS
   +0x000 ReferenceCount   : Uint2B
   +0x002 PageLocation     : Pos 0, 3 Bits
   +0x002 WriteInProgress  : Pos 3, 1 Bit
   +0x002 Modified         : Pos 4, 1 Bit
   +0x002 ReadInProgress   : Pos 5, 1 Bit
   +0x002 CacheAttribute   : Pos 6, 2 Bits
   +0x003 Priority         : Pos 0, 3 Bits
   +0x003 OnProtectedStandby : Pos 3, 1 Bit
   +0x003 InPageError      : Pos 4, 1 Bit
   +0x003 SystemChargedPage : Pos 5, 1 Bit
   +0x003 RemovalRequested : Pos 6, 1 Bit
   +0x003 ParityError      : Pos 7, 1 Bit
   +0x000 EntireField      : Uint4B
*/
typedef struct _MI_PFN_FLAGS {
    union {
        struct {
            USHORT ReferenceCount;         // 0x000
            UCHAR PageLocation : 3;        // 0x002
            UCHAR WriteInProgress : 1;     // 0x002
            UCHAR Modified : 1;            // 0x002
            UCHAR ReadInProgress : 1;      // 0x002
            UCHAR CacheAttribute : 2;      // 0x002
            UCHAR Priority : 3;            // 0x003
            UCHAR OnProtectedStandby : 1;  // 0x003
            UCHAR InPageError : 1;         // 0x003
            UCHAR SystemChargedPage : 1;   // 0x003
            UCHAR RemovalRequested : 1;    // 0x003
            UCHAR ParityError : 1;         // 0x003
        };
        ULONG EntireField;  // 0x000
    };
} MI_PFN_FLAGS, *PMI_PFN_FLAGS;

/*
nt!_MI_PFN_FLAGS5
   +0x000 EntireField      : Uint4B
   +0x000 StandbyList      : <unnamed-tag>
   +0x000 MappedPageList   : <unnamed-tag>
   +0x000 Active           : <unnamed-tag>
*/
typedef struct _MI_PFN_FLAGS5 {
    union {
        ULONG EntireField;  // 0x000
        struct {
            ULONG NodeBlinkHigh : 21;    // 0x000
            ULONG NodeFlinkMiddle : 11;  // 0x000
        } StandbyList;                   // 0x000
        struct {
            UCHAR ModifiedListBucketIndex : 4;  // 0x000
        } MappedPageList;                       // 0x000
        struct {
            ULONG PageTableBlinkLow : 16;   // 0x000
            ULONG PageTableBuddyHigh : 10;  // 0x000
            ULONG PageTableLinked : 1;      // 0x000
            ULONG AnchorLargePageSize : 2;  // 0x000
            ULONG Spare1 : 3;               // 0x000
        } Active;                           // 0x000
    };
} MI_PFN_FLAGS5, *PMI_PFN_FLAGS5;

/*
nt!_MI_PFN_FLAGS4
   +0x000 PteFrame         : Pos 0, 40 Bits
   +0x000 ResidentPage     : Pos 40, 1 Bit
   +0x000 ResidentPageContainsBadPages : Pos 41, 1 Bit
   +0x000 Unused1          : Pos 42, 1 Bit
   +0x000 Partition        : Pos 43, 10 Bits
   +0x000 FileOnly         : Pos 53, 1 Bit
   +0x000 PfnExists        : Pos 54, 1 Bit
   +0x000 NodeFlinkHigh    : Pos 55, 5 Bits
   +0x000 PageIdentity     : Pos 60, 3 Bits
   +0x000 PrototypePte     : Pos 63, 1 Bit
   +0x000 EntireField      : Uint8B
*/
typedef struct _MI_PFN_FLAGS4 {
    union {
        struct {
            ULONGLONG PteFrame : 40;                     // 0x000
            ULONGLONG ResidentPage : 1;                  // 0x000
            ULONGLONG ResidentPageContainsBadPages : 1;  // 0x000
            ULONGLONG Unused1 : 1;                       // 0x000
            ULONGLONG Partition : 10;                    // 0x000
            ULONGLONG FileOnly : 1;                      // 0x000
            ULONGLONG PfnExists : 1;                     // 0x000
            ULONGLONG NodeFlinkHigh : 5;                 // 0x000
            ULONGLONG PageIdentity : 3;                  // 0x000
            ULONGLONG PrototypePte : 1;                  // 0x000
        };
        ULONGLONG EntireField;  // 0x000
    };
} MI_PFN_FLAGS4, *PMI_PFN_FLAGS4;

/*
nt!_MMPFN
   +0x000 ListEntry        : _LIST_ENTRY
   +0x000 TreeNode         : _RTL_BALANCED_NODE
   +0x000 u1               : _MIPFNFLINK
   +0x008 PteAddress       : Ptr64 _MMPTE
   +0x008 PteLong          : Uint8B
   +0x010 OriginalPte      : _MMPTE
   +0x018 u2               : _MIPFNBLINK
   +0x020 u3               : <unnamed-tag>
   +0x024 u5               : _MI_PFN_FLAGS5
   +0x028 u4               : _MI_PFN_FLAGS4
*/
typedef struct _MMPFN {
    union {
        LIST_ENTRY ListEntry;        // 0x000
        RTL_BALANCED_NODE TreeNode;  // 0x000
        struct {
            MIPFNFLINK u1;  // 0x000
            union {
                PMMPTE PteAddress;  // 0x008
                ULONGLONG PteLong;  // 0x008
            };
            MMPTE OriginalPte;  // 0x010
        };
    };
    volatile MIPFNBLINK u2;  // 0x018
    union {
        struct {
            USHORT ReferenceCount;  // 0x020
            MMPFNENTRY1 e1;         // 0x022
        };
        struct {
            MMPFNENTRY3 e3;  // 0x023
            struct {
                USHORT ReferenceCount;  // 0x020
            } e2;                       // 0x020
        };
        volatile MI_PFN_FLAGS e4;  // 0x020
    } u3;                          // 0x020
    MI_PFN_FLAGS5 u5;              // 0x024
    MI_PFN_FLAGS4 u4;              // 0x028
} MMPFN, *PMMPFN;

/*
nt!_MI_WORKING_SET_PAGE_TABLE_AGE_LINKS
   +0x000 Lock             : Int4B
   +0x004 EntryCount       : Uint4B
   +0x008 Flink            : Ptr64 _MMPFN
   +0x010 Blink            : Ptr64 _MMPFN
*/
typedef struct _MI_WORKING_SET_PAGE_TABLE_AGE_LINKS {
    volatile LONG Lock;  // 0x000
    ULONG EntryCount;    // 0x004
    PMMPFN Flink;        // 0x008
    PMMPFN Blink;        // 0x010
} MI_WORKING_SET_PAGE_TABLE_AGE_LINKS, *PMI_WORKING_SET_PAGE_TABLE_AGE_LINKS;

/*
nt!_MMWSL_INSTANCE
   +0x000 PteResumePoint   : [3] Ptr64 _MMPTE
   +0x018 LastAccessClearingRemainder : Uint4B
   +0x01c LastAgingRemainder : Uint4B
   +0x020 LockedEntries    : Uint8B
   +0x040 ActivePageTableLinks : [8] _MI_WORKING_SET_PAGE_TABLE_AGE_LINKS
*/
typedef struct _MMWSL_INSTANCE {
    PMMPTE PteResumePoint[3];                                     // 0x000
    ULONG LastAccessClearingRemainder;                            // 0x018
    ULONG LastAgingRemainder;                                     // 0x01c
    ULONGLONG LockedEntries;                                      // 0x020
    MI_WORKING_SET_PAGE_TABLE_AGE_LINKS ActivePageTableLinks[8];  // 0x040
} MMWSL_INSTANCE, *PMMWSL_INSTANCE;

/*
nt!_MMSUPPORT_INSTANCE
   +0x000 NextPageColor    : Uint4B
   +0x004 PageFaultCount   : Uint4B
   +0x008 TrimmedPageCount : Uint8B
   +0x010 VmWorkingSetList : Ptr64 _MMWSL_INSTANCE
   +0x018 WorkingSetExpansionLinks : _LIST_ENTRY
   +0x028 AgeDistribution  : [8] Uint8B
   +0x068 ExitOutswapGate  : Ptr64 _KGATE
   +0x070 MinimumWorkingSetSize : Uint8B
   +0x078 MaximumWorkingSetSize : Uint8B
   +0x080 WorkingSetLeafSize : Uint8B
   +0x088 WorkingSetLeafPrivateSize : Uint8B
   +0x090 WorkingSetSize   : Uint8B
   +0x098 WorkingSetPrivateSize : Uint8B
   +0x0a0 PeakWorkingSetSize : Uint8B
   +0x0a8 HardFaultCount   : Uint4B
   +0x0ac LastTrimStamp    : Uint2B
   +0x0ae PartitionId      : Uint2B
   +0x0b0 SelfmapLock      : Uint8B
   +0x0b8 Flags            : _MMSUPPORT_FLAGS
   +0x0bc InterlockedFlags : Uint4B
*/
typedef struct _MMSUPPORT_INSTANCE {
    ULONG NextPageColor;                           // 0x000
    volatile ULONG PageFaultCount;                 // 0x004
    ULONGLONG TrimmedPageCount;                    // 0x008
    PMMWSL_INSTANCE VmWorkingSetList;              // 0x010
    LIST_ENTRY WorkingSetExpansionLinks;           // 0x018
    volatile ULONGLONG AgeDistribution[8];         // 0x028
    PKGATE ExitOutswapGate;                        // 0x068
    ULONGLONG MinimumWorkingSetSize;               // 0x070
    ULONGLONG MaximumWorkingSetSize;               // 0x078
    volatile ULONGLONG WorkingSetLeafSize;         // 0x080
    volatile ULONGLONG WorkingSetLeafPrivateSize;  // 0x088
    volatile ULONGLONG WorkingSetSize;             // 0x090
    volatile ULONGLONG WorkingSetPrivateSize;      // 0x098
    volatile ULONGLONG PeakWorkingSetSize;         // 0x0a0
    ULONG HardFaultCount;                          // 0x0a8
    USHORT LastTrimStamp;                          // 0x0ac
    USHORT PartitionId;                            // 0x0ae
    ULONGLONG SelfmapLock;                         // 0x0b0
    volatile MMSUPPORT_FLAGS Flags;                // 0x0b8
    volatile ULONG InterlockedFlags;               // 0x0bc
} MMSUPPORT_INSTANCE, *PMMSUPPORT_INSTANCE;

/*
nt!_MMSUPPORT_SHARED
   +0x000 WorkingSetLockArray : Ptr64 Void
   +0x008 ReleasedCommitDebt : Uint8B
   +0x010 ResetPagesRepurposedCount : Uint8B
   +0x018 WsSwapSupport    : Ptr64 Void
   +0x020 CommitReleaseContext : Ptr64 Void
   +0x028 AccessLog        : Ptr64 Void
   +0x030 ChargedWslePages : Uint8B
   +0x038 ActualWslePages  : Uint8B
   +0x040 WorkingSetCoreLock : Int4B
   +0x048 ShadowMapping    : Ptr64 Void
*/
typedef struct _MMSUPPORT_SHARED {
    PVOID WorkingSetLockArray;            // 0x000
    ULONGLONG ReleasedCommitDebt;         // 0x008
    ULONGLONG ResetPagesRepurposedCount;  // 0x010
    PVOID WsSwapSupport;                  // 0x018
    PVOID CommitReleaseContext;           // 0x020
    PVOID AccessLog;                      // 0x028
    volatile ULONGLONG ChargedWslePages;  // 0x030
    volatile ULONGLONG ActualWslePages;   // 0x038
    volatile LONG WorkingSetCoreLock;     // 0x040
    PVOID ShadowMapping;                  // 0x048
} MMSUPPORT_SHARED, *PMMSUPPORT_SHARED;

/*
nt!_ALPC_PROCESS_CONTEXT
   +0x000 Lock             : _EX_PUSH_LOCK
   +0x008 ViewListHead     : _LIST_ENTRY
   +0x018 PagedPoolQuotaCache : Uint8B
*/
typedef struct _ALPC_PROCESS_CONTEXT {
    _EX_PUSH_LOCK Lock;                      // 0x000
    LIST_ENTRY ViewListHead;                 // 0x008
    volatile ULONGLONG PagedPoolQuotaCache;  // 0x018
} ALPC_PROCESS_CONTEXT, *PALPC_PROCESS_CONTEXT;

/*
nt!_PO_DIAG_STACK_RECORD
   +0x000 StackDepth       : Uint4B
   +0x008 Stack            : [1] Ptr64 Void
*/
typedef struct _PO_DIAG_STACK_RECORD {
    ULONG StackDepth;  // 0x000
    PVOID Stack[1];    // 0x008
} PO_DIAG_STACK_RECORD, *PPO_DIAG_STACK_RECORD;

/*
nt!_IMAGE_RUNTIME_FUNCTION_ENTRY
   +0x000 BeginAddress     : Uint4B
   +0x004 EndAddress       : Uint4B
   +0x008 UnwindInfoAddress : Uint4B
   +0x008 UnwindData       : Uint4B
*/
typedef struct _IMAGE_RUNTIME_FUNCTION_ENTRY {
    ULONG BeginAddress;  // 0x000
    ULONG EndAddress;    // 0x004
    union {
        ULONG UnwindInfoAddress;  // 0x008
        ULONG UnwindData;         // 0x008
    };
} IMAGE_RUNTIME_FUNCTION_ENTRY, *PIMAGE_RUNTIME_FUNCTION_ENTRY;

/*
nt!_FUNCTION_TABLE_TYPE
   RF_SORTED = 0n0
   RF_UNSORTED = 0n1
   RF_CALLBACK = 0n2
   RF_KERNEL_DYNAMIC = 0n3
*/
typedef enum _FUNCTION_TABLE_TYPE {
    RF_SORTED = 0,
    RF_UNSORTED = 1,
    RF_CALLBACK = 2,
    RF_KERNEL_DYNAMIC = 3
} FUNCTION_TABLE_TYPE, *PFUNCTION_TABLE_TYPE;

/*
nt!_DYNAMIC_FUNCTION_TABLE
   +0x000 ListEntry        : _LIST_ENTRY
   +0x010 FunctionTable    : Ptr64 _IMAGE_RUNTIME_FUNCTION_ENTRY
   +0x018 TimeStamp        : _LARGE_INTEGER
   +0x020 MinimumAddress   : Uint8B
   +0x028 MaximumAddress   : Uint8B
   +0x030 BaseAddress      : Uint8B
   +0x038 Callback         : Ptr64     _IMAGE_RUNTIME_FUNCTION_ENTRY*
   +0x040 Context          : Ptr64 Void
   +0x048 OutOfProcessCallbackDll : Ptr64 Wchar
   +0x050 Type             : _FUNCTION_TABLE_TYPE
   +0x054 EntryCount       : Uint4B
   +0x058 TreeNodeMin      : _RTL_BALANCED_NODE
   +0x070 TreeNodeMax      : _RTL_BALANCED_NODE
*/
typedef struct _DYNAMIC_FUNCTION_TABLE {
    LIST_ENTRY ListEntry;                                                   // 0x000
    PIMAGE_RUNTIME_FUNCTION_ENTRY FunctionTable;                            // 0x010
    LARGE_INTEGER TimeStamp;                                                // 0x018
    ULONGLONG MinimumAddress;                                               // 0x020
    ULONGLONG MaximumAddress;                                               // 0x028
    ULONGLONG BaseAddress;                                                  // 0x030
    PIMAGE_RUNTIME_FUNCTION_ENTRY (*Callback)(ULONGLONG arg1, PVOID arg2);  // 0x038
    PVOID Context;                                                          // 0x040
    PWCHAR OutOfProcessCallbackDll;                                         // 0x048
    FUNCTION_TABLE_TYPE Type;                                               // 0x050
    ULONG EntryCount;                                                       // 0x054
    RTL_BALANCED_NODE TreeNodeMin;                                          // 0x058
    RTL_BALANCED_NODE TreeNodeMax;                                          // 0x070
} DYNAMIC_FUNCTION_TABLE, *PDYNAMIC_FUNCTION_TABLE;

/*
nt!_INVERTED_FUNCTION_TABLE_ENTRY
   +0x000 FunctionTable    : Ptr64 _IMAGE_RUNTIME_FUNCTION_ENTRY
   +0x000 DynamicTable     : Ptr64 _DYNAMIC_FUNCTION_TABLE
   +0x008 ImageBase        : Ptr64 Void
   +0x010 SizeOfImage      : Uint4B
   +0x014 SizeOfTable      : Uint4B
*/
typedef struct _INVERTED_FUNCTION_TABLE_ENTRY {
    union {
        PIMAGE_RUNTIME_FUNCTION_ENTRY FunctionTable;  // 0x000
        PDYNAMIC_FUNCTION_TABLE DynamicTable;         // 0x000
    };
    PVOID ImageBase;    // 0x008
    ULONG SizeOfImage;  // 0x010
    ULONG SizeOfTable;  // 0x014
} INVERTED_FUNCTION_TABLE_ENTRY, *PINVERTED_FUNCTION_TABLE_ENTRY;

/*
nt!_INVERTED_FUNCTION_TABLE_KERNEL_MODE
   +0x000 CurrentSize      : Uint4B
   +0x004 MaximumSize      : Uint4B
   +0x008 Epoch            : Uint4B
   +0x00c Overflow         : UChar
   +0x010 TableEntry       : [256] _INVERTED_FUNCTION_TABLE_ENTRY
*/
typedef struct _INVERTED_FUNCTION_TABLE_KERNEL_MODE {
    ULONG CurrentSize;                              // 0x000
    ULONG MaximumSize;                              // 0x004
    volatile ULONG Epoch;                           // 0x008
    UCHAR Overflow;                                 // 0x00c
    INVERTED_FUNCTION_TABLE_ENTRY TableEntry[256];  // 0x010
} INVERTED_FUNCTION_TABLE_KERNEL_MODE, *PINVERTED_FUNCTION_TABLE_KERNEL_MODE;

/*
nt!_MMSUPPORT_FULL
   +0x000 Instance         : _MMSUPPORT_INSTANCE
   +0x0c0 Shared           : _MMSUPPORT_SHARED
*/
typedef struct _MMSUPPORT_FULL {
    MMSUPPORT_INSTANCE Instance;  // 0x000
    MMSUPPORT_SHARED Shared;      // 0x0c0
} MMSUPPORT_FULL, *PMMSUPPORT_FULL;

/*
nt!_PS_PROTECTION
   +0x000 Level            : UChar
   +0x000 Type             : Pos 0, 3 Bits
   +0x000 Audit            : Pos 3, 1 Bit
   +0x000 Signer           : Pos 4, 4 Bits
*/
typedef struct _PS_PROTECTION {
    union {
        UCHAR Level;  // 0x000
        struct {
            UCHAR Type : 3;    // 0x000
            UCHAR Audit : 1;   // 0x000
            UCHAR Signer : 4;  // 0x000
        };
    };
} PS_PROTECTION, *PPS_PROTECTION;

/*
nt!_PROCESS_DISK_COUNTERS
   +0x000 BytesRead        : Uint8B
   +0x008 BytesWritten     : Uint8B
   +0x010 ReadOperationCount : Uint8B
   +0x018 WriteOperationCount : Uint8B
   +0x020 FlushOperationCount : Uint8B
*/
typedef struct _PROCESS_DISK_COUNTERS {
    ULONGLONG BytesRead;            // 0x000
    ULONGLONG BytesWritten;         // 0x008
    ULONGLONG ReadOperationCount;   // 0x010
    ULONGLONG WriteOperationCount;  // 0x018
    ULONGLONG FlushOperationCount;  // 0x020
} PROCESS_DISK_COUNTERS, *PPROCESS_DISK_COUNTERS;

/*
nt!_PS_INTERLOCKED_TIMER_DELAY_VALUES
   +0x000 DelayMs          : Pos 0, 30 Bits
   +0x000 CoalescingWindowMs : Pos 30, 30 Bits
   +0x000 Reserved         : Pos 60, 1 Bit
   +0x000 NewTimerWheel    : Pos 61, 1 Bit
   +0x000 Retry            : Pos 62, 1 Bit
   +0x000 Locked           : Pos 63, 1 Bit
   +0x000 All              : Uint8B
*/
typedef union _PS_INTERLOCKED_TIMER_DELAY_VALUES {
    ULONGLONG DelayMs : 30;             // 0x000
    ULONGLONG CoalescingWindowMs : 30;  // 0x000
    ULONGLONG Reserved : 1;             // 0x000
    ULONGLONG NewTimerWheel : 1;        // 0x000
    ULONGLONG Retry : 1;                // 0x000
    ULONGLONG Locked : 1;               // 0x000
    ULONGLONG All;                      // 0x000
} PS_INTERLOCKED_TIMER_DELAY_VALUES, *PPS_INTERLOCKED_TIMER_DELAY_VALUES;

/*
nt!_JOBOBJECT_WAKE_FILTER
   +0x000 HighEdgeFilter   : Uint4B
   +0x004 LowEdgeFilter    : Uint4B
*/
typedef struct _JOBOBJECT_WAKE_FILTER {
    ULONG HighEdgeFilter;  // 0x000
    ULONG LowEdgeFilter;   // 0x004
} JOBOBJECT_WAKE_FILTER, *PJOBOBJECT_WAKE_FILTER;

/*
nt!_PS_PROCESS_WAKE_INFORMATION
   +0x000 NotificationChannel : Uint8B
   +0x008 WakeCounters     : [7] Uint4B
   +0x024 WakeFilter       : _JOBOBJECT_WAKE_FILTER
   +0x02c NoWakeCounter    : Uint4B
*/
typedef struct _PS_PROCESS_WAKE_INFORMATION {
    ULONGLONG NotificationChannel;     // 0x000
    ULONG WakeCounters[7];             // 0x008
    JOBOBJECT_WAKE_FILTER WakeFilter;  // 0x024
    ULONG NoWakeCounter;               // 0x02c
} PS_PROCESS_WAKE_INFORMATION, *PPS_PROCESS_WAKE_INFORMATION;

/*
nt!_PS_DYNAMIC_ENFORCED_ADDRESS_RANGES
   +0x000 Tree             : _RTL_AVL_TREE
   +0x008 Lock             : _EX_PUSH_LOCK
*/
typedef struct _PS_DYNAMIC_ENFORCED_ADDRESS_RANGES {
    struct _RTL_AVL_TREE Tree;  // 0x000
    struct _EX_PUSH_LOCK Lock;  // 0x008
} PS_DYNAMIC_ENFORCED_ADDRESS_RANGES, *PPS_DYNAMIC_ENFORCED_ADDRESS_RANGES;

/*
nt!_PSP_SYSCALL_PROVIDER_DISPATCH_CONTEXT
   +0x000 Level            : Uint4B
   +0x004 Slot             : Uint4B
*/
typedef struct _PSP_SYSCALL_PROVIDER_DISPATCH_CONTEXT {
    ULONG Level;  // 0x000
    ULONG Slot;   // 0x004
} PSP_SYSCALL_PROVIDER_DISPATCH_CONTEXT, *PPSP_SYSCALL_PROVIDER_DISPATCH_CONTEXT;

/*
nt!_PROCESS_EXECUTION_TRANSITION
   +0x000 TransitionState  : Int2B
   +0x000 InProgress       : Pos 0, 1 Bit
   +0x000 Reserved         : Pos 1, 7 Bits
*/
typedef union _PROCESS_EXECUTION_TRANSITION {
    volatile SHORT TransitionState;  // 0x000
    USHORT InProgress : 1;           // 0x000
    USHORT Reserved : 7;             // 0x000
} PROCESS_EXECUTION_TRANSITION, *PPROCESS_EXECUTION_TRANSITION;

/*
nt!_PROCESS_EXECUTION_STATE
   +0x000 State            : Char
   +0x000 ProcessFrozen    : Pos 0, 1 Bit
   +0x000 ProcessSwapped   : Pos 1, 1 Bit
   +0x000 ProcessGraphicsFreezeOptimized : Pos 2, 1 Bit
   +0x000 Reserved         : Pos 3, 5 Bits
*/
typedef union _PROCESS_EXECUTION_STATE {
    CHAR State;                                // 0x000
    UCHAR ProcessFrozen : 1;                   // 0x000
    UCHAR ProcessSwapped : 1;                  // 0x000
    UCHAR ProcessGraphicsFreezeOptimized : 1;  // 0x000
    UCHAR Reserved : 5;                        // 0x000
} PROCESS_EXECUTION_STATE, *PPROCESS_EXECUTION_STATE;

/*
nt!_PROCESS_EXECUTION
   +0x000 State            : Int4B
   +0x000 Transition       : _PROCESS_EXECUTION_TRANSITION
   +0x002 Current          : _PROCESS_EXECUTION_STATE
   +0x003 Requested        : _PROCESS_EXECUTION_STATE
*/
typedef union _PROCESS_EXECUTION {
    volatile LONG State;  // 0x000
    struct {
        volatile PROCESS_EXECUTION_TRANSITION Transition;  // 0x000
        PROCESS_EXECUTION_STATE Current;                   // 0x002
    };
    PROCESS_EXECUTION_STATE Requested;  // 0x003
} PROCESS_EXECUTION, *PPROCESS_EXECUTION;

/*
nt!_SEP_TOKEN_PRIVILEGES
   +0x000 Present          : Uint8B
   +0x008 Enabled          : Uint8B
   +0x010 EnabledByDefault : Uint8B
*/
typedef struct _SEP_TOKEN_PRIVILEGES {
    ULONGLONG Present;           // 0x000
    ULONGLONG Enabled;           // 0x008
    ULONGLONG EnabledByDefault;  // 0x010
} SEP_TOKEN_PRIVILEGES, *PSEP_TOKEN_PRIVILEGES;

/*
nt!_SEP_AUDIT_POLICY
   +0x000 AdtTokenPolicy   : _TOKEN_AUDIT_POLICY
   +0x01e PolicySetStatus  : UChar
*/
typedef struct _SEP_AUDIT_POLICY {
    TOKEN_AUDIT_POLICY AdtTokenPolicy;  // 0x000
    UCHAR PolicySetStatus;              // 0x01e
} SEP_AUDIT_POLICY, *PSEP_AUDIT_POLICY;

/*
nt!_OBJECT_DIRECTORY_ENTRY
   +0x000 ChainLink        : Ptr64 _OBJECT_DIRECTORY_ENTRY
   +0x008 Object           : Ptr64 Void
   +0x010 HashValue        : Uint4B
*/
typedef struct _OBJECT_DIRECTORY_ENTRY OBJECT_DIRECTORY_ENTRY, *POBJECT_DIRECTORY_ENTRY;
struct _OBJECT_DIRECTORY_ENTRY {
    POBJECT_DIRECTORY_ENTRY ChainLink;  // 0x000
    PVOID Object;                       // 0x008
    ULONG HashValue;                    // 0x010
};

typedef struct _DEVICE_MAP DEVICE_MAP, *PDEVICE_MAP;

/*
nt!_OBJECT_DIRECTORY
   +0x000 HashBuckets      : [37] Ptr64 _OBJECT_DIRECTORY_ENTRY
   +0x128 Lock             : _EX_PUSH_LOCK
   +0x130 DeviceMap        : Ptr64 _DEVICE_MAP
   +0x138 ShadowDirectory  : Ptr64 _OBJECT_DIRECTORY
   +0x140 NamespaceEntry   : Ptr64 Void
   +0x148 SessionObject    : Ptr64 Void
   +0x150 Flags            : Uint4B
   +0x154 SessionId        : Uint4B
*/
typedef struct _OBJECT_DIRECTORY OBJECT_DIRECTORY, *POBJECT_DIRECTORY;
struct _OBJECT_DIRECTORY {
    POBJECT_DIRECTORY_ENTRY HashBuckets[37];  // 0x000
    _EX_PUSH_LOCK Lock;                       // 0x128
    PDEVICE_MAP DeviceMap;                    // 0x130
    POBJECT_DIRECTORY ShadowDirectory;        // 0x138
    PVOID NamespaceEntry;                     // 0x140
    PVOID SessionObject;                      // 0x148
    ULONG Flags;                              // 0x150
    ULONG SessionId;                          // 0x154
};

/*
nt!_DEVICE_MAP
   +0x000 DosDevicesDirectory : Ptr64 _OBJECT_DIRECTORY
   +0x008 GlobalDosDevicesDirectory : Ptr64 _OBJECT_DIRECTORY
   +0x010 ServerSilo       : Ptr64 _EJOB
   +0x018 GlobalDeviceMap  : Ptr64 _DEVICE_MAP
   +0x020 DriveObject      : [26] _EX_FAST_REF
   +0x0f0 ReferenceCount   : Int8B
   +0x0f8 DosDevicesDirectoryHandle : Ptr64 Void
   +0x100 DriveMap         : Uint4B
   +0x104 DriveType        : [32] UChar
*/
struct _DEVICE_MAP {
    POBJECT_DIRECTORY DosDevicesDirectory;        // 0x000
    POBJECT_DIRECTORY GlobalDosDevicesDirectory;  // 0x008
    PEJOB ServerSilo;                             // 0x010
    PDEVICE_MAP GlobalDeviceMap;                  // 0x018
    EX_FAST_REF DriveObject[26];                  // 0x020
    LONGLONG ReferenceCount;                      // 0x0f0
    PVOID DosDevicesDirectoryHandle;              // 0x0f8
    ULONG DriveMap;                               // 0x100
    UCHAR DriveType[32];                          // 0x104
};

/*
nt!_SEP_CACHED_HANDLES_TABLE
   +0x000 Lock             : _EX_PUSH_LOCK
   +0x008 HashTable        : Ptr64 _RTL_DYNAMIC_HASH_TABLE
*/
typedef struct _SEP_CACHED_HANDLES_TABLE {
    EX_PUSH_LOCK Lock;                  // 0x000
    PRTL_DYNAMIC_HASH_TABLE HashTable;  // 0x008
} SEP_CACHED_HANDLES_TABLE, *PSEP_CACHED_HANDLES_TABLE;

/*
nt!_AUTHZBASEP_SECURITY_ATTRIBUTES_INFORMATION
   +0x000 SecurityAttributeCount : Uint4B
   +0x008 SecurityAttributesList : _LIST_ENTRY
   +0x018 WorkingSecurityAttributeCount : Uint4B
   +0x020 WorkingSecurityAttributesList : _LIST_ENTRY
*/
typedef struct _AUTHZBASEP_SECURITY_ATTRIBUTES_INFORMATION {
    ULONG SecurityAttributeCount;              // 0x000
    LIST_ENTRY SecurityAttributesList;         // 0x008
    ULONG WorkingSecurityAttributeCount;       // 0x018
    LIST_ENTRY WorkingSecurityAttributesList;  // 0x020
} AUTHZBASEP_SECURITY_ATTRIBUTES_INFORMATION, *PAUTHZBASEP_SECURITY_ATTRIBUTES_INFORMATION;

// struct _SID
// {
//     UCHAR Revision;                                                         //0x0
//     UCHAR SubAuthorityCount;                                                //0x1
//     struct _SID_IDENTIFIER_AUTHORITY IdentifierAuthority;                   //0x2
//     ULONG SubAuthority[1];                                                  //0x8
// };

/*
nt!_AUTHZBASEP_CLAIM_ATTRIBUTES_COLLECTION
   +0x000 DeviceGroupsCount : Uint4B
   +0x008 pDeviceGroups    : Ptr64 _SID_AND_ATTRIBUTES
   +0x010 RestrictedDeviceGroupsCount : Uint4B
   +0x018 pRestrictedDeviceGroups : Ptr64 _SID_AND_ATTRIBUTES
   +0x020 DeviceGroupsHash : _SID_AND_ATTRIBUTES_HASH
   +0x130 RestrictedDeviceGroupsHash : _SID_AND_ATTRIBUTES_HASH
   +0x240 pUserSecurityAttributes : Ptr64 _AUTHZBASEP_SECURITY_ATTRIBUTES_INFORMATION
   +0x248 pDeviceSecurityAttributes : Ptr64 _AUTHZBASEP_SECURITY_ATTRIBUTES_INFORMATION
   +0x250 pRestrictedUserSecurityAttributes : Ptr64 _AUTHZBASEP_SECURITY_ATTRIBUTES_INFORMATION
   +0x258 pRestrictedDeviceSecurityAttributes : Ptr64 _AUTHZBASEP_SECURITY_ATTRIBUTES_INFORMATION
*/
typedef struct _AUTHZBASEP_CLAIM_ATTRIBUTES_COLLECTION {
    ULONG DeviceGroupsCount;                                                        // 0x000
    PSID_AND_ATTRIBUTES pDeviceGroups;                                              // 0x008
    ULONG RestrictedDeviceGroupsCount;                                              // 0x010
    PSID_AND_ATTRIBUTES pRestrictedDeviceGroups;                                    // 0x018
    SID_AND_ATTRIBUTES_HASH DeviceGroupsHash;                                       // 0x020
    SID_AND_ATTRIBUTES_HASH RestrictedDeviceGroupsHash;                             // 0x130
    PAUTHZBASEP_SECURITY_ATTRIBUTES_INFORMATION pUserSecurityAttributes;            // 0x240
    PAUTHZBASEP_SECURITY_ATTRIBUTES_INFORMATION pDeviceSecurityAttributes;          // 0x248
    PAUTHZBASEP_SECURITY_ATTRIBUTES_INFORMATION pRestrictedUserSecurityAttributes;  // 0x250
    PAUTHZBASEP_SECURITY_ATTRIBUTES_INFORMATION
    pRestrictedDeviceSecurityAttributes;  // 0x258
} AUTHZBASEP_CLAIM_ATTRIBUTES_COLLECTION, *PAUTHZBASEP_CLAIM_ATTRIBUTES_COLLECTION;

/*
nt!_SEP_SID_VALUES_BLOCK
   +0x000 BlockLength      : Uint4B
   +0x008 ReferenceCount   : Int8B
   +0x010 SidCount         : Uint4B
   +0x018 SidValuesStart   : Uint8B
*/
typedef struct _SEP_SID_VALUES_BLOCK {
    ULONG BlockLength;         // 0x000
    LONGLONG ReferenceCount;   // 0x008
    ULONG SidCount;            // 0x010
    ULONGLONG SidValuesStart;  // 0x018
} SEP_SID_VALUES_BLOCK, *PSEP_SID_VALUES_BLOCK;

/*
nt!_OB_HANDLE_REVOCATION_BLOCK
   +0x000 RevocationInfos  : _LIST_ENTRY
   +0x010 Lock             : _EX_PUSH_LOCK
   +0x018 Rundown          : _EX_RUNDOWN_REF
*/
typedef struct _OB_HANDLE_REVOCATION_BLOCK {
    LIST_ENTRY RevocationInfos;  // 0x000
    _EX_PUSH_LOCK Lock;          // 0x010
    EX_RUNDOWN_REF Rundown;      // 0x018
} OB_HANDLE_REVOCATION_BLOCK, *POB_HANDLE_REVOCATION_BLOCK;

/*
nt!_SEP_LOGON_SESSION_REFERENCES
   +0x000 Next             : Ptr64 _SEP_LOGON_SESSION_REFERENCES
   +0x008 LogonId          : _LUID
   +0x010 BuddyLogonId     : _LUID
   +0x018 ReferenceCount   : Int8B
   +0x020 Flags            : Uint4B
   +0x028 pDeviceMap       : Ptr64 _DEVICE_MAP
   +0x030 Token            : Ptr64 Void
   +0x038 AccountName      : _UNICODE_STRING
   +0x048 AuthorityName    : _UNICODE_STRING
   +0x058 CachedHandlesTable : _SEP_CACHED_HANDLES_TABLE
   +0x068 SharedDataLock   : _EX_PUSH_LOCK
   +0x070 SharedClaimAttributes : Ptr64 _AUTHZBASEP_CLAIM_ATTRIBUTES_COLLECTION
   +0x078 SharedSidValues  : Ptr64 _SEP_SID_VALUES_BLOCK
   +0x080 RevocationBlock  : _OB_HANDLE_REVOCATION_BLOCK
   +0x0a0 ServerSilo       : Ptr64 _EJOB
   +0x0a8 SiblingAuthId    : _LUID
   +0x0b0 TokenList        : _LIST_ENTRY
*/
typedef struct _SEP_LOGON_SESSION_REFERENCES SEP_LOGON_SESSION_REFERENCES,
    *PSEP_LOGON_SESSION_REFERENCES;
struct _SEP_LOGON_SESSION_REFERENCES {
    PSEP_LOGON_SESSION_REFERENCES Next;                             // 0x000
    LUID LogonId;                                                   // 0x008
    LUID BuddyLogonId;                                              // 0x010
    LONGLONG ReferenceCount;                                        // 0x018
    ULONG Flags;                                                    // 0x020
    PDEVICE_MAP pDeviceMap;                                         // 0x028
    PVOID Token;                                                    // 0x030
    UNICODE_STRING AccountName;                                     // 0x038
    UNICODE_STRING AuthorityName;                                   // 0x048
    SEP_CACHED_HANDLES_TABLE CachedHandlesTable;                    // 0x058
    _EX_PUSH_LOCK SharedDataLock;                                   // 0x068
    PAUTHZBASEP_CLAIM_ATTRIBUTES_COLLECTION SharedClaimAttributes;  // 0x070
    PSEP_SID_VALUES_BLOCK SharedSidValues;                          // 0x078
    OB_HANDLE_REVOCATION_BLOCK RevocationBlock;                     // 0x080
    PEJOB ServerSilo;                                               // 0x0a0
    LUID SiblingAuthId;                                             // 0x0a8
    LIST_ENTRY TokenList;                                           // 0x0b0
};

/*
nt!_SEP_LOWBOX_NUMBER_ENTRY
   +0x000 HashEntry        : _RTL_DYNAMIC_HASH_TABLE_ENTRY
   +0x018 ReferenceCount   : Int8B
   +0x020 PackageSid       : Ptr64 Void
   +0x028 LowboxNumber     : Uint4B
   +0x030 AtomTable        : Ptr64 Void
*/
typedef struct _SEP_LOWBOX_NUMBER_ENTRY {
    RTL_DYNAMIC_HASH_TABLE_ENTRY HashEntry;  // 0x000
    LONGLONG ReferenceCount;                 // 0x018
    PVOID PackageSid;                        // 0x020
    ULONG LowboxNumber;                      // 0x028
    PVOID AtomTable;                         // 0x030
} SEP_LOWBOX_NUMBER_ENTRY, *PSEP_LOWBOX_NUMBER_ENTRY;

/*
nt!_SEP_CACHED_HANDLES_ENTRY_TYPE
   SepCachedHandlesEntryLowbox = 0n0
   SepCachedHandlesEntryBnoIsolation = 0n1
*/
typedef enum _SEP_CACHED_HANDLES_ENTRY_TYPE {
    SepCachedHandlesEntryLowbox = 0,
    SepCachedHandlesEntryBnoIsolation = 1
} SEP_CACHED_HANDLES_ENTRY_TYPE, *PSEP_CACHED_HANDLES_ENTRY_TYPE;

/*
nt!_SEP_CACHED_HANDLES_ENTRY_DESCRIPTOR
   +0x000 DescriptorType   : _SEP_CACHED_HANDLES_ENTRY_TYPE
   +0x008 PackageSid       : Ptr64 Void
   +0x008 IsolationPrefix  : _UNICODE_STRING
*/
typedef struct _SEP_CACHED_HANDLES_ENTRY_DESCRIPTOR {
    SEP_CACHED_HANDLES_ENTRY_TYPE DescriptorType;  // 0x000
    union {
        PVOID PackageSid;                // 0x008
        UNICODE_STRING IsolationPrefix;  // 0x008
    };
} SEP_CACHED_HANDLES_ENTRY_DESCRIPTOR, *PSEP_CACHED_HANDLES_ENTRY_DESCRIPTOR;

/*
nt!_SEP_CACHED_HANDLES_ENTRY
   +0x000 HashEntry        : _RTL_DYNAMIC_HASH_TABLE_ENTRY
   +0x018 ReferenceCount   : Int8B
   +0x020 EntryDescriptor  : _SEP_CACHED_HANDLES_ENTRY_DESCRIPTOR
   +0x038 HandleCount      : Uint4B
   +0x040 Handles          : Ptr64 Ptr64 Void
*/
typedef struct _SEP_CACHED_HANDLES_ENTRY {
    RTL_DYNAMIC_HASH_TABLE_ENTRY HashEntry;               // 0x000
    LONGLONG ReferenceCount;                              // 0x018
    SEP_CACHED_HANDLES_ENTRY_DESCRIPTOR EntryDescriptor;  // 0x020
    ULONG HandleCount;                                    // 0x038
    PVOID* Handles;                                       // 0x040
} SEP_CACHED_HANDLES_ENTRY, *PSEP_CACHED_HANDLES_ENTRY;

/*
nt!_SEP_LUID_TO_INDEX_MAP_ENTRY
   +0x000 HashEntry        : _RTL_DYNAMIC_HASH_TABLE_ENTRY
   +0x018 ReferenceCount   : Int8B
   +0x020 Luid             : Uint8B
   +0x028 IndexIntoGlobalSingletonTable : Uint8B
   +0x030 MarkedForDeletion : UChar
*/
typedef struct _SEP_LUID_TO_INDEX_MAP_ENTRY {
    RTL_DYNAMIC_HASH_TABLE_ENTRY HashEntry;   // 0x000
    LONGLONG ReferenceCount;                  // 0x018
    ULONGLONG Luid;                           // 0x020
    ULONGLONG IndexIntoGlobalSingletonTable;  // 0x028
    UCHAR MarkedForDeletion;                  // 0x030
} SEP_LUID_TO_INDEX_MAP_ENTRY, *PSEP_LUID_TO_INDEX_MAP_ENTRY;

/*
nt!_SEP_TOKEN_DIAG_TRACK_ENTRY
   +0x000 ProcessCid       : Ptr64 Void
   +0x008 ThreadCid        : Ptr64 Void
   +0x010 ImageFileName    : [16] UChar
   +0x020 CreateMethod     : Uint4B
   +0x028 CreateTrace      : [30] Uint8B
   +0x118 Count            : Int4B
   +0x11c CaptureCount     : Int4B
*/
typedef struct _SEP_TOKEN_DIAG_TRACK_ENTRY {
    PVOID ProcessCid;           // 0x000
    PVOID ThreadCid;            // 0x008
    UCHAR ImageFileName[16];    // 0x010
    ULONG CreateMethod;         // 0x020
    ULONGLONG CreateTrace[30];  // 0x028
    LONG Count;                 // 0x118
    LONG CaptureCount;          // 0x11c
} SEP_TOKEN_DIAG_TRACK_ENTRY, *PSEP_TOKEN_DIAG_TRACK_ENTRY;

/*
nt!_TOKEN
   +0x000 TokenSource      : _TOKEN_SOURCE
   +0x010 TokenId          : _LUID
   +0x018 AuthenticationId : _LUID
   +0x020 ParentTokenId    : _LUID
   +0x028 ExpirationTime   : _LARGE_INTEGER
   +0x030 TokenLock        : Ptr64 _ERESOURCE
   +0x038 ModifiedId       : _LUID
   +0x040 Privileges       : _SEP_TOKEN_PRIVILEGES
   +0x058 AuditPolicy      : _SEP_AUDIT_POLICY
   +0x078 SessionId        : Uint4B
   +0x07c UserAndGroupCount : Uint4B
   +0x080 RestrictedSidCount : Uint4B
   +0x084 VariableLength   : Uint4B
   +0x088 DynamicCharged   : Uint4B
   +0x08c DynamicAvailable : Uint4B
   +0x090 DefaultOwnerIndex : Uint4B
   +0x098 UserAndGroups    : Ptr64 _SID_AND_ATTRIBUTES
   +0x0a0 RestrictedSids   : Ptr64 _SID_AND_ATTRIBUTES
   +0x0a8 PrimaryGroup     : Ptr64 Void
   +0x0b0 DynamicPart      : Ptr64 Uint4B
   +0x0b8 DefaultDacl      : Ptr64 _ACL
   +0x0c0 TokenType        : _TOKEN_TYPE
   +0x0c4 ImpersonationLevel : _SECURITY_IMPERSONATION_LEVEL
   +0x0c8 TokenFlags       : Uint4B
   +0x0cc TokenInUse       : UChar
   +0x0d0 IntegrityLevelIndex : Uint4B
   +0x0d4 MandatoryPolicy  : Uint4B
   +0x0d8 LogonSession     : Ptr64 _SEP_LOGON_SESSION_REFERENCES
   +0x0e0 OriginatingLogonSession : _LUID
   +0x0e8 SidHash          : _SID_AND_ATTRIBUTES_HASH
   +0x1f8 RestrictedSidHash : _SID_AND_ATTRIBUTES_HASH
   +0x308 pSecurityAttributes : Ptr64 _AUTHZBASEP_SECURITY_ATTRIBUTES_INFORMATION
   +0x310 Package          : Ptr64 Void
   +0x318 Capabilities     : Ptr64 _SID_AND_ATTRIBUTES
   +0x320 CapabilityCount  : Uint4B
   +0x328 CapabilitiesHash : _SID_AND_ATTRIBUTES_HASH
   +0x438 LowboxNumberEntry : Ptr64 _SEP_LOWBOX_NUMBER_ENTRY
   +0x440 LowboxHandlesEntry : Ptr64 _SEP_CACHED_HANDLES_ENTRY
   +0x448 pClaimAttributes : Ptr64 _AUTHZBASEP_CLAIM_ATTRIBUTES_COLLECTION
   +0x450 TrustLevelSid    : Ptr64 Void
   +0x458 TrustLinkedToken : Ptr64 _TOKEN
   +0x460 IntegrityLevelSidValue : Ptr64 Void
   +0x468 TokenSidValues   : Ptr64 _SEP_SID_VALUES_BLOCK
   +0x470 IndexEntry       : Ptr64 _SEP_LUID_TO_INDEX_MAP_ENTRY
   +0x478 DiagnosticInfo   : Ptr64 _SEP_TOKEN_DIAG_TRACK_ENTRY
   +0x480 BnoIsolationHandlesEntry : Ptr64 _SEP_CACHED_HANDLES_ENTRY
   +0x488 SessionObject    : Ptr64 Void
   +0x490 VariablePart     : Uint8B
*/
typedef struct _TOKEN TOKEN, *PTOKEN;
struct _TOKEN {
    TOKEN_SOURCE TokenSource;                                         // 0x000
    LUID TokenId;                                                     // 0x010
    LUID AuthenticationId;                                            // 0x018
    LUID ParentTokenId;                                               // 0x020
    LARGE_INTEGER ExpirationTime;                                     // 0x028
    PERESOURCE TokenLock;                                             // 0x030
    LUID ModifiedId;                                                  // 0x038
    SEP_TOKEN_PRIVILEGES Privileges;                                  // 0x040
    SEP_AUDIT_POLICY AuditPolicy;                                     // 0x058
    ULONG SessionId;                                                  // 0x078
    ULONG UserAndGroupCount;                                          // 0x07c
    ULONG RestrictedSidCount;                                         // 0x080
    ULONG VariableLength;                                             // 0x084
    ULONG DynamicCharged;                                             // 0x088
    ULONG DynamicAvailable;                                           // 0x08c
    ULONG DefaultOwnerIndex;                                          // 0x090
    PSID_AND_ATTRIBUTES UserAndGroups;                                // 0x098
    PSID_AND_ATTRIBUTES RestrictedSids;                               // 0x0a0
    PVOID PrimaryGroup;                                               // 0x0a8
    PULONG DynamicPart;                                               // 0x0b0
    PACL DefaultDacl;                                                 // 0x0b8
    TOKEN_TYPE TokenType;                                             // 0x0c0
    SECURITY_IMPERSONATION_LEVEL ImpersonationLevel;                  // 0x0c4
    ULONG TokenFlags;                                                 // 0x0c8
    UCHAR TokenInUse;                                                 // 0x0cc
    ULONG IntegrityLevelIndex;                                        // 0x0d0
    ULONG MandatoryPolicy;                                            // 0x0d4
    PSEP_LOGON_SESSION_REFERENCES LogonSession;                       // 0x0d8
    LUID OriginatingLogonSession;                                     // 0x0e0
    SID_AND_ATTRIBUTES_HASH SidHash;                                  // 0x0e8
    SID_AND_ATTRIBUTES_HASH RestrictedSidHash;                        // 0x1f8
    PAUTHZBASEP_SECURITY_ATTRIBUTES_INFORMATION pSecurityAttributes;  // 0x308
    PVOID Package;                                                    // 0x310
    PSID_AND_ATTRIBUTES Capabilities;                                 // 0x318
    ULONG CapabilityCount;                                            // 0x320
    SID_AND_ATTRIBUTES_HASH CapabilitiesHash;                         // 0x328
    PSEP_LOWBOX_NUMBER_ENTRY LowboxNumberEntry;                       // 0x438
    PSEP_CACHED_HANDLES_ENTRY LowboxHandlesEntry;                     // 0x440
    PAUTHZBASEP_CLAIM_ATTRIBUTES_COLLECTION pClaimAttributes;         // 0x448
    PVOID TrustLevelSid;                                              // 0x450
    PTOKEN TrustLinkedToken;                                          // 0x458
    PVOID IntegrityLevelSidValue;                                     // 0x460
    PSEP_SID_VALUES_BLOCK TokenSidValues;                             // 0x468
    PSEP_LUID_TO_INDEX_MAP_ENTRY IndexEntry;                          // 0x470
    PSEP_TOKEN_DIAG_TRACK_ENTRY DiagnosticInfo;                       // 0x478
    PSEP_CACHED_HANDLES_ENTRY BnoIsolationHandlesEntry;               // 0x480
    PVOID SessionObject;                                              // 0x488
    ULONGLONG VariablePart;                                           // 0x490
};

/*
nt!_KPROCESS
   +0x000 Header           : _DISPATCHER_HEADER
   +0x018 ProfileListHead  : _LIST_ENTRY
   +0x028 DirectoryTableBase : Uint8B
   +0x030 ThreadListHead   : _LIST_ENTRY
   +0x040 ProcessLock      : Uint4B
   +0x044 ProcessTimerDelay : Uint4B
   +0x048 DeepFreezeStartTime : Uint8B
   +0x050 Affinity         : Ptr64 _KAFFINITY_EX
   +0x058 AutoBoostState   : _KAB_UM_PROCESS_CONTEXT
   +0x068 ReadyListHead    : _LIST_ENTRY
   +0x078 SwapListEntry    : _SINGLE_LIST_ENTRY
   +0x080 ActiveProcessors : Ptr64 _KAFFINITY_EX
   +0x088 AutoAlignment    : Pos 0, 1 Bit
   +0x088 DisableBoost     : Pos 1, 1 Bit
   +0x088 DisableQuantum   : Pos 2, 1 Bit
   +0x088 DeepFreeze       : Pos 3, 1 Bit
   +0x088 TimerVirtualization : Pos 4, 1 Bit
   +0x088 CheckStackExtents : Pos 5, 1 Bit
   +0x088 CacheIsolationEnabled : Pos 6, 1 Bit
   +0x088 PpmPolicy        : Pos 7, 4 Bits
   +0x088 VaSpaceDeleted   : Pos 11, 1 Bit
   +0x088 MultiGroup       : Pos 12, 1 Bit
   +0x088 ForegroundProcess : Pos 13, 1 Bit
   +0x088 ReservedFlags    : Pos 14, 18 Bits
   +0x088 ProcessFlags     : Int4B
   +0x08c Spare0c          : Uint4B
   +0x090 BasePriority     : Char
   +0x091 QuantumReset     : Char
   +0x092 Visited          : Char
   +0x093 Flags            : _KEXECUTE_OPTIONS
   +0x098 ActiveGroupsMask : _KGROUP_MASK
   +0x0a8 ActiveGroupPadding : [2] Uint8B
   +0x0b8 IdealProcessorAssignmentBlock : Ptr64 _KI_IDEAL_PROCESSOR_ASSIGNMENT_BLOCK
   +0x0c0 Padding          : [6] Uint8B
   +0x0f0 Padding2         : Uint4B
   +0x0f4 SchedulerAssistYieldBoostCount : Uint4B
   +0x0f8 SchedulerAssistYieldBoostAllowedTime : Int8B
   +0x100 Spare0d          : Uint4B
   +0x104 IdealGlobalNode  : Uint2B
   +0x106 Spare1           : Uint2B
   +0x108 StackCount       : _KSTACK_COUNT
   +0x110 ProcessListEntry : _LIST_ENTRY
   +0x120 CycleTime        : Uint8B
   +0x128 ContextSwitches  : Uint8B
   +0x130 SchedulingGroup  : Ptr64 _KSCHEDULING_GROUP
   +0x138 KernelTime       : Uint8B
   +0x140 UserTime         : Uint8B
   +0x148 ReadyTime        : Uint8B
   +0x150 FreezeCount      : Uint4B
   +0x154 Spare4           : Uint4B
   +0x158 UserDirectoryTableBase : Uint8B
   +0x160 AddressPolicy    : UChar
   +0x161 Spare2           : [7] UChar
   +0x168 InstrumentationCallback : Ptr64 Void
   +0x170 SecureState      : <unnamed-tag>
   +0x178 KernelWaitTime   : Uint8B
   +0x180 UserWaitTime     : Uint8B
   +0x188 LastRebalanceQpc : Uint8B
   +0x190 PerProcessorCycleTimes : Ptr64 Void
   +0x198 ExtendedFeatureDisableMask : Uint8B
   +0x1a0 PrimaryGroup     : Uint2B
   +0x1a2 Spare3           : [3] Uint2B
   +0x1a8 UserCetLogging   : Ptr64 Void
   +0x1b0 CpuPartitionList : _LIST_ENTRY
   +0x1c0 AvailableCpuState : Ptr64 _KPROCESS_AVAILABLE_CPU_STATE
*/
typedef struct _KPROCESS {
    DISPATCHER_HEADER Header;               // 0x000
    LIST_ENTRY ProfileListHead;             // 0x018
    ULONG_PTR DirectoryTableBase;           // 0x028
    LIST_ENTRY ThreadListHead;              // 0x030
    ULONG ProcessLock;                      // 0x040
    ULONG ProcessTimerDelay;                // 0x044
    ULONGLONG DeepFreezeStartTime;          // 0x048
    PKAFFINITY_EX Affinity;                 // 0x050
    KAB_UM_PROCESS_CONTEXT AutoBoostState;  // 0x058
    LIST_ENTRY ReadyListHead;               // 0x068
    SINGLE_LIST_ENTRY SwapListEntry;        // 0x078
    PKAFFINITY_EX ActiveProcessors;         // 0x080
    union {
        struct {
            ULONG AutoAlignment : 1;          // 0x088
            ULONG DisableBoost : 1;           // 0x088
            ULONG DisableQuantum : 1;         // 0x088
            ULONG DeepFreeze : 1;             // 0x088
            ULONG TimerVirtualization : 1;    // 0x088
            ULONG CheckStackExtents : 1;      // 0x088
            ULONG CacheIsolationEnabled : 1;  // 0x088
            ULONG PpmPolicy : 4;              // 0x088
            ULONG VaSpaceDeleted : 1;         // 0x088
            ULONG MultiGroup : 1;             // 0x088
            ULONG ForegroundProcess : 1;      // 0x088
            ULONG ReservedFlags : 18;         // 0x088
        };
        volatile LONG ProcessFlags;  // 0x088
    };
    ULONG Spare0c;                                                       // 0x08c
    CHAR BasePriority;                                                   // 0x090
    CHAR QuantumReset;                                                   // 0x091
    CHAR Visited;                                                        // 0x092
    KEXECUTE_OPTIONS Flags;                                              // 0x093
    KGROUP_MASK ActiveGroupsMask;                                        // 0x098
    ULONGLONG ActiveGroupPadding[2];                                     // 0x0a8
    PKI_IDEAL_PROCESSOR_ASSIGNMENT_BLOCK IdealProcessorAssignmentBlock;  // 0x0b8
    ULONGLONG Padding[8];                                                // 0x0c0
    ULONG Spare0d;                                                       // 0x100
    USHORT IdealGlobalNode;                                              // 0x104
    USHORT Spare1;                                                       // 0x106
    volatile KSTACK_COUNT StackCount;                                    // 0x108
    LIST_ENTRY ProcessListEntry;                                         // 0x110
    ULONGLONG CycleTime;                                                 // 0x120
    ULONGLONG ContextSwitches;                                           // 0x128
    PKSCHEDULING_GROUP SchedulingGroup;                                  // 0x130
    ULONGLONG KernelTime;                                                // 0x138
    ULONGLONG UserTime;                                                  // 0x140
    ULONGLONG ReadyTime;                                                 // 0x148
    ULONG FreezeCount;                                                   // 0x150
    ULONG Spare4;                                                        // 0x154
    ULONG_PTR UserDirectoryTableBase;                                    // 0x158
    UCHAR AddressPolicy;                                                 // 0x160
    UCHAR Spare2[7];                                                     // 0x161
    PVOID InstrumentationCallback;                                       // 0x168
    union {
        ULONG_PTR SecureHandle;  // 0x170
        struct {
            ULONGLONG SecureProcess : 1;              // 0x170
            ULONGLONG TrustedApp : 1;                 // 0x170
        } Flags;                                      // 0x170
    } SecureState;                                    // 0x170
    ULONGLONG KernelWaitTime;                         // 0x178
    ULONGLONG UserWaitTime;                           // 0x180
    ULONGLONG LastRebalanceQpc;                       // 0x188
    PVOID PerProcessorCycleTimes;                     // 0x190
    ULONGLONG ExtendedFeatureDisableMask;             // 0x198
    USHORT PrimaryGroup;                              // 0x1a0
    USHORT Spare3[3];                                 // 0x1a2
    PVOID UserCetLogging;                             // 0x1a8
    LIST_ENTRY CpuPartitionList;                      // 0x1b0
    PKPROCESS_AVAILABLE_CPU_STATE AvailableCpuState;  // 0x1c0
} KPROCESS, *PKPROCESS;

/*
nt!_EPROCESS
   +0x000 Pcb              : _KPROCESS
   +0x1c8 ProcessLock      : _EX_PUSH_LOCK
   +0x1d0 UniqueProcessId  : Ptr64 Void
   +0x1d8 ActiveProcessLinks : _LIST_ENTRY
   +0x1e8 RundownProtect   : _EX_RUNDOWN_REF
   +0x1f0 Flags2           : Uint4B
   +0x1f0 JobNotReallyActive : Pos 0, 1 Bit
   +0x1f0 AccountingFolded : Pos 1, 1 Bit
   +0x1f0 NewProcessReported : Pos 2, 1 Bit
   +0x1f0 ExitProcessReported : Pos 3, 1 Bit
   +0x1f0 ReportCommitChanges : Pos 4, 1 Bit
   +0x1f0 LastReportMemory : Pos 5, 1 Bit
   +0x1f0 ForceWakeCharge  : Pos 6, 1 Bit
   +0x1f0 CrossSessionCreate : Pos 7, 1 Bit
   +0x1f0 NeedsHandleRundown : Pos 8, 1 Bit
   +0x1f0 RefTraceEnabled  : Pos 9, 1 Bit
   +0x1f0 PicoCreated      : Pos 10, 1 Bit
   +0x1f0 EmptyJobEvaluated : Pos 11, 1 Bit
   +0x1f0 DefaultPagePriority : Pos 12, 3 Bits
   +0x1f0 PrimaryTokenFrozen : Pos 15, 1 Bit
   +0x1f0 ProcessVerifierTarget : Pos 16, 1 Bit
   +0x1f0 RestrictSetThreadContext : Pos 17, 1 Bit
   +0x1f0 AffinityPermanent : Pos 18, 1 Bit
   +0x1f0 AffinityUpdateEnable : Pos 19, 1 Bit
   +0x1f0 PropagateNode    : Pos 20, 1 Bit
   +0x1f0 ExplicitAffinity : Pos 21, 1 Bit
   +0x1f0 Flags2Available1 : Pos 22, 2 Bits
   +0x1f0 EnableReadVmLogging : Pos 24, 1 Bit
   +0x1f0 EnableWriteVmLogging : Pos 25, 1 Bit
   +0x1f0 FatalAccessTerminationRequested : Pos 26, 1 Bit
   +0x1f0 DisableSystemAllowedCpuSet : Pos 27, 1 Bit
   +0x1f0 Flags2Available2 : Pos 28, 3 Bits
   +0x1f0 InPrivate        : Pos 31, 1 Bit
   +0x1f4 Flags            : Uint4B
   +0x1f4 CreateReported   : Pos 0, 1 Bit
   +0x1f4 NoDebugInherit   : Pos 1, 1 Bit
   +0x1f4 ProcessExiting   : Pos 2, 1 Bit
   +0x1f4 ProcessDelete    : Pos 3, 1 Bit
   +0x1f4 ManageExecutableMemoryWrites : Pos 4, 1 Bit
   +0x1f4 VmDeleted        : Pos 5, 1 Bit
   +0x1f4 OutswapEnabled   : Pos 6, 1 Bit
   +0x1f4 Outswapped       : Pos 7, 1 Bit
   +0x1f4 FailFastOnCommitFail : Pos 8, 1 Bit
   +0x1f4 Wow64VaSpace4Gb  : Pos 9, 1 Bit
   +0x1f4 AddressSpaceInitialized : Pos 10, 2 Bits
   +0x1f4 SetTimerResolution : Pos 12, 1 Bit
   +0x1f4 BreakOnTermination : Pos 13, 1 Bit
   +0x1f4 DeprioritizeViews : Pos 14, 1 Bit
   +0x1f4 WriteWatch       : Pos 15, 1 Bit
   +0x1f4 ProcessInSession : Pos 16, 1 Bit
   +0x1f4 OverrideAddressSpace : Pos 17, 1 Bit
   +0x1f4 HasAddressSpace  : Pos 18, 1 Bit
   +0x1f4 LaunchPrefetched : Pos 19, 1 Bit
   +0x1f4 Reserved         : Pos 20, 1 Bit
   +0x1f4 VmTopDown        : Pos 21, 1 Bit
   +0x1f4 ImageNotifyDone  : Pos 22, 1 Bit
   +0x1f4 PdeUpdateNeeded  : Pos 23, 1 Bit
   +0x1f4 VdmAllowed       : Pos 24, 1 Bit
   +0x1f4 ProcessRundown   : Pos 25, 1 Bit
   +0x1f4 ProcessInserted  : Pos 26, 1 Bit
   +0x1f4 DefaultIoPriority : Pos 27, 3 Bits
   +0x1f4 ProcessSelfDelete : Pos 30, 1 Bit
   +0x1f4 SetTimerResolutionLink : Pos 31, 1 Bit
   +0x1f8 CreateTime       : _LARGE_INTEGER
   +0x200 ProcessQuotaUsage : [2] Uint8B
   +0x210 ProcessQuotaPeak : [2] Uint8B
   +0x220 PeakVirtualSize  : Uint8B
   +0x228 VirtualSize      : Uint8B
   +0x230 SessionProcessLinks : _LIST_ENTRY
   +0x240 ExceptionPortData : Ptr64 Void
   +0x240 ExceptionPortValue : Uint8B
   +0x240 ExceptionPortState : Pos 0, 3 Bits
   +0x248 Token            : _EX_FAST_REF
   +0x250 MmReserved       : Uint8B
   +0x258 AddressCreationLock : _EX_PUSH_LOCK
   +0x260 PageTableCommitmentLock : _EX_PUSH_LOCK
   +0x268 RotateInProgress : Ptr64 _ETHREAD
   +0x270 ForkInProgress   : Ptr64 _ETHREAD
   +0x278 CommitChargeJob  : Ptr64 _EJOB
   +0x280 CloneRoot        : _RTL_AVL_TREE
   +0x288 NumberOfPrivatePages : Uint8B
   +0x290 NumberOfLockedPages : Uint8B
   +0x298 Win32Process     : Ptr64 Void
   +0x2a0 Job              : Ptr64 _EJOB
   +0x2a8 SectionObject    : Ptr64 Void
   +0x2b0 SectionBaseAddress : Ptr64 Void
   +0x2b8 Cookie           : Uint4B
   +0x2c0 WorkingSetWatch  : Ptr64 _PAGEFAULT_HISTORY
   +0x2c8 Win32WindowStation : Ptr64 Void
   +0x2d0 InheritedFromUniqueProcessId : Ptr64 Void
   +0x2d8 OwnerProcessId   : Uint8B
   +0x2e0 Peb              : Ptr64 _PEB
   +0x2e8 Session          : Ptr64 _PSP_SESSION_SPACE
   +0x2f0 Spare1           : Ptr64 Void
   +0x2f8 QuotaBlock       : Ptr64 _EPROCESS_QUOTA_BLOCK
   +0x300 ObjectTable      : Ptr64 _HANDLE_TABLE
   +0x308 DebugPort        : Ptr64 Void
   +0x310 WoW64Process     : Ptr64 _EWOW64PROCESS
   +0x318 DeviceMap        : _EX_FAST_REF
   +0x320 EtwDataSource    : Ptr64 Void
   +0x328 PageDirectoryPte : Uint8B
   +0x330 ImageFilePointer : Ptr64 _FILE_OBJECT
   +0x338 ImageFileName    : [15] UChar
   +0x347 PriorityClass    : UChar
   +0x348 SecurityPort     : Ptr64 Void
   +0x350 SeAuditProcessCreationInfo : _SE_AUDIT_PROCESS_CREATION_INFO
   +0x358 JobLinks         : _LIST_ENTRY
   +0x368 HighestUserAddress : Ptr64 Void
   +0x370 ThreadListHead   : _LIST_ENTRY
   +0x380 ActiveThreads    : Uint4B
   +0x384 ImagePathHash    : Uint4B
   +0x388 DefaultHardErrorProcessing : Uint4B
   +0x38c LastThreadExitStatus : Int4B
   +0x390 PrefetchTrace    : _EX_FAST_REF
   +0x398 LockedPagesList  : Ptr64 Void
   +0x3a0 ReadOperationCount : _LARGE_INTEGER
   +0x3a8 WriteOperationCount : _LARGE_INTEGER
   +0x3b0 OtherOperationCount : _LARGE_INTEGER
   +0x3b8 ReadTransferCount : _LARGE_INTEGER
   +0x3c0 WriteTransferCount : _LARGE_INTEGER
   +0x3c8 OtherTransferCount : _LARGE_INTEGER
   +0x3d0 CommitChargeLimit : Uint8B
   +0x3d8 CommitCharge     : Uint8B
   +0x3e0 CommitChargePeak : Uint8B
   +0x400 Vm               : _MMSUPPORT_FULL
   +0x540 MmProcessLinks   : _LIST_ENTRY
   +0x550 ModifiedPageCount : Uint4B
   +0x554 ExitStatus       : Int4B
   +0x558 VadRoot          : _RTL_AVL_TREE
   +0x560 VadHint          : Ptr64 Void
   +0x568 VadCount         : Uint8B
   +0x570 VadPhysicalPages : Uint8B
   +0x578 VadPhysicalPagesLimit : Uint8B
   +0x580 AlpcContext      : _ALPC_PROCESS_CONTEXT
   +0x5a0 TimerResolutionLink : _LIST_ENTRY
   +0x5b0 TimerResolutionStackRecord : Ptr64 _PO_DIAG_STACK_RECORD
   +0x5b8 RequestedTimerResolution : Uint4B
   +0x5bc SmallestTimerResolution : Uint4B
   +0x5c0 ExitTime         : _LARGE_INTEGER
   +0x5c8 InvertedFunctionTable : Ptr64 _INVERTED_FUNCTION_TABLE_KERNEL_MODE
   +0x5d0 InvertedFunctionTableLock : _EX_PUSH_LOCK
   +0x5d8 ActiveThreadsHighWatermark : Uint4B
   +0x5dc LargePrivateVadCount : Uint4B
   +0x5e0 ThreadListLock   : _EX_PUSH_LOCK
   +0x5e8 WnfContext       : Ptr64 Void
   +0x5f0 ServerSilo       : Ptr64 _EJOB
   +0x5f8 SignatureLevel   : UChar
   +0x5f9 SectionSignatureLevel : UChar
   +0x5fa Protection       : _PS_PROTECTION
   +0x5fb HangCount        : Pos 0, 3 Bits
   +0x5fb GhostCount       : Pos 3, 3 Bits
   +0x5fb PrefilterException : Pos 6, 1 Bit
   +0x5fc Flags3           : Uint4B
   +0x5fc Minimal          : Pos 0, 1 Bit
   +0x5fc ReplacingPageRoot : Pos 1, 1 Bit
   +0x5fc Crashed          : Pos 2, 1 Bit
   +0x5fc JobVadsAreTracked : Pos 3, 1 Bit
   +0x5fc VadTrackingDisabled : Pos 4, 1 Bit
   +0x5fc AuxiliaryProcess : Pos 5, 1 Bit
   +0x5fc SubsystemProcess : Pos 6, 1 Bit
   +0x5fc IndirectCpuSets  : Pos 7, 1 Bit
   +0x5fc RelinquishedCommit : Pos 8, 1 Bit
   +0x5fc HighGraphicsPriority : Pos 9, 1 Bit
   +0x5fc CommitFailLogged : Pos 10, 1 Bit
   +0x5fc ReserveFailLogged : Pos 11, 1 Bit
   +0x5fc SystemProcess    : Pos 12, 1 Bit
   +0x5fc AllImagesAtBasePristineBase : Pos 13, 1 Bit
   +0x5fc AddressPolicyFrozen : Pos 14, 1 Bit
   +0x5fc ProcessFirstResume : Pos 15, 1 Bit
   +0x5fc ForegroundExternal : Pos 16, 1 Bit
   +0x5fc ForegroundSystem : Pos 17, 1 Bit
   +0x5fc HighMemoryPriority : Pos 18, 1 Bit
   +0x5fc EnableProcessSuspendResumeLogging : Pos 19, 1 Bit
   +0x5fc EnableThreadSuspendResumeLogging : Pos 20, 1 Bit
   +0x5fc SecurityDomainChanged : Pos 21, 1 Bit
   +0x5fc SecurityFreezeComplete : Pos 22, 1 Bit
   +0x5fc VmProcessorHost  : Pos 23, 1 Bit
   +0x5fc VmProcessorHostTransition : Pos 24, 1 Bit
   +0x5fc AltSyscall       : Pos 25, 1 Bit
   +0x5fc TimerResolutionIgnore : Pos 26, 1 Bit
   +0x5fc DisallowUserTerminate : Pos 27, 1 Bit
   +0x5fc EnableProcessRemoteExecProtectVmLogging : Pos 28, 1 Bit
   +0x5fc EnableProcessLocalExecProtectVmLogging : Pos 29, 1 Bit
   +0x5fc MemoryCompressionProcess : Pos 30, 1 Bit
   +0x5fc EnableProcessImpersonationLogging : Pos 31, 1 Bit
   +0x600 DeviceAsid       : Int4B
   +0x608 SvmData          : Ptr64 Void
   +0x610 SvmProcessLock   : _EX_PUSH_LOCK
   +0x618 SvmLock          : Uint8B
   +0x620 SvmProcessDeviceListHead : _LIST_ENTRY
   +0x630 LastFreezeInterruptTime : Uint8B
   +0x638 DiskCounters     : Ptr64 _PROCESS_DISK_COUNTERS
   +0x640 PicoContext      : Ptr64 Void
   +0x648 EnclaveTable     : Ptr64 Void
   +0x650 EnclaveNumber    : Uint8B
   +0x658 EnclaveLock      : _EX_PUSH_LOCK
   +0x660 HighPriorityFaultsAllowed : Uint4B
   +0x668 EnergyContext    : Ptr64 _PO_PROCESS_ENERGY_CONTEXT
   +0x670 VmContext        : Ptr64 Void
   +0x678 SequenceNumber   : Uint8B
   +0x680 CreateInterruptTime : Uint8B
   +0x688 CreateUnbiasedInterruptTime : Uint8B
   +0x690 TotalUnbiasedFrozenTime : Uint8B
   +0x698 LastAppStateUpdateTime : Uint8B
   +0x6a0 LastAppStateUptime : Pos 0, 61 Bits
   +0x6a0 LastAppState     : Pos 61, 3 Bits
   +0x6a8 SharedCommitCharge : Uint8B
   +0x6b0 SharedCommitLock : _EX_PUSH_LOCK
   +0x6b8 SharedCommitLinks : _LIST_ENTRY
   +0x6c8 AllowedCpuSets   : Uint8B
   +0x6d0 DefaultCpuSets   : Uint8B
   +0x6c8 AllowedCpuSetsIndirect : Ptr64 Uint8B
   +0x6d0 DefaultCpuSetsIndirect : Ptr64 Uint8B
   +0x6d8 DiskIoAttribution : Ptr64 Void
   +0x6e0 DxgProcess       : Ptr64 Void
   +0x6e8 Win32KFilterSet  : Uint4B
   +0x6ec Machine          : Uint2B
   +0x6ee MmSlabIdentity   : UChar
   +0x6ef Spare0           : UChar
   +0x6f0 ProcessTimerDelay : _PS_INTERLOCKED_TIMER_DELAY_VALUES
   +0x6f8 KTimerSets       : Uint4B
   +0x6fc KTimer2Sets      : Uint4B
   +0x700 ThreadTimerSets  : Uint4B
   +0x708 VirtualTimerListLock : Uint8B
   +0x710 VirtualTimerListHead : _LIST_ENTRY
   +0x720 WakeChannel      : _WNF_STATE_NAME
   +0x720 WakeInfo         : _PS_PROCESS_WAKE_INFORMATION
   +0x750 MitigationFlags  : Uint4B
   +0x750 MitigationFlagsValues : <unnamed-tag>
   +0x754 MitigationFlags2 : Uint4B
   +0x754 MitigationFlags2Values : <unnamed-tag>
   +0x758 PartitionObject  : Ptr64 Void
   +0x760 SecurityDomain   : Uint8B
   +0x768 ParentSecurityDomain : Uint8B
   +0x770 CoverageSamplerContext : Ptr64 Void
   +0x778 MmHotPatchContext : Ptr64 Void
   +0x780 DynamicEHContinuationTargetsTree : _RTL_AVL_TREE
   +0x788 DynamicEHContinuationTargetsLock : _EX_PUSH_LOCK
   +0x790 DynamicEnforcedCetCompatibleRanges : _PS_DYNAMIC_ENFORCED_ADDRESS_RANGES
   +0x7a0 DisabledComponentFlags : Uint4B
   +0x7a4 PageCombineSequence : Int4B
   +0x7a8 EnableOptionalXStateFeaturesLock : _EX_PUSH_LOCK
   +0x7b0 PathRedirectionHashes : Ptr64 Uint4B
   +0x7b8 SyscallProvider  : Ptr64 _PS_SYSCALL_PROVIDER
   +0x7c0 SyscallProviderProcessLinks : _LIST_ENTRY
   +0x7d0 SyscallProviderDispatchContext : _PSP_SYSCALL_PROVIDER_DISPATCH_CONTEXT
   +0x7d8 MitigationFlags3 : Uint4B
   +0x7d8 MitigationFlags3Values : <unnamed-tag>
   +0x7dc Flags4           : Uint4B
   +0x7dc ThreadWasActive  : Pos 0, 1 Bit
   +0x7dc MinimalTerminate : Pos 1, 1 Bit
   +0x7dc ImageExpansionDisable : Pos 2, 1 Bit
   +0x7dc SessionFirstProcess : Pos 3, 1 Bit
   +0x7e0 SyscallUsage     : Uint4B
   +0x7e0 SyscallUsageValues : <unnamed-tag>
   +0x7e4 SupervisorDeviceAsid : Int4B
   +0x7e8 SupervisorSvmData : Ptr64 Void
   +0x7f0 NetworkCounters  : Ptr64 _PROCESS_NETWORK_COUNTERS
   +0x7f8 Execution        : _PROCESS_EXECUTION
   +0x800 ThreadIndexTable : Ptr64 Void
   +0x808 FreezeWorkLinks  : _LIST_ENTRY
*/
typedef struct _EPROCESS {
    KPROCESS Pcb;                   // 0x000
    _EX_PUSH_LOCK ProcessLock;      // 0x1c8
    HANDLE UniqueProcessId;         // 0x1d0
    LIST_ENTRY ActiveProcessLinks;  // 0x1d8
    EX_RUNDOWN_REF RundownProtect;  // 0x1e8
    union {
        ULONG Flags2;  // 0x1f0
        struct {
            ULONG JobNotReallyActive : 1;               // 0x1f0
            ULONG AccountingFolded : 1;                 // 0x1f0
            ULONG NewProcessReported : 1;               // 0x1f0
            ULONG ExitProcessReported : 1;              // 0x1f0
            ULONG ReportCommitChanges : 1;              // 0x1f0
            ULONG LastReportMemory : 1;                 // 0x1f0
            ULONG ForceWakeCharge : 1;                  // 0x1f0
            ULONG CrossSessionCreate : 1;               // 0x1f0
            ULONG NeedsHandleRundown : 1;               // 0x1f0
            ULONG RefTraceEnabled : 1;                  // 0x1f0
            ULONG PicoCreated : 1;                      // 0x1f0
            ULONG EmptyJobEvaluated : 1;                // 0x1f0
            ULONG DefaultPagePriority : 3;              // 0x1f0
            ULONG PrimaryTokenFrozen : 1;               // 0x1f0
            ULONG ProcessVerifierTarget : 1;            // 0x1f0
            ULONG RestrictSetThreadContext : 1;         // 0x1f0
            ULONG AffinityPermanent : 1;                // 0x1f0
            ULONG AffinityUpdateEnable : 1;             // 0x1f0
            ULONG PropagateNode : 1;                    // 0x1f0
            ULONG ExplicitAffinity : 1;                 // 0x1f0
            ULONG Flags2Available1 : 2;                 // 0x1f0
            ULONG EnableReadVmLogging : 1;              // 0x1f0
            ULONG EnableWriteVmLogging : 1;             // 0x1f0
            ULONG FatalAccessTerminationRequested : 1;  // 0x1f0
            ULONG DisableSystemAllowedCpuSet : 1;       // 0x1f0
            ULONG Flags2Available2 : 3;                 // 0x1f0
            ULONG InPrivate : 1;                        // 0x1f0
        };
    };
    union {
        ULONG Flags;  // 0x1f4
        struct {
            ULONG CreateReported : 1;                // 0x1f4
            ULONG NoDebugInherit : 1;                // 0x1f4
            ULONG ProcessExiting : 1;                // 0x1f4
            ULONG ProcessDelete : 1;                 // 0x1f4
            ULONG ManageExecutableMemoryWrites : 1;  // 0x1f4
            ULONG VmDeleted : 1;                     // 0x1f4
            ULONG OutswapEnabled : 1;                // 0x1f4
            ULONG Outswapped : 1;                    // 0x1f4
            ULONG FailFastOnCommitFail : 1;          // 0x1f4
            ULONG Wow64VaSpace4Gb : 1;               // 0x1f4
            ULONG AddressSpaceInitialized : 2;       // 0x1f4
            ULONG SetTimerResolution : 1;            // 0x1f4
            ULONG BreakOnTermination : 1;            // 0x1f4
            ULONG DeprioritizeViews : 1;             // 0x1f4
            ULONG WriteWatch : 1;                    // 0x1f4
            ULONG ProcessInSession : 1;              // 0x1f4
            ULONG OverrideAddressSpace : 1;          // 0x1f4
            ULONG HasAddressSpace : 1;               // 0x1f4
            ULONG LaunchPrefetched : 1;              // 0x1f4
            ULONG Reserved : 1;                      // 0x1f4
            ULONG VmTopDown : 1;                     // 0x1f4
            ULONG ImageNotifyDone : 1;               // 0x1f4
            ULONG PdeUpdateNeeded : 1;               // 0x1f4
            ULONG VdmAllowed : 1;                    // 0x1f4
            ULONG ProcessRundown : 1;                // 0x1f4
            ULONG ProcessInserted : 1;               // 0x1f4
            ULONG DefaultIoPriority : 3;             // 0x1f4
            ULONG ProcessSelfDelete : 1;             // 0x1f4
            ULONG SetTimerResolutionLink : 1;        // 0x1f4
        };
    };
    LARGE_INTEGER CreateTime;        // 0x1f8
    ULONGLONG ProcessQuotaUsage[2];  // 0x200
    ULONGLONG ProcessQuotaPeak[2];   // 0x210
    ULONGLONG PeakVirtualSize;       // 0x220
    ULONGLONG VirtualSize;           // 0x228
    LIST_ENTRY SessionProcessLinks;  // 0x230
    union {
        PVOID ExceptionPortData;           // 0x240
        ULONGLONG ExceptionPortValue;      // 0x240
        ULONGLONG ExceptionPortState : 3;  // 0x240
    };
    EX_FAST_REF Token;                                           // 0x248
    ULONGLONG MmReserved;                                        // 0x250
    _EX_PUSH_LOCK AddressCreationLock;                           // 0x258
    _EX_PUSH_LOCK PageTableCommitmentLock;                       // 0x260
    PETHREAD RotateInProgress;                                   // 0x268
    PETHREAD ForkInProgress;                                     // 0x270
    PEJOB volatile CommitChargeJob;                              // 0x278
    RTL_AVL_TREE CloneRoot;                                      // 0x280
    volatile ULONGLONG NumberOfPrivatePages;                     // 0x288
    volatile ULONGLONG NumberOfLockedPages;                      // 0x290
    PVOID Win32Process;                                          // 0x298
    PEJOB volatile Job;                                          // 0x2a0
    PVOID SectionObject;                                         // 0x2a8
    PVOID SectionBaseAddress;                                    // 0x2b0
    ULONG Cookie;                                                // 0x2b8
    PPAGEFAULT_HISTORY WorkingSetWatch;                          // 0x2c0
    PVOID Win32WindowStation;                                    // 0x2c8
    HANDLE InheritedFromUniqueProcessId;                         // 0x2d0
    volatile ULONGLONG OwnerProcessId;                           // 0x2d8
    PPEB Peb;                                                    // 0x2e0
    struct _PSP_SESSION_SPACE* Session;                          // 0x2e8
    PVOID Spare1;                                                // 0x2f0
    PEPROCESS_QUOTA_BLOCK QuotaBlock;                            // 0x2f8
    PHANDLE_TABLE ObjectTable;                                   // 0x300
    PVOID DebugPort;                                             // 0x308
    PEWOW64PROCESS WoW64Process;                                 // 0x310
    EX_FAST_REF DeviceMap;                                       // 0x318
    PVOID EtwDataSource;                                         // 0x320
    ULONGLONG PageDirectoryPte;                                  // 0x328
    PFILE_OBJECT ImageFilePointer;                               // 0x330
    UCHAR ImageFileName[15];                                     // 0x338
    UCHAR PriorityClass;                                         // 0x347
    PVOID SecurityPort;                                          // 0x348
    SE_AUDIT_PROCESS_CREATION_INFO SeAuditProcessCreationInfo;   // 0x350
    LIST_ENTRY JobLinks;                                         // 0x358
    PVOID HighestUserAddress;                                    // 0x368
    LIST_ENTRY ThreadListHead;                                   // 0x370
    volatile ULONG ActiveThreads;                                // 0x380
    ULONG ImagePathHash;                                         // 0x384
    ULONG DefaultHardErrorProcessing;                            // 0x388
    LONG LastThreadExitStatus;                                   // 0x38c
    EX_FAST_REF PrefetchTrace;                                   // 0x390
    PVOID LockedPagesList;                                       // 0x398
    LARGE_INTEGER ReadOperationCount;                            // 0x3a0
    LARGE_INTEGER WriteOperationCount;                           // 0x3a8
    LARGE_INTEGER OtherOperationCount;                           // 0x3b0
    LARGE_INTEGER ReadTransferCount;                             // 0x3b8
    LARGE_INTEGER WriteTransferCount;                            // 0x3c0
    LARGE_INTEGER OtherTransferCount;                            // 0x3c8
    ULONGLONG CommitChargeLimit;                                 // 0x3d0
    volatile ULONGLONG CommitCharge;                             // 0x3d8
    volatile ULONGLONG CommitChargePeak;                         // 0x3e0
    MMSUPPORT_FULL Vm;                                           // 0x400
    LIST_ENTRY MmProcessLinks;                                   // 0x540
    volatile ULONG ModifiedPageCount;                            // 0x550
    LONG ExitStatus;                                             // 0x554
    RTL_AVL_TREE VadRoot;                                        // 0x558
    PVOID VadHint;                                               // 0x560
    ULONGLONG VadCount;                                          // 0x568
    volatile ULONGLONG VadPhysicalPages;                         // 0x570
    ULONGLONG VadPhysicalPagesLimit;                             // 0x578
    ALPC_PROCESS_CONTEXT AlpcContext;                            // 0x580
    LIST_ENTRY TimerResolutionLink;                              // 0x5a0
    PPO_DIAG_STACK_RECORD TimerResolutionStackRecord;            // 0x5b0
    ULONG RequestedTimerResolution;                              // 0x5b8
    ULONG SmallestTimerResolution;                               // 0x5bc
    LARGE_INTEGER ExitTime;                                      // 0x5c0
    PINVERTED_FUNCTION_TABLE_KERNEL_MODE InvertedFunctionTable;  // 0x5c8
    _EX_PUSH_LOCK InvertedFunctionTableLock;                     // 0x5d0
    ULONG ActiveThreadsHighWatermark;                            // 0x5d8
    ULONG LargePrivateVadCount;                                  // 0x5dc
    _EX_PUSH_LOCK ThreadListLock;                                // 0x5e0
    PVOID WnfContext;                                            // 0x5e8
    PEJOB ServerSilo;                                            // 0x5f0
    UCHAR SignatureLevel;                                        // 0x5f8
    UCHAR SectionSignatureLevel;                                 // 0x5f9
    PS_PROTECTION Protection;                                    // 0x5fa
    UCHAR HangCount : 3;                                         // 0x5fb
    UCHAR GhostCount : 3;                                        // 0x5fb
    UCHAR PrefilterException : 1;                                // 0x5fb
    union {
        ULONG Flags3;  // 0x5fc
        struct {
            ULONG Minimal : 1;                                  // 0x5fc
            ULONG ReplacingPageRoot : 1;                        // 0x5fc
            ULONG Crashed : 1;                                  // 0x5fc
            ULONG JobVadsAreTracked : 1;                        // 0x5fc
            ULONG VadTrackingDisabled : 1;                      // 0x5fc
            ULONG AuxiliaryProcess : 1;                         // 0x5fc
            ULONG SubsystemProcess : 1;                         // 0x5fc
            ULONG IndirectCpuSets : 1;                          // 0x5fc
            ULONG RelinquishedCommit : 1;                       // 0x5fc
            ULONG HighGraphicsPriority : 1;                     // 0x5fc
            ULONG CommitFailLogged : 1;                         // 0x5fc
            ULONG ReserveFailLogged : 1;                        // 0x5fc
            ULONG SystemProcess : 1;                            // 0x5fc
            ULONG AllImagesAtBasePristineBase : 1;              // 0x5fc
            ULONG AddressPolicyFrozen : 1;                      // 0x5fc
            ULONG ProcessFirstResume : 1;                       // 0x5fc
            ULONG ForegroundExternal : 1;                       // 0x5fc
            ULONG ForegroundSystem : 1;                         // 0x5fc
            ULONG HighMemoryPriority : 1;                       // 0x5fc
            ULONG EnableProcessSuspendResumeLogging : 1;        // 0x5fc
            ULONG EnableThreadSuspendResumeLogging : 1;         // 0x5fc
            ULONG SecurityDomainChanged : 1;                    // 0x5fc
            ULONG SecurityFreezeComplete : 1;                   // 0x5fc
            ULONG VmProcessorHost : 1;                          // 0x5fc
            ULONG VmProcessorHostTransition : 1;                // 0x5fc
            ULONG AltSyscall : 1;                               // 0x5fc
            ULONG TimerResolutionIgnore : 1;                    // 0x5fc
            ULONG DisallowUserTerminate : 1;                    // 0x5fc
            ULONG EnableProcessRemoteExecProtectVmLogging : 1;  // 0x5fc
            ULONG EnableProcessLocalExecProtectVmLogging : 1;   // 0x5fc
            ULONG MemoryCompressionProcess : 1;                 // 0x5fc
            ULONG EnableProcessImpersonationLogging : 1;        // 0x5fc
        };
    };
    LONG DeviceAsid;                                   // 0x600
    PVOID SvmData;                                     // 0x608
    _EX_PUSH_LOCK SvmProcessLock;                      // 0x610
    ULONGLONG SvmLock;                                 // 0x618
    LIST_ENTRY SvmProcessDeviceListHead;               // 0x620
    ULONGLONG LastFreezeInterruptTime;                 // 0x630
    PPROCESS_DISK_COUNTERS DiskCounters;               // 0x638
    PVOID PicoContext;                                 // 0x640
    PVOID EnclaveTable;                                // 0x648
    ULONGLONG EnclaveNumber;                           // 0x650
    _EX_PUSH_LOCK EnclaveLock;                         // 0x658
    ULONG HighPriorityFaultsAllowed;                   // 0x660
    struct _PO_PROCESS_ENERGY_CONTEXT* EnergyContext;  // 0x668
    PVOID VmContext;                                   // 0x670
    ULONGLONG SequenceNumber;                          // 0x678
    ULONGLONG CreateInterruptTime;                     // 0x680
    ULONGLONG CreateUnbiasedInterruptTime;             // 0x688
    ULONGLONG TotalUnbiasedFrozenTime;                 // 0x690
    ULONGLONG LastAppStateUpdateTime;                  // 0x698
    ULONGLONG LastAppStateUptime : 61;                 // 0x6a0
    ULONGLONG LastAppState : 3;                        // 0x6a0
    volatile ULONGLONG SharedCommitCharge;             // 0x6a8
    _EX_PUSH_LOCK SharedCommitLock;                    // 0x6b0
    LIST_ENTRY SharedCommitLinks;                      // 0x6b8
    union {
        struct {
            ULONGLONG AllowedCpuSets;  // 0x6c8
            ULONGLONG DefaultCpuSets;  // 0x6d0
        };
        struct {
            PULONGLONG AllowedCpuSetsIndirect;  // 0x6c8
            PULONGLONG DefaultCpuSetsIndirect;  // 0x6d0
        };
    };
    PVOID DiskIoAttribution;                                       // 0x6d8
    PVOID DxgProcess;                                              // 0x6e0
    ULONG Win32KFilterSet;                                         // 0x6e8
    USHORT Machine;                                                // 0x6ec
    UCHAR MmSlabIdentity;                                          // 0x6ee
    UCHAR Spare0;                                                  // 0x6ef
    volatile PS_INTERLOCKED_TIMER_DELAY_VALUES ProcessTimerDelay;  // 0x6f0
    volatile ULONG KTimerSets;                                     // 0x6f8
    volatile ULONG KTimer2Sets;                                    // 0x6fc
    volatile ULONG ThreadTimerSets;                                // 0x700
    ULONGLONG VirtualTimerListLock;                                // 0x708
    LIST_ENTRY VirtualTimerListHead;                               // 0x710
    union {
        WNF_STATE_NAME WakeChannel;            // 0x720
        PS_PROCESS_WAKE_INFORMATION WakeInfo;  // 0x720
    };
    union {
        ULONG MitigationFlags;  // 0x750
        struct {
            ULONG ControlFlowGuardEnabled : 1;                   // 0x750
            ULONG ControlFlowGuardExportSuppressionEnabled : 1;  // 0x750
            ULONG ControlFlowGuardStrict : 1;                    // 0x750
            ULONG DisallowStrippedImages : 1;                    // 0x750
            ULONG ForceRelocateImages : 1;                       // 0x750
            ULONG HighEntropyASLREnabled : 1;                    // 0x750
            ULONG StackRandomizationDisabled : 1;                // 0x750
            ULONG ExtensionPointDisable : 1;                     // 0x750
            ULONG DisableDynamicCode : 1;                        // 0x750
            ULONG DisableDynamicCodeAllowOptOut : 1;             // 0x750
            ULONG DisableDynamicCodeAllowRemoteDowngrade : 1;    // 0x750
            ULONG AuditDisableDynamicCode : 1;                   // 0x750
            ULONG DisallowWin32kSystemCalls : 1;                 // 0x750
            ULONG AuditDisallowWin32kSystemCalls : 1;            // 0x750
            ULONG EnableFilteredWin32kAPIs : 1;                  // 0x750
            ULONG AuditFilteredWin32kAPIs : 1;                   // 0x750
            ULONG DisableNonSystemFonts : 1;                     // 0x750
            ULONG AuditNonSystemFontLoading : 1;                 // 0x750
            ULONG PreferSystem32Images : 1;                      // 0x750
            ULONG ProhibitRemoteImageMap : 1;                    // 0x750
            ULONG AuditProhibitRemoteImageMap : 1;               // 0x750
            ULONG ProhibitLowILImageMap : 1;                     // 0x750
            ULONG AuditProhibitLowILImageMap : 1;                // 0x750
            ULONG SignatureMitigationOptIn : 1;                  // 0x750
            ULONG AuditBlockNonMicrosoftBinaries : 1;            // 0x750
            ULONG AuditBlockNonMicrosoftBinariesAllowStore : 1;  // 0x750
            ULONG LoaderIntegrityContinuityEnabled : 1;          // 0x750
            ULONG AuditLoaderIntegrityContinuity : 1;            // 0x750
            ULONG EnableModuleTamperingProtection : 1;           // 0x750
            ULONG EnableModuleTamperingProtectionNoInherit : 1;  // 0x750
            ULONG RestrictIndirectBranchPrediction : 1;          // 0x750
            ULONG IsolateSecurityDomain : 1;                     // 0x750
        } MitigationFlagsValues;                                 // 0x750
    };
    union {
        ULONG MitigationFlags2;  // 0x754
        struct {
            ULONG EnableExportAddressFilter : 1;                 // 0x754
            ULONG AuditExportAddressFilter : 1;                  // 0x754
            ULONG EnableExportAddressFilterPlus : 1;             // 0x754
            ULONG AuditExportAddressFilterPlus : 1;              // 0x754
            ULONG EnableRopStackPivot : 1;                       // 0x754
            ULONG AuditRopStackPivot : 1;                        // 0x754
            ULONG EnableRopCallerCheck : 1;                      // 0x754
            ULONG AuditRopCallerCheck : 1;                       // 0x754
            ULONG EnableRopSimExec : 1;                          // 0x754
            ULONG AuditRopSimExec : 1;                           // 0x754
            ULONG EnableImportAddressFilter : 1;                 // 0x754
            ULONG AuditImportAddressFilter : 1;                  // 0x754
            ULONG DisablePageCombine : 1;                        // 0x754
            ULONG SpeculativeStoreBypassDisable : 1;             // 0x754
            ULONG CetUserShadowStacks : 1;                       // 0x754
            ULONG AuditCetUserShadowStacks : 1;                  // 0x754
            ULONG AuditCetUserShadowStacksLogged : 1;            // 0x754
            ULONG UserCetSetContextIpValidation : 1;             // 0x754
            ULONG AuditUserCetSetContextIpValidation : 1;        // 0x754
            ULONG AuditUserCetSetContextIpValidationLogged : 1;  // 0x754
            ULONG CetUserShadowStacksStrictMode : 1;             // 0x754
            ULONG BlockNonCetBinaries : 1;                       // 0x754
            ULONG BlockNonCetBinariesNonEhcont : 1;              // 0x754
            ULONG AuditBlockNonCetBinaries : 1;                  // 0x754
            ULONG AuditBlockNonCetBinariesLogged : 1;            // 0x754
            ULONG XtendedControlFlowGuard_Deprecated : 1;        // 0x754
            ULONG AuditXtendedControlFlowGuard_Deprecated : 1;   // 0x754
            ULONG PointerAuthUserIp : 1;                         // 0x754
            ULONG AuditPointerAuthUserIp : 1;                    // 0x754
            ULONG AuditPointerAuthUserIpLogged : 1;              // 0x754
            ULONG CetDynamicApisOutOfProcOnly : 1;               // 0x754
            ULONG UserCetSetContextIpValidationRelaxedMode : 1;  // 0x754
        } MitigationFlags2Values;                                // 0x754
    };
    PVOID PartitionObject;                                                  // 0x758
    ULONGLONG SecurityDomain;                                               // 0x760
    ULONGLONG ParentSecurityDomain;                                         // 0x768
    PVOID CoverageSamplerContext;                                           // 0x770
    PVOID MmHotPatchContext;                                                // 0x778
    RTL_AVL_TREE DynamicEHContinuationTargetsTree;                          // 0x780
    _EX_PUSH_LOCK DynamicEHContinuationTargetsLock;                         // 0x788
    PS_DYNAMIC_ENFORCED_ADDRESS_RANGES DynamicEnforcedCetCompatibleRanges;  // 0x790
    ULONG DisabledComponentFlags;                                           // 0x7a0
    volatile LONG PageCombineSequence;                                      // 0x7a4
    _EX_PUSH_LOCK EnableOptionalXStateFeaturesLock;                         // 0x7a8
    PULONG volatile PathRedirectionHashes;                                  // 0x7b0
    struct _PS_SYSCALL_PROVIDER* SyscallProvider;                           // 0x7b8
    LIST_ENTRY SyscallProviderProcessLinks;                                 // 0x7c0
    PSP_SYSCALL_PROVIDER_DISPATCH_CONTEXT SyscallProviderDispatchContext;   // 0x7d0
    union {
        ULONG MitigationFlags3;  // 0x7d8
        struct {
            ULONG RestrictCoreSharing : 1;            // 0x7d8
            ULONG DisallowFsctlSystemCalls : 1;       // 0x7d8
            ULONG AuditDisallowFsctlSystemCalls : 1;  // 0x7d8
            ULONG MitigationFlags3Spare : 29;         // 0x7d8
        } MitigationFlags3Values;                     // 0x7d8
    };
    union {
        ULONG Flags4;  // 0x7dc
        struct {
            ULONG ThreadWasActive : 1;        // 0x7dc
            ULONG MinimalTerminate : 1;       // 0x7dc
            ULONG ImageExpansionDisable : 1;  // 0x7dc
            ULONG SessionFirstProcess : 1;    // 0x7dc
        };
    };
    union {
        ULONG SyscallUsage;  // 0x7e0
        struct {
            ULONG SystemModuleInformation : 1;            // 0x7e0
            ULONG SystemModuleInformationEx : 1;          // 0x7e0
            ULONG SystemLocksInformation : 1;             // 0x7e0
            ULONG SystemStackTraceInformation : 1;        // 0x7e0
            ULONG SystemHandleInformation : 1;            // 0x7e0
            ULONG SystemExtendedHandleInformation : 1;    // 0x7e0
            ULONG SystemObjectInformation : 1;            // 0x7e0
            ULONG SystemBigPoolInformation : 1;           // 0x7e0
            ULONG SystemExtendedProcessInformation : 1;   // 0x7e0
            ULONG SystemSessionProcessInformation : 1;    // 0x7e0
            ULONG SystemMemoryTopologyInformation : 1;    // 0x7e0
            ULONG SystemMemoryChannelInformation : 1;     // 0x7e0
            ULONG SystemUnused : 1;                       // 0x7e0
            ULONG SystemPlatformBinaryInformation : 1;    // 0x7e0
            ULONG SystemFirmwareTableInformation : 1;     // 0x7e0
            ULONG SystemBootMetadataInformation : 1;      // 0x7e0
            ULONG SystemWheaIpmiHardwareInformation : 1;  // 0x7e0
            ULONG SystemSuperfetchPrefetch : 1;           // 0x7e0
            ULONG SystemSuperfetchPfnQuery : 1;           // 0x7e0
            ULONG SystemSuperfetchPrivSourceQuery : 1;    // 0x7e0
            ULONG SystemSuperfetchMemoryListQuery : 1;    // 0x7e0
            ULONG SystemSuperfetchMemoryRangesQuery : 1;  // 0x7e0
            ULONG SystemSuperfetchPfnSetPriority : 1;     // 0x7e0
            ULONG SystemSuperfetchMovePages : 1;          // 0x7e0
            ULONG SystemSuperfetchPfnSetPageHeat : 1;     // 0x7e0
            ULONG SysDbgGetTriageDump : 1;                // 0x7e0
            ULONG SysDbgGetLiveKernelDump : 1;            // 0x7e0
            ULONG SyscallUsageValuesSpare : 5;            // 0x7e0
        } SyscallUsageValues;                             // 0x7e0
    };
    LONG SupervisorDeviceAsid;                  // 0x7e4
    PVOID SupervisorSvmData;                    // 0x7e8
    PPROCESS_NETWORK_COUNTERS NetworkCounters;  // 0x7f0
    PROCESS_EXECUTION Execution;                // 0x7f8
    PVOID ThreadIndexTable;                     // 0x800
} EPROCESS, *_PEPROCESS;

#endif  // __STRUCTURES_H__
