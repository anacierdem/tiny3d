## Auto-generated file, transpiled with RSPL
#define TRI_BUFFER_COUNT 70
#define LIGHT_COUNT 4
#define RDPQ_TRIANGLE_CUSTOM_VTX 1
#define VTX_ATTR_XY 0x00
#define VTX_ATTR_Z 0x04
#define VTX_ATTR_CLIPFLAGS 0x06
#define VTX_ATTR_REJFLAGS 0x07
#define VTX_ATTR_RGBA 0x08
#define VTX_ATTR_ST 0x0C
#define VTX_ATTR_CLIPPOSi 0x10
#define VTX_ATTR_Wi 0x16
#define VTX_ATTR_CLIPPOSf 0x18
#define VTX_ATTR_Wf 0x1E
#define VTX_ATTR_INVWi 0x20
#define VTX_ATTR_INVWf 0x22
#define RDPQ_TRIANGLE_VTX1 a0
#define RDPQ_TRIANGLE_VTX2 a1
#define RDPQ_TRIANGLE_VTX3 a2
#define RDPQ_TRIANGLE_VTX1_DMEM 0,v1
#define RDPQ_TRIANGLE_VTX2_DMEM 4,v1
#define RDPQ_TRIANGLE_VTX3_DMEM 2,v1
#define TRI_SIZE 36
#define TRI_SIZE_2 72
#define TRI_SIZE_3 108
#define RDP_CMD_MAX_SIZE 176
#define LIGHT_SIZE 16
#define MATRIX_SIZE 0x40
#define DRAW_TRIS 1
#define DP_STATUS_END_VALID 512
#define RDPQ_DYNAMIC_BUFFER_SIZE 65536
#define RDP_CMD_PRIM 0x3A00'0000
#define RDP_CMD_DEPTH 0x2E
#define RDP_CMD_RECT 0x36
#define PARTICLE_INPUT_SIZE 16
#define RDP_FLUSH_SIZE 504
#define STATE_PART_SCALE UV_GEN_PARAMS
#define RDP_POS_MASK 0b1111'1111'1111
#include <rsp_queue.inc>
#include <rdpq_macros.h>
#include "./asm/rspq_triangle.inc"

.set noreorder
.set noat
.set nomacro

#undef zero
#undef at
#undef v0
#undef v1
#undef a0
#undef a1
#undef a2
#undef a3
#undef t0
#undef t1
#undef t2
#undef t3
#undef t4
#undef t5
#undef t6
#undef t7
#undef s0
#undef s1
#undef s2
#undef s3
#undef s4
#undef s5
#undef s6
#undef s7
#undef t8
#undef t9
#undef k0
#undef k1
#undef gp
#undef sp
#undef fp
#undef ra
.equ hex.$zero, 0
.equ hex.$at, 1
.equ hex.$v0, 2
.equ hex.$v1, 3
.equ hex.$a0, 4
.equ hex.$a1, 5
.equ hex.$a2, 6
.equ hex.$a3, 7
.equ hex.$t0, 8
.equ hex.$t1, 9
.equ hex.$t2, 10
.equ hex.$t3, 11
.equ hex.$t4, 12
.equ hex.$t5, 13
.equ hex.$t6, 14
.equ hex.$t7, 15
.equ hex.$s0, 16
.equ hex.$s1, 17
.equ hex.$s2, 18
.equ hex.$s3, 19
.equ hex.$s4, 20
.equ hex.$s5, 21
.equ hex.$s6, 22
.equ hex.$s7, 23
.equ hex.$t8, 24
.equ hex.$t9, 25
.equ hex.$k0, 26
.equ hex.$k1, 27
.equ hex.$gp, 28
.equ hex.$sp, 29
.equ hex.$fp, 30
.equ hex.$ra, 31
#define vco 0
#define vcc 1
#define vce 2

