#pragma once
#ifndef PIC
#define PIC

#include "asm-utils.h"
#include "port.h"

#define PIC_MASTER_PORT_COMMAND	0x20
#define PIC_SLAVE_PORT_COMMAND	0xA0
#define PIC_MASTER_PORT_DATA	0x21
#define PIC_SLAVE_PORT_DATA		0xA1

#define PIC_ICW1_ICW4			0x01		// ICW4 will be present
#define PIC_ICW1_SINGLE			0x02		// cascade mode
#define PIC_ICW1_INTERVAL4		0x04		// call address interval 4(8)
#define PIC_ICW1_LEVEL			0x08		// edge mode
#define PIC_ICW1_INIT			0x10
 
#define PIC_ICW4_8086			0x01		// 8086/88 mode
#define PIC_ICW4_AUTO			0x02		// auto/normal EOI
#define PIC_ICW4_BUF_SLAVE		0x08		// slave buffered mode
#define PIC_ICW4_BUF_MASTER		0x0C		// master buffered mode
#define PIC_ICW4_SFNM			0x10		// special fully nested mode

#define PIC_COMMAND_EOI			0x20		//	end of interrupt

VOID ATTRIB_PACKEDToPIC(UINT8 vectorIndex, UINT8 command);
VOID RemapPIC(UINT8 masterVectorOffset, UINT8 slaveVectorOffset);
VOID MaskPICIRQ(UINT8 irqIndex);
VOID UnmaskPICIRQ(UINT8 irqIndex);
VOID MaskPIC(VOID);
VOID UnmaskPIC(VOID);

#endif