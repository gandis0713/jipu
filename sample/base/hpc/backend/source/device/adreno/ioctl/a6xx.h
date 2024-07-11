#pragma once

#include <stddef.h>
#include <stdint.h>

/// Counter choices for Adreno A6XX series.
enum class adreno_a6xx_counter : uint32_t
{
    /// Command Parser: always count
    A6XX_CP_ALWAYS_COUNT = 0,
    /// Command Parser: busy gfx core idle
    A6XX_CP_BUSY_GFX_CORE_IDLE = 1,
    /// Command Parser: busy cycles
    A6XX_CP_BUSY_CYCLES = 2,
    /// Command Parser: num preemptions
    A6XX_CP_NUM_PREEMPTIONS = 3,
    /// Command Parser: preemption reaction delay
    A6XX_CP_PREEMPTION_REACTION_DELAY = 4,
    /// Command Parser: preemption switch out time
    A6XX_CP_PREEMPTION_SWITCH_OUT_TIME = 5,
    /// Command Parser: preemption switch IN time
    A6XX_CP_PREEMPTION_SWITCH_IN_TIME = 6,
    /// Command Parser: dead draws IN bin render
    A6XX_CP_DEAD_DRAWS_IN_BIN_RENDER = 7,
    /// Command Parser: predicated draws killed
    A6XX_CP_PREDICATED_DRAWS_KILLED = 8,
    /// Command Parser: mode switch
    A6XX_CP_MODE_SWITCH = 9,
    /// Command Parser: zpass done
    A6XX_CP_ZPASS_DONE = 10,
    /// Command Parser: context done
    A6XX_CP_CONTEXT_DONE = 11,
    /// Command Parser: cache flush
    A6XX_CP_CACHE_FLUSH = 12,
    /// Command Parser: long preemptions
    A6XX_CP_LONG_PREEMPTIONS = 13,
    /// Command Parser: sqe I cache starve
    A6XX_CP_SQE_I_CACHE_STARVE = 14,
    /// Command Parser: sqe idle
    A6XX_CP_SQE_IDLE = 15,
    /// Command Parser: sqe pm4 starve RB IB
    A6XX_CP_SQE_PM4_STARVE_RB_IB = 16,
    /// Command Parser: sqe pm4 starve sds
    A6XX_CP_SQE_PM4_STARVE_SDS = 17,
    /// Command Parser: sqe mrb starve
    A6XX_CP_SQE_MRB_STARVE = 18,
    /// Command Parser: sqe rrb starve
    A6XX_CP_SQE_RRB_STARVE = 19,
    /// Command Parser: sqe vsd starve
    A6XX_CP_SQE_VSD_STARVE = 20,
    /// Command Parser: vsd decode starve
    A6XX_CP_VSD_DECODE_STARVE = 21,
    /// Command Parser: sqe pipe out stall
    A6XX_CP_SQE_PIPE_OUT_STALL = 22,
    /// Command Parser: sqe sync stall
    A6XX_CP_SQE_SYNC_STALL = 23,
    /// Command Parser: sqe pm4 wfi stall
    A6XX_CP_SQE_PM4_WFI_STALL = 24,
    /// Command Parser: sqe sys wfi stall
    A6XX_CP_SQE_SYS_WFI_STALL = 25,
    /// Command Parser: sqe T4 exec
    A6XX_CP_SQE_T4_EXEC = 26,
    /// Command Parser: sqe load state exec
    A6XX_CP_SQE_LOAD_STATE_EXEC = 27,
    /// Command Parser: sqe save sds state
    A6XX_CP_SQE_SAVE_SDS_STATE = 28,
    /// Command Parser: sqe draw exec
    A6XX_CP_SQE_DRAW_EXEC = 29,
    /// Command Parser: sqe ctxt reg bunch exec
    A6XX_CP_SQE_CTXT_REG_BUNCH_EXEC = 30,
    /// Command Parser: sqe exec profiled
    A6XX_CP_SQE_EXEC_PROFILED = 31,
    /// Command Parser: memory pool empty
    A6XX_CP_MEMORY_POOL_EMPTY = 32,
    /// Command Parser: memory pool sync stall
    A6XX_CP_MEMORY_POOL_SYNC_STALL = 33,
    /// Command Parser: memory pool above thresh
    A6XX_CP_MEMORY_POOL_ABOVE_THRESH = 34,
    /// Command Parser: ahb WR stall pre draws
    A6XX_CP_AHB_WR_STALL_PRE_DRAWS = 35,
    /// Command Parser: ahb stall sqe gmu
    A6XX_CP_AHB_STALL_SQE_GMU = 36,
    /// Command Parser: ahb stall sqe WR other
    A6XX_CP_AHB_STALL_SQE_WR_OTHER = 37,
    /// Command Parser: ahb stall sqe RD other
    A6XX_CP_AHB_STALL_SQE_RD_OTHER = 38,
    /// Command Parser: cluster0 empty
    A6XX_CP_CLUSTER0_EMPTY = 39,
    /// Command Parser: cluster1 empty
    A6XX_CP_CLUSTER1_EMPTY = 40,
    /// Command Parser: cluster2 empty
    A6XX_CP_CLUSTER2_EMPTY = 41,
    /// Command Parser: cluster3 empty
    A6XX_CP_CLUSTER3_EMPTY = 42,
    /// Command Parser: cluster4 empty
    A6XX_CP_CLUSTER4_EMPTY = 43,
    /// Command Parser: cluster5 empty
    A6XX_CP_CLUSTER5_EMPTY = 44,
    /// Command Parser: pm4 data
    A6XX_CP_PM4_DATA = 45,
    /// Command Parser: pm4 headers
    A6XX_CP_PM4_HEADERS = 46,
    /// Command Parser: vbif read beats
    A6XX_CP_VBIF_READ_BEATS = 47,
    /// Command Parser: vbif write beats
    A6XX_CP_VBIF_WRITE_BEATS = 48,
    /// Command Parser: sqe instr counter
    A6XX_CP_SQE_INSTR_COUNTER = 49,
    /// RBBM: always count
    A6XX_RBBM_ALWAYS_COUNT = 256,
    /// RBBM: always ON
    A6XX_RBBM_ALWAYS_ON = 257,
    /// RBBM: TSE busy
    A6XX_RBBM_TSE_BUSY = 258,
    /// RBBM: RAS busy
    A6XX_RBBM_RAS_BUSY = 259,
    /// RBBM: PC dcall busy
    A6XX_RBBM_PC_DCALL_BUSY = 260,
    /// RBBM: PC vsd busy
    A6XX_RBBM_PC_VSD_BUSY = 261,
    /// RBBM: status masked
    A6XX_RBBM_STATUS_MASKED = 262,
    /// RBBM: com busy
    A6XX_RBBM_COM_BUSY = 263,
    /// RBBM: dcom busy
    A6XX_RBBM_DCOM_BUSY = 264,
    /// RBBM: vbif busy
    A6XX_RBBM_VBIF_BUSY = 265,
    /// RBBM: VSC busy
    A6XX_RBBM_VSC_BUSY = 266,
    /// RBBM: tess busy
    A6XX_RBBM_TESS_BUSY = 267,
    /// RBBM: UCHE busy
    A6XX_RBBM_UCHE_BUSY = 268,
    /// RBBM: HLSQ busy
    A6XX_RBBM_HLSQ_BUSY = 269,
    /// PC: busy cycles
    A6XX_PC_BUSY_CYCLES = 512,
    /// PC: working cycles
    A6XX_PC_WORKING_CYCLES = 513,
    /// PC: stall cycles VFD
    A6XX_PC_STALL_CYCLES_VFD = 514,
    /// PC: stall cycles TSE
    A6XX_PC_STALL_CYCLES_TSE = 515,
    /// PC: stall cycles VPC
    A6XX_PC_STALL_CYCLES_VPC = 516,
    /// PC: stall cycles UCHE
    A6XX_PC_STALL_CYCLES_UCHE = 517,
    /// PC: stall cycles tess
    A6XX_PC_STALL_CYCLES_TESS = 518,
    /// PC: stall cycles TSE only
    A6XX_PC_STALL_CYCLES_TSE_ONLY = 519,
    /// PC: stall cycles VPC only
    A6XX_PC_STALL_CYCLES_VPC_ONLY = 520,
    /// PC: pass1 TF stall cycles
    A6XX_PC_PASS1_TF_STALL_CYCLES = 521,
    /// PC: starve cycles for index
    A6XX_PC_STARVE_CYCLES_FOR_INDEX = 522,
    /// PC: starve cycles for tess factor
    A6XX_PC_STARVE_CYCLES_FOR_TESS_FACTOR = 523,
    /// PC: starve cycles for viz stream
    A6XX_PC_STARVE_CYCLES_FOR_VIZ_STREAM = 524,
    /// PC: starve cycles for position
    A6XX_PC_STARVE_CYCLES_FOR_POSITION = 525,
    /// PC: starve cycles DI
    A6XX_PC_STARVE_CYCLES_DI = 526,
    /// PC: vis streams loaded
    A6XX_PC_VIS_STREAMS_LOADED = 527,
    /// PC: instances
    A6XX_PC_INSTANCES = 528,
    /// PC: VPC primitives
    A6XX_PC_VPC_PRIMITIVES = 529,
    /// PC: dead prim
    A6XX_PC_DEAD_PRIM = 530,
    /// PC: live prim
    A6XX_PC_LIVE_PRIM = 531,
    /// PC: vertex hits
    A6XX_PC_VERTEX_HITS = 532,
    /// PC: IA vertices
    A6XX_PC_IA_VERTICES = 533,
    /// PC: IA primitives
    A6XX_PC_IA_PRIMITIVES = 534,
    /// PC: GS primitives
    A6XX_PC_GS_PRIMITIVES = 535,
    /// PC: HS invocations
    A6XX_PC_HS_INVOCATIONS = 536,
    /// PC: DS invocations
    A6XX_PC_DS_INVOCATIONS = 537,
    /// PC: VS invocations
    A6XX_PC_VS_INVOCATIONS = 538,
    /// PC: GS invocations
    A6XX_PC_GS_INVOCATIONS = 539,
    /// PC: DS primitives
    A6XX_PC_DS_PRIMITIVES = 540,
    /// PC: VPC pos data transaction
    A6XX_PC_VPC_POS_DATA_TRANSACTION = 541,
    /// PC: 3D drawcalls
    A6XX_PC_3D_DRAWCALLS = 542,
    /// PC: 2D drawcalls
    A6XX_PC_2D_DRAWCALLS = 543,
    /// PC: non drawcall global events
    A6XX_PC_NON_DRAWCALL_GLOBAL_EVENTS = 544,
    /// PC: tess busy cycles
    A6XX_PC_TESS_BUSY_CYCLES = 545,
    /// PC: tess working cycles
    A6XX_PC_TESS_WORKING_CYCLES = 546,
    /// PC: tess stall cycles PC
    A6XX_PC_TESS_STALL_CYCLES_PC = 547,
    /// PC: tess starve cycles PC
    A6XX_PC_TESS_STARVE_CYCLES_PC = 548,
    /// PC: TSE transaction
    A6XX_PC_TSE_TRANSACTION = 549,
    /// PC: TSE vertex
    A6XX_PC_TSE_VERTEX = 550,
    /// PC: tess PC UV trans
    A6XX_PC_TESS_PC_UV_TRANS = 551,
    /// PC: tess PC UV patches
    A6XX_PC_TESS_PC_UV_PATCHES = 552,
    /// PC: tess factor trans
    A6XX_PC_TESS_FACTOR_TRANS = 553,
    /// VFD: busy cycles
    A6XX_VFD_BUSY_CYCLES = 768,
    /// VFD: stall cycles UCHE
    A6XX_VFD_STALL_CYCLES_UCHE = 769,
    /// VFD: stall cycles VPC alloc
    A6XX_VFD_STALL_CYCLES_VPC_ALLOC = 770,
    /// VFD: stall cycles SP info
    A6XX_VFD_STALL_CYCLES_SP_INFO = 771,
    /// VFD: stall cycles SP attr
    A6XX_VFD_STALL_CYCLES_SP_ATTR = 772,
    /// VFD: starve cycles UCHE
    A6XX_VFD_STARVE_CYCLES_UCHE = 773,
    /// VFD: rbuffer full
    A6XX_VFD_RBUFFER_FULL = 774,
    /// VFD: attr info fifo full
    A6XX_VFD_ATTR_INFO_FIFO_FULL = 775,
    /// VFD: decoded attribute bytes
    A6XX_VFD_DECODED_ATTRIBUTE_BYTES = 776,
    /// VFD: num attributes
    A6XX_VFD_NUM_ATTRIBUTES = 777,
    /// VFD: upper shader fibers
    A6XX_VFD_UPPER_SHADER_FIBERS = 778,
    /// VFD: lower shader fibers
    A6XX_VFD_LOWER_SHADER_FIBERS = 779,
    /// VFD: mode 0 fibers
    A6XX_VFD_MODE_0_FIBERS = 780,
    /// VFD: mode 1 fibers
    A6XX_VFD_MODE_1_FIBERS = 781,
    /// VFD: mode 2 fibers
    A6XX_VFD_MODE_2_FIBERS = 782,
    /// VFD: mode 3 fibers
    A6XX_VFD_MODE_3_FIBERS = 783,
    /// VFD: mode 4 fibers
    A6XX_VFD_MODE_4_FIBERS = 784,
    /// VFD: total vertices
    A6XX_VFD_TOTAL_VERTICES = 785,
    /// VFD: vfdp stall cycles VFD
    A6XX_VFD_VFDP_STALL_CYCLES_VFD = 786,
    /// VFD: vfdp stall cycles VFD index
    A6XX_VFD_VFDP_STALL_CYCLES_VFD_INDEX = 787,
    /// VFD: vfdp stall cycles VFD prog
    A6XX_VFD_VFDP_STALL_CYCLES_VFD_PROG = 788,
    /// VFD: vfdp starve cycles PC
    A6XX_VFD_VFDP_STARVE_CYCLES_PC = 789,
    /// VFD: vfdp VS stage waves
    A6XX_VFD_VFDP_VS_STAGE_WAVES = 790,
    /// High Level SeQuencer: busy cycles
    A6XX_HLSQ_BUSY_CYCLES = 1024,
    /// High Level SeQuencer: stall cycles UCHE
    A6XX_HLSQ_STALL_CYCLES_UCHE = 1025,
    /// High Level SeQuencer: stall cycles SP state
    A6XX_HLSQ_STALL_CYCLES_SP_STATE = 1026,
    /// High Level SeQuencer: stall cycles SP FS stage
    A6XX_HLSQ_STALL_CYCLES_SP_FS_STAGE = 1027,
    /// High Level SeQuencer: UCHE latency cycles
    A6XX_HLSQ_UCHE_LATENCY_CYCLES = 1028,
    /// High Level SeQuencer: UCHE latency count
    A6XX_HLSQ_UCHE_LATENCY_COUNT = 1029,
    /// High Level SeQuencer: FS stage 1X waves
    A6XX_HLSQ_FS_STAGE_1X_WAVES = 1030,
    /// High Level SeQuencer: FS stage 2X waves
    A6XX_HLSQ_FS_STAGE_2X_WAVES = 1031,
    /// High Level SeQuencer: quads
    A6XX_HLSQ_QUADS = 1032,
    /// High Level SeQuencer: CS invocations
    A6XX_HLSQ_CS_INVOCATIONS = 1033,
    /// High Level SeQuencer: compute drawcalls
    A6XX_HLSQ_COMPUTE_DRAWCALLS = 1034,
    /// High Level SeQuencer: FS data wait programming
    A6XX_HLSQ_FS_DATA_WAIT_PROGRAMMING = 1035,
    /// High Level SeQuencer: dual FS prog active
    A6XX_HLSQ_DUAL_FS_PROG_ACTIVE = 1036,
    /// High Level SeQuencer: dual VS prog active
    A6XX_HLSQ_DUAL_VS_PROG_ACTIVE = 1037,
    /// High Level SeQuencer: FS batch count zero
    A6XX_HLSQ_FS_BATCH_COUNT_ZERO = 1038,
    /// High Level SeQuencer: VS batch count zero
    A6XX_HLSQ_VS_BATCH_COUNT_ZERO = 1039,
    /// High Level SeQuencer: wave pending NO quad
    A6XX_HLSQ_WAVE_PENDING_NO_QUAD = 1040,
    /// High Level SeQuencer: wave pending NO prim base
    A6XX_HLSQ_WAVE_PENDING_NO_PRIM_BASE = 1041,
    /// High Level SeQuencer: stall cycles VPC
    A6XX_HLSQ_STALL_CYCLES_VPC = 1042,
    /// High Level SeQuencer: pixels
    A6XX_HLSQ_PIXELS = 1043,
    /// High Level SeQuencer: draw mode switch vsfs sync
    A6XX_HLSQ_DRAW_MODE_SWITCH_VSFS_SYNC = 1044,
    /// VPC: busy cycles
    A6XX_VPC_BUSY_CYCLES = 1280,
    /// VPC: working cycles
    A6XX_VPC_WORKING_CYCLES = 1281,
    /// VPC: stall cycles UCHE
    A6XX_VPC_STALL_CYCLES_UCHE = 1282,
    /// VPC: stall cycles VFD wack
    A6XX_VPC_STALL_CYCLES_VFD_WACK = 1283,
    /// VPC: stall cycles HLSQ prim alloc
    A6XX_VPC_STALL_CYCLES_HLSQ_PRIM_ALLOC = 1284,
    /// VPC: stall cycles PC
    A6XX_VPC_STALL_CYCLES_PC = 1285,
    /// VPC: stall cycles SP LM
    A6XX_VPC_STALL_CYCLES_SP_LM = 1286,
    /// VPC: starve cycles SP
    A6XX_VPC_STARVE_CYCLES_SP = 1287,
    /// VPC: starve cycles LRZ
    A6XX_VPC_STARVE_CYCLES_LRZ = 1288,
    /// VPC: PC primitives
    A6XX_VPC_PC_PRIMITIVES = 1289,
    /// VPC: SP components
    A6XX_VPC_SP_COMPONENTS = 1290,
    /// VPC: stall cycles vpcram pos
    A6XX_VPC_STALL_CYCLES_VPCRAM_POS = 1291,
    /// VPC: LRZ assign primitives
    A6XX_VPC_LRZ_ASSIGN_PRIMITIVES = 1292,
    /// VPC: RB visible primitives
    A6XX_VPC_RB_VISIBLE_PRIMITIVES = 1293,
    /// VPC: LM transaction
    A6XX_VPC_LM_TRANSACTION = 1294,
    /// VPC: streamout transaction
    A6XX_VPC_STREAMOUT_TRANSACTION = 1295,
    /// VPC: VS busy cycles
    A6XX_VPC_VS_BUSY_CYCLES = 1296,
    /// VPC: PS busy cycles
    A6XX_VPC_PS_BUSY_CYCLES = 1297,
    /// VPC: VS working cycles
    A6XX_VPC_VS_WORKING_CYCLES = 1298,
    /// VPC: PS working cycles
    A6XX_VPC_PS_WORKING_CYCLES = 1299,
    /// VPC: starve cycles RB
    A6XX_VPC_STARVE_CYCLES_RB = 1300,
    /// VPC: num vpcram read pos
    A6XX_VPC_NUM_VPCRAM_READ_POS = 1301,
    /// VPC: wit full cycles
    A6XX_VPC_WIT_FULL_CYCLES = 1302,
    /// VPC: vpcram full cycles
    A6XX_VPC_VPCRAM_FULL_CYCLES = 1303,
    /// VPC: LM full wait for intp end
    A6XX_VPC_LM_FULL_WAIT_FOR_INTP_END = 1304,
    /// VPC: num vpcram write
    A6XX_VPC_NUM_VPCRAM_WRITE = 1305,
    /// VPC: num vpcram read SO
    A6XX_VPC_NUM_VPCRAM_READ_SO = 1306,
    /// VPC: num attr req LM
    A6XX_VPC_NUM_ATTR_REQ_LM = 1307,
    /// Triangle Setup Engine: busy cycles
    A6XX_TSE_BUSY_CYCLES = 1536,
    /// Triangle Setup Engine: clipping cycles
    A6XX_TSE_CLIPPING_CYCLES = 1537,
    /// Triangle Setup Engine: stall cycles RAS
    A6XX_TSE_STALL_CYCLES_RAS = 1538,
    /// Triangle Setup Engine: stall cycles LRZ baryplane
    A6XX_TSE_STALL_CYCLES_LRZ_BARYPLANE = 1539,
    /// Triangle Setup Engine: stall cycles LRZ zplane
    A6XX_TSE_STALL_CYCLES_LRZ_ZPLANE = 1540,
    /// Triangle Setup Engine: starve cycles PC
    A6XX_TSE_STARVE_CYCLES_PC = 1541,
    /// Triangle Setup Engine: input prim
    A6XX_TSE_INPUT_PRIM = 1542,
    /// Triangle Setup Engine: input null prim
    A6XX_TSE_INPUT_NULL_PRIM = 1543,
    /// Triangle Setup Engine: trival rej prim
    A6XX_TSE_TRIVAL_REJ_PRIM = 1544,
    /// Triangle Setup Engine: clipped prim
    A6XX_TSE_CLIPPED_PRIM = 1545,
    /// Triangle Setup Engine: zero area prim
    A6XX_TSE_ZERO_AREA_PRIM = 1546,
    /// Triangle Setup Engine: faceness culled prim
    A6XX_TSE_FACENESS_CULLED_PRIM = 1547,
    /// Triangle Setup Engine: zero pixel prim
    A6XX_TSE_ZERO_PIXEL_PRIM = 1548,
    /// Triangle Setup Engine: output null prim
    A6XX_TSE_OUTPUT_NULL_PRIM = 1549,
    /// Triangle Setup Engine: output visible prim
    A6XX_TSE_OUTPUT_VISIBLE_PRIM = 1550,
    /// Triangle Setup Engine: cinvocation
    A6XX_TSE_CINVOCATION = 1551,
    /// Triangle Setup Engine: cprimitives
    A6XX_TSE_CPRIMITIVES = 1552,
    /// Triangle Setup Engine: 2D input prim
    A6XX_TSE_2D_INPUT_PRIM = 1553,
    /// Triangle Setup Engine: 2D alive cycles
    A6XX_TSE_2D_ALIVE_CYCLES = 1554,
    /// Triangle Setup Engine: clip planes
    A6XX_TSE_CLIP_PLANES = 1555,
    /// RAS: busy cycles
    A6XX_RAS_BUSY_CYCLES = 1792,
    /// RAS: supertile active cycles
    A6XX_RAS_SUPERTILE_ACTIVE_CYCLES = 1793,
    /// RAS: stall cycles LRZ
    A6XX_RAS_STALL_CYCLES_LRZ = 1794,
    /// RAS: starve cycles TSE
    A6XX_RAS_STARVE_CYCLES_TSE = 1795,
    /// RAS: super tiles
    A6XX_RAS_SUPER_TILES = 1796,
    /// RAS: 8x4 tiles
    A6XX_RAS_8X4_TILES = 1797,
    /// RAS: maskgen active
    A6XX_RAS_MASKGEN_ACTIVE = 1798,
    /// RAS: fully covered super tiles
    A6XX_RAS_FULLY_COVERED_SUPER_TILES = 1799,
    /// RAS: fully covered 8x4 tiles
    A6XX_RAS_FULLY_COVERED_8X4_TILES = 1800,
    /// RAS: prim killed invisilbe
    A6XX_RAS_PRIM_KILLED_INVISILBE = 1801,
    /// RAS: supertile gen active cycles
    A6XX_RAS_SUPERTILE_GEN_ACTIVE_CYCLES = 1802,
    /// RAS: LRZ intf working cycles
    A6XX_RAS_LRZ_INTF_WORKING_CYCLES = 1803,
    /// RAS: blocks
    A6XX_RAS_BLOCKS = 1804,
    /// Unified L2 Cache: busy cycles
    A6XX_UCHE_BUSY_CYCLES = 2048,
    /// Unified L2 Cache: stall cycles arbiter
    A6XX_UCHE_STALL_CYCLES_ARBITER = 2049,
    /// Unified L2 Cache: vbif latency cycles
    A6XX_UCHE_VBIF_LATENCY_CYCLES = 2050,
    /// Unified L2 Cache: vbif latency samples
    A6XX_UCHE_VBIF_LATENCY_SAMPLES = 2051,
    /// Unified L2 Cache: vbif read beats TP
    A6XX_UCHE_VBIF_READ_BEATS_TP = 2052,
    /// Unified L2 Cache: vbif read beats VFD
    A6XX_UCHE_VBIF_READ_BEATS_VFD = 2053,
    /// Unified L2 Cache: vbif read beats HLSQ
    A6XX_UCHE_VBIF_READ_BEATS_HLSQ = 2054,
    /// Unified L2 Cache: vbif read beats LRZ
    A6XX_UCHE_VBIF_READ_BEATS_LRZ = 2055,
    /// Unified L2 Cache: vbif read beats SP
    A6XX_UCHE_VBIF_READ_BEATS_SP = 2056,
    /// Unified L2 Cache: read requests TP
    A6XX_UCHE_READ_REQUESTS_TP = 2057,
    /// Unified L2 Cache: read requests VFD
    A6XX_UCHE_READ_REQUESTS_VFD = 2058,
    /// Unified L2 Cache: read requests HLSQ
    A6XX_UCHE_READ_REQUESTS_HLSQ = 2059,
    /// Unified L2 Cache: read requests LRZ
    A6XX_UCHE_READ_REQUESTS_LRZ = 2060,
    /// Unified L2 Cache: read requests SP
    A6XX_UCHE_READ_REQUESTS_SP = 2061,
    /// Unified L2 Cache: write requests LRZ
    A6XX_UCHE_WRITE_REQUESTS_LRZ = 2062,
    /// Unified L2 Cache: write requests SP
    A6XX_UCHE_WRITE_REQUESTS_SP = 2063,
    /// Unified L2 Cache: write requests VPC
    A6XX_UCHE_WRITE_REQUESTS_VPC = 2064,
    /// Unified L2 Cache: write requests VSC
    A6XX_UCHE_WRITE_REQUESTS_VSC = 2065,
    /// Unified L2 Cache: evicts
    A6XX_UCHE_EVICTS = 2066,
    /// Unified L2 Cache: bank req0
    A6XX_UCHE_BANK_REQ0 = 2067,
    /// Unified L2 Cache: bank req1
    A6XX_UCHE_BANK_REQ1 = 2068,
    /// Unified L2 Cache: bank req2
    A6XX_UCHE_BANK_REQ2 = 2069,
    /// Unified L2 Cache: bank req3
    A6XX_UCHE_BANK_REQ3 = 2070,
    /// Unified L2 Cache: bank req4
    A6XX_UCHE_BANK_REQ4 = 2071,
    /// Unified L2 Cache: bank req5
    A6XX_UCHE_BANK_REQ5 = 2072,
    /// Unified L2 Cache: bank req6
    A6XX_UCHE_BANK_REQ6 = 2073,
    /// Unified L2 Cache: bank req7
    A6XX_UCHE_BANK_REQ7 = 2074,
    /// Unified L2 Cache: vbif read beats ch0
    A6XX_UCHE_VBIF_READ_BEATS_CH0 = 2075,
    /// Unified L2 Cache: vbif read beats ch1
    A6XX_UCHE_VBIF_READ_BEATS_CH1 = 2076,
    /// Unified L2 Cache: gmem read beats
    A6XX_UCHE_GMEM_READ_BEATS = 2077,
    /// Unified L2 Cache: tph ref full
    A6XX_UCHE_TPH_REF_FULL = 2078,
    /// Unified L2 Cache: tph victim full
    A6XX_UCHE_TPH_VICTIM_FULL = 2079,
    /// Unified L2 Cache: tph ext full
    A6XX_UCHE_TPH_EXT_FULL = 2080,
    /// Unified L2 Cache: vbif stall write data
    A6XX_UCHE_VBIF_STALL_WRITE_DATA = 2081,
    /// Unified L2 Cache: dcmp latency samples
    A6XX_UCHE_DCMP_LATENCY_SAMPLES = 2082,
    /// Unified L2 Cache: dcmp latency cycles
    A6XX_UCHE_DCMP_LATENCY_CYCLES = 2083,
    /// Unified L2 Cache: vbif read beats PC
    A6XX_UCHE_VBIF_READ_BEATS_PC = 2084,
    /// Unified L2 Cache: read requests PC
    A6XX_UCHE_READ_REQUESTS_PC = 2085,
    /// Unified L2 Cache: ram read req
    A6XX_UCHE_RAM_READ_REQ = 2086,
    /// Unified L2 Cache: ram write req
    A6XX_UCHE_RAM_WRITE_REQ = 2087,
    /// Texture Processor: busy cycles
    A6XX_TP_BUSY_CYCLES = 2304,
    /// Texture Processor: stall cycles UCHE
    A6XX_TP_STALL_CYCLES_UCHE = 2305,
    /// Texture Processor: latency cycles
    A6XX_TP_LATENCY_CYCLES = 2306,
    /// Texture Processor: latency trans
    A6XX_TP_LATENCY_TRANS = 2307,
    /// Texture Processor: flag cache request samples
    A6XX_TP_FLAG_CACHE_REQUEST_SAMPLES = 2308,
    /// Texture Processor: flag cache request latency
    A6XX_TP_FLAG_CACHE_REQUEST_LATENCY = 2309,
    /// Texture Processor: L1 cacheline requests
    A6XX_TP_L1_CACHELINE_REQUESTS = 2310,
    /// Texture Processor: L1 cacheline misses
    A6XX_TP_L1_CACHELINE_MISSES = 2311,
    /// Texture Processor: SP TP trans
    A6XX_TP_SP_TP_TRANS = 2312,
    /// Texture Processor: TP SP trans
    A6XX_TP_TP_SP_TRANS = 2313,
    /// Texture Processor: output pixels
    A6XX_TP_OUTPUT_PIXELS = 2314,
    /// Texture Processor: filter workload 16bit
    A6XX_TP_FILTER_WORKLOAD_16BIT = 2315,
    /// Texture Processor: filter workload 32bit
    A6XX_TP_FILTER_WORKLOAD_32BIT = 2316,
    /// Texture Processor: quads received
    A6XX_TP_QUADS_RECEIVED = 2317,
    /// Texture Processor: quads offset
    A6XX_TP_QUADS_OFFSET = 2318,
    /// Texture Processor: quads shadow
    A6XX_TP_QUADS_SHADOW = 2319,
    /// Texture Processor: quads array
    A6XX_TP_QUADS_ARRAY = 2320,
    /// Texture Processor: quads gradient
    A6XX_TP_QUADS_GRADIENT = 2321,
    /// Texture Processor: quads 1D
    A6XX_TP_QUADS_1D = 2322,
    /// Texture Processor: quads 2D
    A6XX_TP_QUADS_2D = 2323,
    /// Texture Processor: quads buffer
    A6XX_TP_QUADS_BUFFER = 2324,
    /// Texture Processor: quads 3D
    A6XX_TP_QUADS_3D = 2325,
    /// Texture Processor: quads cube
    A6XX_TP_QUADS_CUBE = 2326,
    /// Texture Processor: divergent quads received
    A6XX_TP_DIVERGENT_QUADS_RECEIVED = 2327,
    /// Texture Processor: prt non resident events
    A6XX_TP_PRT_NON_RESIDENT_EVENTS = 2328,
    /// Texture Processor: output pixels point
    A6XX_TP_OUTPUT_PIXELS_POINT = 2329,
    /// Texture Processor: output pixels bilinear
    A6XX_TP_OUTPUT_PIXELS_BILINEAR = 2330,
    /// Texture Processor: output pixels mip
    A6XX_TP_OUTPUT_PIXELS_MIP = 2331,
    /// Texture Processor: output pixels aniso
    A6XX_TP_OUTPUT_PIXELS_ANISO = 2332,
    /// Texture Processor: output pixels zero lod
    A6XX_TP_OUTPUT_PIXELS_ZERO_LOD = 2333,
    /// Texture Processor: flag cache requests
    A6XX_TP_FLAG_CACHE_REQUESTS = 2334,
    /// Texture Processor: flag cache misses
    A6XX_TP_FLAG_CACHE_MISSES = 2335,
    /// Texture Processor: L1 5 L2 requests
    A6XX_TP_L1_5_L2_REQUESTS = 2336,
    /// Texture Processor: 2D output pixels
    A6XX_TP_2D_OUTPUT_PIXELS = 2337,
    /// Texture Processor: 2D output pixels point
    A6XX_TP_2D_OUTPUT_PIXELS_POINT = 2338,
    /// Texture Processor: 2D output pixels bilinear
    A6XX_TP_2D_OUTPUT_PIXELS_BILINEAR = 2339,
    /// Texture Processor: 2D filter workload 16bit
    A6XX_TP_2D_FILTER_WORKLOAD_16BIT = 2340,
    /// Texture Processor: 2D filter workload 32bit
    A6XX_TP_2D_FILTER_WORKLOAD_32BIT = 2341,
    /// Texture Processor: tpa2tpc trans
    A6XX_TP_TPA2TPC_TRANS = 2342,
    /// Texture Processor: L1 misses astc 1tile
    A6XX_TP_L1_MISSES_ASTC_1TILE = 2343,
    /// Texture Processor: L1 misses astc 2tile
    A6XX_TP_L1_MISSES_ASTC_2TILE = 2344,
    /// Texture Processor: L1 misses astc 4tile
    A6XX_TP_L1_MISSES_ASTC_4TILE = 2345,
    /// Texture Processor: L1 5 L2 compress reqs
    A6XX_TP_L1_5_L2_COMPRESS_REQS = 2346,
    /// Texture Processor: L1 5 L2 compress miss
    A6XX_TP_L1_5_L2_COMPRESS_MISS = 2347,
    /// Texture Processor: L1 bank conflict
    A6XX_TP_L1_BANK_CONFLICT = 2348,
    /// Texture Processor: L1 5 miss latency cycles
    A6XX_TP_L1_5_MISS_LATENCY_CYCLES = 2349,
    /// Texture Processor: L1 5 miss latency trans
    A6XX_TP_L1_5_MISS_LATENCY_TRANS = 2350,
    /// Texture Processor: quads constant multiplied
    A6XX_TP_QUADS_CONSTANT_MULTIPLIED = 2351,
    /// Texture Processor: frontend working cycles
    A6XX_TP_FRONTEND_WORKING_CYCLES = 2352,
    /// Texture Processor: L1 tag working cycles
    A6XX_TP_L1_TAG_WORKING_CYCLES = 2353,
    /// Texture Processor: L1 data write working cycles
    A6XX_TP_L1_DATA_WRITE_WORKING_CYCLES = 2354,
    /// Texture Processor: pre L1 decom working cycles
    A6XX_TP_PRE_L1_DECOM_WORKING_CYCLES = 2355,
    /// Texture Processor: backend working cycles
    A6XX_TP_BACKEND_WORKING_CYCLES = 2356,
    /// Texture Processor: flag cache working cycles
    A6XX_TP_FLAG_CACHE_WORKING_CYCLES = 2357,
    /// Texture Processor: L1 5 cache working cycles
    A6XX_TP_L1_5_CACHE_WORKING_CYCLES = 2358,
    /// Texture Processor: starve cycles SP
    A6XX_TP_STARVE_CYCLES_SP = 2359,
    /// Texture Processor: starve cycles UCHE
    A6XX_TP_STARVE_CYCLES_UCHE = 2360,
    /// Shader/Streaming Processor: busy cycles
    A6XX_SP_BUSY_CYCLES = 2560,
    /// Shader/Streaming Processor: ALU working cycles
    A6XX_SP_ALU_WORKING_CYCLES = 2561,
    /// Shader/Streaming Processor: EFU working cycles
    A6XX_SP_EFU_WORKING_CYCLES = 2562,
    /// Shader/Streaming Processor: stall cycles VPC
    A6XX_SP_STALL_CYCLES_VPC = 2563,
    /// Shader/Streaming Processor: stall cycles TP
    A6XX_SP_STALL_CYCLES_TP = 2564,
    /// Shader/Streaming Processor: stall cycles UCHE
    A6XX_SP_STALL_CYCLES_UCHE = 2565,
    /// Shader/Streaming Processor: stall cycles RB
    A6XX_SP_STALL_CYCLES_RB = 2566,
    /// Shader/Streaming Processor: non execution cycles
    A6XX_SP_NON_EXECUTION_CYCLES = 2567,
    /// Shader/Streaming Processor: wave contexts
    A6XX_SP_WAVE_CONTEXTS = 2568,
    /// Shader/Streaming Processor: wave context cycles
    A6XX_SP_WAVE_CONTEXT_CYCLES = 2569,
    /// Shader/Streaming Processor: FS stage wave cycles
    A6XX_SP_FS_STAGE_WAVE_CYCLES = 2570,
    /// Shader/Streaming Processor: FS stage wave samples
    A6XX_SP_FS_STAGE_WAVE_SAMPLES = 2571,
    /// Shader/Streaming Processor: VS stage wave cycles
    A6XX_SP_VS_STAGE_WAVE_CYCLES = 2572,
    /// Shader/Streaming Processor: VS stage wave samples
    A6XX_SP_VS_STAGE_WAVE_SAMPLES = 2573,
    /// Shader/Streaming Processor: FS stage duration cycles
    A6XX_SP_FS_STAGE_DURATION_CYCLES = 2574,
    /// Shader/Streaming Processor: VS stage duration cycles
    A6XX_SP_VS_STAGE_DURATION_CYCLES = 2575,
    /// Shader/Streaming Processor: wave ctrl cycles
    A6XX_SP_WAVE_CTRL_CYCLES = 2576,
    /// Shader/Streaming Processor: wave load cycles
    A6XX_SP_WAVE_LOAD_CYCLES = 2577,
    /// Shader/Streaming Processor: wave emit cycles
    A6XX_SP_WAVE_EMIT_CYCLES = 2578,
    /// Shader/Streaming Processor: wave nop cycles
    A6XX_SP_WAVE_NOP_CYCLES = 2579,
    /// Shader/Streaming Processor: wave wait cycles
    A6XX_SP_WAVE_WAIT_CYCLES = 2580,
    /// Shader/Streaming Processor: wave fetch cycles
    A6XX_SP_WAVE_FETCH_CYCLES = 2581,
    /// Shader/Streaming Processor: wave idle cycles
    A6XX_SP_WAVE_IDLE_CYCLES = 2582,
    /// Shader/Streaming Processor: wave end cycles
    A6XX_SP_WAVE_END_CYCLES = 2583,
    /// Shader/Streaming Processor: wave long sync cycles
    A6XX_SP_WAVE_LONG_SYNC_CYCLES = 2584,
    /// Shader/Streaming Processor: wave short sync cycles
    A6XX_SP_WAVE_SHORT_SYNC_CYCLES = 2585,
    /// Shader/Streaming Processor: wave join cycles
    A6XX_SP_WAVE_JOIN_CYCLES = 2586,
    /// Shader/Streaming Processor: LM load instructions
    A6XX_SP_LM_LOAD_INSTRUCTIONS = 2587,
    /// Shader/Streaming Processor: LM store instructions
    A6XX_SP_LM_STORE_INSTRUCTIONS = 2588,
    /// Shader/Streaming Processor: LM atomics
    A6XX_SP_LM_ATOMICS = 2589,
    /// Shader/Streaming Processor: GM load instructions
    A6XX_SP_GM_LOAD_INSTRUCTIONS = 2590,
    /// Shader/Streaming Processor: GM store instructions
    A6XX_SP_GM_STORE_INSTRUCTIONS = 2591,
    /// Shader/Streaming Processor: GM atomics
    A6XX_SP_GM_ATOMICS = 2592,
    /// Shader/Streaming Processor: VS stage tex instructions
    A6XX_SP_VS_STAGE_TEX_INSTRUCTIONS = 2593,
    /// Shader/Streaming Processor: VS stage EFU instructions
    A6XX_SP_VS_STAGE_EFU_INSTRUCTIONS = 2594,
    /// Shader/Streaming Processor: VS stage full ALU instructions
    A6XX_SP_VS_STAGE_FULL_ALU_INSTRUCTIONS = 2595,
    /// Shader/Streaming Processor: VS stage half ALU instructions
    A6XX_SP_VS_STAGE_HALF_ALU_INSTRUCTIONS = 2596,
    /// Shader/Streaming Processor: FS stage tex instructions
    A6XX_SP_FS_STAGE_TEX_INSTRUCTIONS = 2597,
    /// Shader/Streaming Processor: FS stage cflow instructions
    A6XX_SP_FS_STAGE_CFLOW_INSTRUCTIONS = 2598,
    /// Shader/Streaming Processor: FS stage EFU instructions
    A6XX_SP_FS_STAGE_EFU_INSTRUCTIONS = 2599,
    /// Shader/Streaming Processor: FS stage full ALU instructions
    A6XX_SP_FS_STAGE_FULL_ALU_INSTRUCTIONS = 2600,
    /// Shader/Streaming Processor: FS stage half ALU instructions
    A6XX_SP_FS_STAGE_HALF_ALU_INSTRUCTIONS = 2601,
    /// Shader/Streaming Processor: FS stage bary instructions
    A6XX_SP_FS_STAGE_BARY_INSTRUCTIONS = 2602,
    /// Shader/Streaming Processor: VS instructions
    A6XX_SP_VS_INSTRUCTIONS = 2603,
    /// Shader/Streaming Processor: FS instructions
    A6XX_SP_FS_INSTRUCTIONS = 2604,
    /// Shader/Streaming Processor: addr lock count
    A6XX_SP_ADDR_LOCK_COUNT = 2605,
    /// Shader/Streaming Processor: UCHE read trans
    A6XX_SP_UCHE_READ_TRANS = 2606,
    /// Shader/Streaming Processor: UCHE write trans
    A6XX_SP_UCHE_WRITE_TRANS = 2607,
    /// Shader/Streaming Processor: export VPC trans
    A6XX_SP_EXPORT_VPC_TRANS = 2608,
    /// Shader/Streaming Processor: export RB trans
    A6XX_SP_EXPORT_RB_TRANS = 2609,
    /// Shader/Streaming Processor: pixels killed
    A6XX_SP_PIXELS_KILLED = 2610,
    /// Shader/Streaming Processor: icl1 requests
    A6XX_SP_ICL1_REQUESTS = 2611,
    /// Shader/Streaming Processor: icl1 misses
    A6XX_SP_ICL1_MISSES = 2612,
    /// Shader/Streaming Processor: HS instructions
    A6XX_SP_HS_INSTRUCTIONS = 2613,
    /// Shader/Streaming Processor: DS instructions
    A6XX_SP_DS_INSTRUCTIONS = 2614,
    /// Shader/Streaming Processor: GS instructions
    A6XX_SP_GS_INSTRUCTIONS = 2615,
    /// Shader/Streaming Processor: CS instructions
    A6XX_SP_CS_INSTRUCTIONS = 2616,
    /// Shader/Streaming Processor: GPR read
    A6XX_SP_GPR_READ = 2617,
    /// Shader/Streaming Processor: GPR write
    A6XX_SP_GPR_WRITE = 2618,
    /// Shader/Streaming Processor: FS stage half EFU instructions
    A6XX_SP_FS_STAGE_HALF_EFU_INSTRUCTIONS = 2619,
    /// Shader/Streaming Processor: VS stage half EFU instructions
    A6XX_SP_VS_STAGE_HALF_EFU_INSTRUCTIONS = 2620,
    /// Shader/Streaming Processor: LM bank conflicts
    A6XX_SP_LM_BANK_CONFLICTS = 2621,
    /// Shader/Streaming Processor: tex control working cycles
    A6XX_SP_TEX_CONTROL_WORKING_CYCLES = 2622,
    /// Shader/Streaming Processor: load control working cycles
    A6XX_SP_LOAD_CONTROL_WORKING_CYCLES = 2623,
    /// Shader/Streaming Processor: flow control working cycles
    A6XX_SP_FLOW_CONTROL_WORKING_CYCLES = 2624,
    /// Shader/Streaming Processor: LM working cycles
    A6XX_SP_LM_WORKING_CYCLES = 2625,
    /// Shader/Streaming Processor: dispatcher working cycles
    A6XX_SP_DISPATCHER_WORKING_CYCLES = 2626,
    /// Shader/Streaming Processor: sequencer working cycles
    A6XX_SP_SEQUENCER_WORKING_CYCLES = 2627,
    /// Shader/Streaming Processor: low efficiency starved BY TP
    A6XX_SP_LOW_EFFICIENCY_STARVED_BY_TP = 2628,
    /// Shader/Streaming Processor: starve cycles HLSQ
    A6XX_SP_STARVE_CYCLES_HLSQ = 2629,
    /// Shader/Streaming Processor: non execution LS cycles
    A6XX_SP_NON_EXECUTION_LS_CYCLES = 2630,
    /// Shader/Streaming Processor: working EU
    A6XX_SP_WORKING_EU = 2631,
    /// Shader/Streaming Processor: any EU working
    A6XX_SP_ANY_EU_WORKING = 2632,
    /// Shader/Streaming Processor: working EU FS stage
    A6XX_SP_WORKING_EU_FS_STAGE = 2633,
    /// Shader/Streaming Processor: any EU working FS stage
    A6XX_SP_ANY_EU_WORKING_FS_STAGE = 2634,
    /// Shader/Streaming Processor: working EU VS stage
    A6XX_SP_WORKING_EU_VS_STAGE = 2635,
    /// Shader/Streaming Processor: any EU working VS stage
    A6XX_SP_ANY_EU_WORKING_VS_STAGE = 2636,
    /// Shader/Streaming Processor: working EU CS stage
    A6XX_SP_WORKING_EU_CS_STAGE = 2637,
    /// Shader/Streaming Processor: any EU working CS stage
    A6XX_SP_ANY_EU_WORKING_CS_STAGE = 2638,
    /// Shader/Streaming Processor: GPR read prefetch
    A6XX_SP_GPR_READ_PREFETCH = 2639,
    /// Shader/Streaming Processor: GPR read conflict
    A6XX_SP_GPR_READ_CONFLICT = 2640,
    /// Shader/Streaming Processor: GPR write conflict
    A6XX_SP_GPR_WRITE_CONFLICT = 2641,
    /// Shader/Streaming Processor: GM load latency cycles
    A6XX_SP_GM_LOAD_LATENCY_CYCLES = 2642,
    /// Shader/Streaming Processor: GM load latency samples
    A6XX_SP_GM_LOAD_LATENCY_SAMPLES = 2643,
    /// Shader/Streaming Processor: executable waves
    A6XX_SP_EXECUTABLE_WAVES = 2644,
    /// RB: busy cycles
    A6XX_RB_BUSY_CYCLES = 2816,
    /// RB: stall cycles HLSQ
    A6XX_RB_STALL_CYCLES_HLSQ = 2817,
    /// RB: stall cycles fifo0 full
    A6XX_RB_STALL_CYCLES_FIFO0_FULL = 2818,
    /// RB: stall cycles fifo1 full
    A6XX_RB_STALL_CYCLES_FIFO1_FULL = 2819,
    /// RB: stall cycles fifo2 full
    A6XX_RB_STALL_CYCLES_FIFO2_FULL = 2820,
    /// RB: starve cycles SP
    A6XX_RB_STARVE_CYCLES_SP = 2821,
    /// RB: starve cycles LRZ tile
    A6XX_RB_STARVE_CYCLES_LRZ_TILE = 2822,
    /// RB: starve cycles CCU
    A6XX_RB_STARVE_CYCLES_CCU = 2823,
    /// RB: starve cycles Z plane
    A6XX_RB_STARVE_CYCLES_Z_PLANE = 2824,
    /// RB: starve cycles bary plane
    A6XX_RB_STARVE_CYCLES_BARY_PLANE = 2825,
    /// RB: Z workload
    A6XX_RB_Z_WORKLOAD = 2826,
    /// RB: HLSQ active
    A6XX_RB_HLSQ_ACTIVE = 2827,
    /// RB: Z read
    A6XX_RB_Z_READ = 2828,
    /// RB: Z write
    A6XX_RB_Z_WRITE = 2829,
    /// RB: C read
    A6XX_RB_C_READ = 2830,
    /// RB: C write
    A6XX_RB_C_WRITE = 2831,
    /// RB: total pass
    A6XX_RB_TOTAL_PASS = 2832,
    /// RB: Z pass
    A6XX_RB_Z_PASS = 2833,
    /// RB: Z fail
    A6XX_RB_Z_FAIL = 2834,
    /// RB: S fail
    A6XX_RB_S_FAIL = 2835,
    /// RB: blended fxp components
    A6XX_RB_BLENDED_FXP_COMPONENTS = 2836,
    /// RB: blended fp16 components
    A6XX_RB_BLENDED_FP16_COMPONENTS = 2837,
    /// RB: PS invocations
    A6XX_RB_PS_INVOCATIONS = 2838,
    /// RB: 2D alive cycles
    A6XX_RB_2D_ALIVE_CYCLES = 2839,
    /// RB: 2D stall cycles a2d
    A6XX_RB_2D_STALL_CYCLES_A2D = 2840,
    /// RB: 2D starve cycles src
    A6XX_RB_2D_STARVE_CYCLES_SRC = 2841,
    /// RB: 2D starve cycles SP
    A6XX_RB_2D_STARVE_CYCLES_SP = 2842,
    /// RB: 2D starve cycles dst
    A6XX_RB_2D_STARVE_CYCLES_DST = 2843,
    /// RB: 2D valid pixels
    A6XX_RB_2D_VALID_PIXELS = 2844,
    /// RB: 3D pixels
    A6XX_RB_3D_PIXELS = 2845,
    /// RB: blender working cycles
    A6XX_RB_BLENDER_WORKING_CYCLES = 2846,
    /// RB: zproc working cycles
    A6XX_RB_ZPROC_WORKING_CYCLES = 2847,
    /// RB: cproc working cycles
    A6XX_RB_CPROC_WORKING_CYCLES = 2848,
    /// RB: sampler working cycles
    A6XX_RB_SAMPLER_WORKING_CYCLES = 2849,
    /// RB: stall cycles CCU color read
    A6XX_RB_STALL_CYCLES_CCU_COLOR_READ = 2850,
    /// RB: stall cycles CCU color write
    A6XX_RB_STALL_CYCLES_CCU_COLOR_WRITE = 2851,
    /// RB: stall cycles CCU depth read
    A6XX_RB_STALL_CYCLES_CCU_DEPTH_READ = 2852,
    /// RB: stall cycles CCU depth write
    A6XX_RB_STALL_CYCLES_CCU_DEPTH_WRITE = 2853,
    /// RB: stall cycles VPC
    A6XX_RB_STALL_CYCLES_VPC = 2854,
    /// RB: 2D input trans
    A6XX_RB_2D_INPUT_TRANS = 2855,
    /// RB: 2D output RB dst trans
    A6XX_RB_2D_OUTPUT_RB_DST_TRANS = 2856,
    /// RB: 2D output RB src trans
    A6XX_RB_2D_OUTPUT_RB_SRC_TRANS = 2857,
    /// RB: blended fp32 components
    A6XX_RB_BLENDED_FP32_COMPONENTS = 2858,
    /// RB: color pix tiles
    A6XX_RB_COLOR_PIX_TILES = 2859,
    /// RB: stall cycles CCU
    A6XX_RB_STALL_CYCLES_CCU = 2860,
    /// RB: early Z arb3 grant
    A6XX_RB_EARLY_Z_ARB3_GRANT = 2861,
    /// RB: late Z arb3 grant
    A6XX_RB_LATE_Z_ARB3_GRANT = 2862,
    /// RB: early Z skip grant
    A6XX_RB_EARLY_Z_SKIP_GRANT = 2863,
    /// VSC: busy cycles
    A6XX_VSC_BUSY_CYCLES = 5888,
    /// VSC: working cycles
    A6XX_VSC_WORKING_CYCLES = 5889,
    /// VSC: stall cycles UCHE
    A6XX_VSC_STALL_CYCLES_UCHE = 5890,
    /// VSC: eot num
    A6XX_VSC_EOT_NUM = 5891,
    /// VSC: input tiles
    A6XX_VSC_INPUT_TILES = 5892,
    /// Cache and Compression Unit: busy cycles
    A6XX_CCU_BUSY_CYCLES = 6144,
    /// Cache and Compression Unit: stall cycles RB depth return
    A6XX_CCU_STALL_CYCLES_RB_DEPTH_RETURN = 6145,
    /// Cache and Compression Unit: stall cycles RB color return
    A6XX_CCU_STALL_CYCLES_RB_COLOR_RETURN = 6146,
    /// Cache and Compression Unit: starve cycles flag return
    A6XX_CCU_STARVE_CYCLES_FLAG_RETURN = 6147,
    /// Cache and Compression Unit: depth blocks
    A6XX_CCU_DEPTH_BLOCKS = 6148,
    /// Cache and Compression Unit: color blocks
    A6XX_CCU_COLOR_BLOCKS = 6149,
    /// Cache and Compression Unit: depth block hit
    A6XX_CCU_DEPTH_BLOCK_HIT = 6150,
    /// Cache and Compression Unit: color block hit
    A6XX_CCU_COLOR_BLOCK_HIT = 6151,
    /// Cache and Compression Unit: partial block read
    A6XX_CCU_PARTIAL_BLOCK_READ = 6152,
    /// Cache and Compression Unit: gmem read
    A6XX_CCU_GMEM_READ = 6153,
    /// Cache and Compression Unit: gmem write
    A6XX_CCU_GMEM_WRITE = 6154,
    /// Cache and Compression Unit: depth read flag0 count
    A6XX_CCU_DEPTH_READ_FLAG0_COUNT = 6155,
    /// Cache and Compression Unit: depth read flag1 count
    A6XX_CCU_DEPTH_READ_FLAG1_COUNT = 6156,
    /// Cache and Compression Unit: depth read flag2 count
    A6XX_CCU_DEPTH_READ_FLAG2_COUNT = 6157,
    /// Cache and Compression Unit: depth read flag3 count
    A6XX_CCU_DEPTH_READ_FLAG3_COUNT = 6158,
    /// Cache and Compression Unit: depth read flag4 count
    A6XX_CCU_DEPTH_READ_FLAG4_COUNT = 6159,
    /// Cache and Compression Unit: depth read flag5 count
    A6XX_CCU_DEPTH_READ_FLAG5_COUNT = 6160,
    /// Cache and Compression Unit: depth read flag6 count
    A6XX_CCU_DEPTH_READ_FLAG6_COUNT = 6161,
    /// Cache and Compression Unit: depth read flag8 count
    A6XX_CCU_DEPTH_READ_FLAG8_COUNT = 6162,
    /// Cache and Compression Unit: color read flag0 count
    A6XX_CCU_COLOR_READ_FLAG0_COUNT = 6163,
    /// Cache and Compression Unit: color read flag1 count
    A6XX_CCU_COLOR_READ_FLAG1_COUNT = 6164,
    /// Cache and Compression Unit: color read flag2 count
    A6XX_CCU_COLOR_READ_FLAG2_COUNT = 6165,
    /// Cache and Compression Unit: color read flag3 count
    A6XX_CCU_COLOR_READ_FLAG3_COUNT = 6166,
    /// Cache and Compression Unit: color read flag4 count
    A6XX_CCU_COLOR_READ_FLAG4_COUNT = 6167,
    /// Cache and Compression Unit: color read flag5 count
    A6XX_CCU_COLOR_READ_FLAG5_COUNT = 6168,
    /// Cache and Compression Unit: color read flag6 count
    A6XX_CCU_COLOR_READ_FLAG6_COUNT = 6169,
    /// Cache and Compression Unit: color read flag8 count
    A6XX_CCU_COLOR_READ_FLAG8_COUNT = 6170,
    /// Cache and Compression Unit: 2D RD req
    A6XX_CCU_2D_RD_REQ = 6171,
    /// Cache and Compression Unit: 2D WR req
    A6XX_CCU_2D_WR_REQ = 6172,
    /// Low Resolution Z: busy cycles
    A6XX_LRZ_BUSY_CYCLES = 6400,
    /// Low Resolution Z: starve cycles RAS
    A6XX_LRZ_STARVE_CYCLES_RAS = 6401,
    /// Low Resolution Z: stall cycles RB
    A6XX_LRZ_STALL_CYCLES_RB = 6402,
    /// Low Resolution Z: stall cycles VSC
    A6XX_LRZ_STALL_CYCLES_VSC = 6403,
    /// Low Resolution Z: stall cycles VPC
    A6XX_LRZ_STALL_CYCLES_VPC = 6404,
    /// Low Resolution Z: stall cycles flag prefetch
    A6XX_LRZ_STALL_CYCLES_FLAG_PREFETCH = 6405,
    /// Low Resolution Z: stall cycles UCHE
    A6XX_LRZ_STALL_CYCLES_UCHE = 6406,
    /// Low Resolution Z: LRZ read
    A6XX_LRZ_LRZ_READ = 6407,
    /// Low Resolution Z: LRZ write
    A6XX_LRZ_LRZ_WRITE = 6408,
    /// Low Resolution Z: read latency
    A6XX_LRZ_READ_LATENCY = 6409,
    /// Low Resolution Z: merge cache updating
    A6XX_LRZ_MERGE_CACHE_UPDATING = 6410,
    /// Low Resolution Z: prim killed BY maskgen
    A6XX_LRZ_PRIM_KILLED_BY_MASKGEN = 6411,
    /// Low Resolution Z: prim killed BY LRZ
    A6XX_LRZ_PRIM_KILLED_BY_LRZ = 6412,
    /// Low Resolution Z: visible prim after LRZ
    A6XX_LRZ_VISIBLE_PRIM_AFTER_LRZ = 6413,
    /// Low Resolution Z: full 8x8 tiles
    A6XX_LRZ_FULL_8X8_TILES = 6414,
    /// Low Resolution Z: partial 8x8 tiles
    A6XX_LRZ_PARTIAL_8X8_TILES = 6415,
    /// Low Resolution Z: tile killed
    A6XX_LRZ_TILE_KILLED = 6416,
    /// Low Resolution Z: total pixel
    A6XX_LRZ_TOTAL_PIXEL = 6417,
    /// Low Resolution Z: visible pixel after LRZ
    A6XX_LRZ_VISIBLE_PIXEL_AFTER_LRZ = 6418,
    /// Low Resolution Z: fully covered tiles
    A6XX_LRZ_FULLY_COVERED_TILES = 6419,
    /// Low Resolution Z: partial covered tiles
    A6XX_LRZ_PARTIAL_COVERED_TILES = 6420,
    /// Low Resolution Z: feedback accept
    A6XX_LRZ_FEEDBACK_ACCEPT = 6421,
    /// Low Resolution Z: feedback discard
    A6XX_LRZ_FEEDBACK_DISCARD = 6422,
    /// Low Resolution Z: feedback stall
    A6XX_LRZ_FEEDBACK_STALL = 6423,
    /// Low Resolution Z: stall cycles RB zplane
    A6XX_LRZ_STALL_CYCLES_RB_ZPLANE = 6424,
    /// Low Resolution Z: stall cycles RB bplane
    A6XX_LRZ_STALL_CYCLES_RB_BPLANE = 6425,
    /// Low Resolution Z: stall cycles VC
    A6XX_LRZ_STALL_CYCLES_VC = 6426,
    /// Low Resolution Z: RAS mask trans
    A6XX_LRZ_RAS_MASK_TRANS = 6427,
    /// CMP: cmpdecmp stall cycles arb
    A6XX_CMP_CMPDECMP_STALL_CYCLES_ARB = 6656,
    /// CMP: cmpdecmp vbif latency cycles
    A6XX_CMP_CMPDECMP_VBIF_LATENCY_CYCLES = 6657,
    /// CMP: cmpdecmp vbif latency samples
    A6XX_CMP_CMPDECMP_VBIF_LATENCY_SAMPLES = 6658,
    /// CMP: cmpdecmp vbif read data CCU
    A6XX_CMP_CMPDECMP_VBIF_READ_DATA_CCU = 6659,
    /// CMP: cmpdecmp vbif write data CCU
    A6XX_CMP_CMPDECMP_VBIF_WRITE_DATA_CCU = 6660,
    /// CMP: cmpdecmp vbif read request
    A6XX_CMP_CMPDECMP_VBIF_READ_REQUEST = 6661,
    /// CMP: cmpdecmp vbif write request
    A6XX_CMP_CMPDECMP_VBIF_WRITE_REQUEST = 6662,
    /// CMP: cmpdecmp vbif read data
    A6XX_CMP_CMPDECMP_VBIF_READ_DATA = 6663,
    /// CMP: cmpdecmp vbif write data
    A6XX_CMP_CMPDECMP_VBIF_WRITE_DATA = 6664,
    /// CMP: cmpdecmp flag fetch cycles
    A6XX_CMP_CMPDECMP_FLAG_FETCH_CYCLES = 6665,
    /// CMP: cmpdecmp flag fetch samples
    A6XX_CMP_CMPDECMP_FLAG_FETCH_SAMPLES = 6666,
    /// CMP: cmpdecmp depth write flag1 count
    A6XX_CMP_CMPDECMP_DEPTH_WRITE_FLAG1_COUNT = 6667,
    /// CMP: cmpdecmp depth write flag2 count
    A6XX_CMP_CMPDECMP_DEPTH_WRITE_FLAG2_COUNT = 6668,
    /// CMP: cmpdecmp depth write flag3 count
    A6XX_CMP_CMPDECMP_DEPTH_WRITE_FLAG3_COUNT = 6669,
    /// CMP: cmpdecmp depth write flag4 count
    A6XX_CMP_CMPDECMP_DEPTH_WRITE_FLAG4_COUNT = 6670,
    /// CMP: cmpdecmp depth write flag5 count
    A6XX_CMP_CMPDECMP_DEPTH_WRITE_FLAG5_COUNT = 6671,
    /// CMP: cmpdecmp depth write flag6 count
    A6XX_CMP_CMPDECMP_DEPTH_WRITE_FLAG6_COUNT = 6672,
    /// CMP: cmpdecmp depth write flag8 count
    A6XX_CMP_CMPDECMP_DEPTH_WRITE_FLAG8_COUNT = 6673,
    /// CMP: cmpdecmp color write flag1 count
    A6XX_CMP_CMPDECMP_COLOR_WRITE_FLAG1_COUNT = 6674,
    /// CMP: cmpdecmp color write flag2 count
    A6XX_CMP_CMPDECMP_COLOR_WRITE_FLAG2_COUNT = 6675,
    /// CMP: cmpdecmp color write flag3 count
    A6XX_CMP_CMPDECMP_COLOR_WRITE_FLAG3_COUNT = 6676,
    /// CMP: cmpdecmp color write flag4 count
    A6XX_CMP_CMPDECMP_COLOR_WRITE_FLAG4_COUNT = 6677,
    /// CMP: cmpdecmp color write flag5 count
    A6XX_CMP_CMPDECMP_COLOR_WRITE_FLAG5_COUNT = 6678,
    /// CMP: cmpdecmp color write flag6 count
    A6XX_CMP_CMPDECMP_COLOR_WRITE_FLAG6_COUNT = 6679,
    /// CMP: cmpdecmp color write flag8 count
    A6XX_CMP_CMPDECMP_COLOR_WRITE_FLAG8_COUNT = 6680,
    /// CMP: cmpdecmp 2D stall cycles vbif req
    A6XX_CMP_CMPDECMP_2D_STALL_CYCLES_VBIF_REQ = 6681,
    /// CMP: cmpdecmp 2D stall cycles vbif WR
    A6XX_CMP_CMPDECMP_2D_STALL_CYCLES_VBIF_WR = 6682,
    /// CMP: cmpdecmp 2D stall cycles vbif return
    A6XX_CMP_CMPDECMP_2D_STALL_CYCLES_VBIF_RETURN = 6683,
    /// CMP: cmpdecmp 2D RD data
    A6XX_CMP_CMPDECMP_2D_RD_DATA = 6684,
    /// CMP: cmpdecmp 2D WR data
    A6XX_CMP_CMPDECMP_2D_WR_DATA = 6685,
    /// CMP: cmpdecmp vbif read data UCHE ch0
    A6XX_CMP_CMPDECMP_VBIF_READ_DATA_UCHE_CH0 = 6686,
    /// CMP: cmpdecmp vbif read data UCHE ch1
    A6XX_CMP_CMPDECMP_VBIF_READ_DATA_UCHE_CH1 = 6687,
    /// CMP: cmpdecmp 2D output trans
    A6XX_CMP_CMPDECMP_2D_OUTPUT_TRANS = 6688,
    /// CMP: cmpdecmp vbif write data UCHE
    A6XX_CMP_CMPDECMP_VBIF_WRITE_DATA_UCHE = 6689,
    /// CMP: cmpdecmp depth write flag0 count
    A6XX_CMP_CMPDECMP_DEPTH_WRITE_FLAG0_COUNT = 6690,
    /// CMP: cmpdecmp color write flag0 count
    A6XX_CMP_CMPDECMP_COLOR_WRITE_FLAG0_COUNT = 6691,
    /// CMP: cmpdecmp color write flagalpha count
    A6XX_CMP_CMPDECMP_COLOR_WRITE_FLAGALPHA_COUNT = 6692,
    /// CMP: cmpdecmp 2D busy cycles
    A6XX_CMP_CMPDECMP_2D_BUSY_CYCLES = 6693,
    /// CMP: cmpdecmp 2D reorder starve cycles
    A6XX_CMP_CMPDECMP_2D_REORDER_STARVE_CYCLES = 6694,
    /// CMP: cmpdecmp 2D pixels
    A6XX_CMP_CMPDECMP_2D_PIXELS = 6695,
};