.data
  RSPQ_BeginOverlayHeader
    RSPQ_DefineCommand RSPQ_Loop, 8
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
    RSPQ_DefineCommand RSPQ_Loop, 4
  RSPQ_EndOverlayHeader

  RSPQ_BeginSavedState
    STATE_MEM_START:
    .align 3
    MATRIX_PROJ: .ds.b 64
    .align 3
    MATRIX_MVP: .ds.b 64
    .align 3
    MATRIX_NORMAL: .ds.b 24
    TRI_BUFFER: .ds.b 2520
    .align 4
    CLIP_BUFFER_TMP: .ds.b 252
    .align 1
    CLIP_RA: .ds.b 2
    .align 4
    CLIP_BUFFER_RESULT: .ds.b 288
    .align 4
    SCREEN_SCALE_OFFSET: .half 0, 0, 0, 0, 0, 0, 16383, 255
    .align 4
    NORMAL_MASK_SHIFT: .half 63488, 2016, 31, 0, 1, 32, 2048, 0
    .align 3
    CLIPPING_PLANES: .byte 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, -1, 0, 1, 0, -1
    .align 1
    UV_GEN_PARAMS: .half 0, 0
    .align 3
    NORM_SCALE_W: .half 65535, 65535, 65535, 0, 0, 0, 0, 65535
    .align 2
    SEGMENT_TABLE: .word 0, 0, 0, 0, 0, 0, 0, 0
    .align 2
    COLOR_AMBIENT: .ds.b 8
    .align 2
    LIGHT_DIR_COLOR: .ds.b 64
    .align 2
    TRI_COMMAND: .word 0
    .align 2
    MATRIX_STACK_PTR: .word 0
    .align 1
    FOG_SCALE_OFFSET: .half 0, 0, 0, 32767
    .align 1
    RDPQ_TRI_BUFF_OFFSET: .half 0
    .align 1
    SCREEN_UVGEN_SCALE: .half 0
    .align 1
    VERTEX_FX_FUNC: .half 0
    FACE_CULLING: .byte 0
    FOG_STORE_OFFSET: .byte 12
    ACTIVE_LIGHT_SIZE: .byte 0
    STATE_MEM_END:
  RSPQ_EndSavedState

  TEMP_STATE_MEM_START:
    .align 2
    CLIP_CODE_ADDR: .word 0
    .align 2
    CLIP_CODE_ORG_ADDR: .word 0
    .align 1
    CLIP_CODE_SIZE: .half 0
  TEMP_STATE_MEM_END:

.text
OVERLAY_CODE_START:

.align 3
T3DCmd_DrawParticles:
  ori $s4, $zero, %lo(TRI_BUFFER)
  srl $t8, $a1, 24
  lw $t8, %lo(SEGMENT_TABLE)($t8)
  addiu $s4, $s4, 8
  or $t2, $zero, $zero
  andi $t0, $a0, 65535
  addu $t6, $s4, $t0
  addu $a1, $a1, $t8
  addiu $t0, $t0, -1
  jal DMAExec ## Args: $t0, $t1, $s0, $s4, $t2
  or $s0, $zero, $a1
  ori $at, $zero, %lo(SCREEN_SCALE_OFFSET)
  ldv $v09, 0, 0, $at
  ori $k0, $zero, %lo(MATRIX_MVP)
  ldv $v11, 0, 8, $at
  ldv $v03, 0, 16, $k0
  ldv $v01, 0, 0, $k0
  ldv $v11, 8, 8, $at
  ldv $v08, 0, 56, $k0
  lw $k1, %lo(RDPQ_SCISSOR_RECT + 0)
  ldv $v02, 0, 8, $k0
  ldv $v09, 8, 0, $at
  ori $at, $zero, %lo(NORM_SCALE_W)
  ldv $v07, 0, 48, $k0
  mtc2 $k1, $v13.e1
  ldv $v03, 8, 16, $k0
  ldv $v04, 0, 24, $k0
  ldv $v06, 0, 40, $k0
  ldv $v05, 0, 32, $k0
  ldv $v07, 8, 48, $k0
  ldv $v01, 8, 0, $k0
  ldv $v08, 8, 56, $k0
  ldv $v05, 8, 32, $k0
  ldv $v04, 8, 24, $k0
  ldv $v06, 8, 40, $k0
  ldv $v02, 8, 8, $k0
  ldv $v12, 0, 0, $at
  lw $k0, %lo(RDPQ_SCISSOR_RECT + 4)
  ldv $v12, 8, 0, $at
  addiu $at, $zero, 4095
  srl $a2, $k0, 12
  mtc2 $at, $v14.e3
  or $t8, $zero, $s4
  vmudl $v10, $v00, $v31.e3
  vmadm $v09, $v09, $v31.e3
  mtc2 $a2, $v14.e4
  mtc2 $a2, $v14.e0
  mtc2 $k0, $v14.e5
  ori $s3, $zero, %lo(CLIP_BUFFER_TMP)
  mtc2 $k0, $v14.e1
  addiu $t9, $s3, 504
  ori $at, $zero, %lo(UV_GEN_PARAMS)
  srl $a2, $k1, 12
  vmadn $v10, $v00, $v00
  vand $v14, $v14, $v14.e3
  mtc2 $a2, $v13.e4
  mtc2 $k1, $v13.e5
  mtc2 $a2, $v13.e0
  lsv $v16, 0, 0, $at
  ori $s4, $zero, %lo(CLIP_BUFFER_TMP)
  jal DMAWaitIdle
  vand $v13, $v13, $v14.e3
  lpv $v19, 0, 0, $t8
  vmudm $v19, $v19, $v31.e7
  vaddc $v15, $v19, $v16.e0
  vmov $v19.e3, $v30.e7
  vmov $v19.e7, $v30.e7
  LABEL_0001:
  vmudn $v18, $v02, $v19.h0
  addiu $a0, $zero, 46
  vmadh $v17, $v01, $v19.h0
  sb $a0, 0 + 8($s3)
  vmadn $v18, $v04, $v19.h1
  vmadh $v17, $v03, $v19.h1
  vmadn $v18, $v06, $v19.h2
  vmadh $v17, $v05, $v19.h2
  vmadn $v18, $v08, $v19.h3
  vmadh $v17, $v07, $v19.h3
  addiu $t2, $zero, 54
  vch $v29, $v17, $v17.h3
  vcl $v29, $v18, $v18.h3
  vmudl $v18, $v18, $v12.v
  lpv $v25, 0, 8, $t8
  vmadm $v17, $v17, $v12.v
  vmadn $v18, $v00, $v00
  cfc2 $t4, $vcc
  vrcph $v20.e3, $v17.e3
  spv $v25, 4, 0, $s3 ## Barrier: 0x1
  vrcpl $v21.e3, $v18.e3
  vrcph $v20.e3, $v17.e7
  vrcpl $v21.e7, $v18.e7
  andi $t1, $t4, 1028
  vrcph $v20.e7, $v00.e7
  vmov $v17.e7, $v15.e7
  vmov $v17.e3, $v15.e3
  lui $t7, 0x3A00
  vmov $v18.e3, $v00.e3
  vmov $v18.e7, $v00.e7
  vmudl $v29, $v18, $v21.h3
  vmadm $v29, $v17, $v21.h3
  andi $t5, $t4, 16448
  addiu $at, $zero, 3
  vmadn $v18, $v18, $v20.h3
  vmadh $v17, $v17, $v20.h3
  vmudl $v29, $v18, $v10.v
  vmadm $v29, $v17, $v10.v
  sw $t7, ($s3) ## Barrier: 0x1
  vmadn $v21, $v18, $v09.v
  vmadh $v20, $v17, $v09.v
  vaddc $v22, $v20, $v11.v
  vaddc $v23, $v22, $v17.h3
  vsubc $v19, $v22, $v17.h3
  vlt $v23, $v23, $v14
  vge $v19, $v19, $v13
  mfc2 $sp, $v23.e1
  mfc2 $s1, $v19.e0
  vlt $v24, $v19, $v23
  mfc2 $k1, $v19.e5
  vmov $v22.e3, $v00.e0
  andi $sp, $sp, 4095
  sll $s1, $s1, 12
  mfc2 $k0, $v19.e1
  mfc2 $fp, $v23.e5
  andi $k1, $k1, 4095
  andi $k0, $k0, 4095
  or $k0, $k0, $s1
  mfc2 $s1, $v19.e4
  sw $k0, 4 + 16($s3)
  lpv $v19, 0, 0, $t8
  sll $s1, $s1, 12
  or $k1, $k1, $s1
  mfc2 $s1, $v23.e0
  cfc2 $t4, $vcc
  vmudm $v19, $v19, $v31.e7
  andi $fp, $fp, 4095
  sll $s1, $s1, 12
  or $sp, $sp, $s1
  andi $a2, $t4, 3
  sw $sp, 0 + 16($s3) ## Barrier: 0x2
  vaddc $v15, $v19, $v16.e0
  mfc2 $s1, $v23.e4
  sb $t2, 0 + 16($s3) ## Barrier: 0x2
  or $t1, $t1, $a2
  sll $s1, $s1, 12
  andi $a2, $t4, 48
  or $fp, $fp, $s1
  slv $v22, 4, 12, $s3
  vmov $v19.e3, $v30.e7
  vmov $v19.e7, $v30.e7
  bne $t1, $at, LABEL_0003
  or $t5, $t5, $a2
  addiu $s3, $s3, 24
  LABEL_0003:
  addiu $at, $zero, 48
  bne $t5, $at, LABEL_0004
  vmov $v22.e7, $v00.e0
  sw $k1, 4 + 16($s3)
  sb $a0, 0 + 8($s3)
  sw $fp, 0 + 16($s3) ## Barrier: 0x2
  slv $v22, 12, 12, $s3
  spv $v25, 0, 0, $s3 ## Barrier: 0x1
  sb $t2, 0 + 16($s3) ## Barrier: 0x2
  sw $t7, ($s3) ## Barrier: 0x1
  addiu $s3, $s3, 24
  LABEL_0004:
  sltu $at, $s3, $t9
  bne $at, $zero, LABEL_0005
  addiu $t8, $t8, 16
  jal RDPQ_Send ## Args: $s4, $s3
  nop
  or $s3, $zero, $s4
  LABEL_0005:
  bne $t8, $t6, LABEL_0001
  nop
  LABEL_0002:
  j RDPQ_Send
  ori $ra, $zero, %lo(RSPQ_Loop)
T3DCmd_DrawParticles_End:
  jr $ra
  nop

OVERLAY_CODE_END:

#define zero $0
#define v0 $2
#define v1 $3
#define a0 $4
#define a1 $5
#define a2 $6
#define a3 $7
#define t0 $8
#define t1 $9
#define t2 $10
#define t3 $11
#define t4 $12
#define t5 $13
#define t6 $14
#define t7 $15
#define s0 $16
#define s1 $17
#define s2 $18
#define s3 $19
#define s4 $20
#define s5 $21
#define s6 $22
#define s7 $23
#define t8 $24
#define t9 $25
#define k0 $26
#define k1 $27
#define gp $28
#define sp $29
#define fp $30
#define ra $31

.set at
.set macro