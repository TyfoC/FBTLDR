#include <pic.h>

VOID SendCommandToPIC(UINT8 vectorIndex, UINT8 command) {
	if (vectorIndex >= 8) OutByte(PIC_SLAVE_PORT_COMMAND, command);
	OutByte(PIC_MASTER_PORT_COMMAND, command);
}

VOID RemapPIC(UINT8 masterVectorOffset, UINT8 slaveVectorOffset) {
	UINT8 masterMask = InByte(PIC_MASTER_PORT_DATA);
	UINT8 slaveMask = InByte(PIC_SLAVE_PORT_DATA);
 
	OutByte(PIC_MASTER_PORT_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
	IOWait();
	OutByte(PIC_SLAVE_PORT_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
	IOWait();
	OutByte(PIC_MASTER_PORT_DATA, masterVectorOffset);
	IOWait();
	OutByte(PIC_SLAVE_PORT_DATA, slaveVectorOffset);
	IOWait();
	OutByte(PIC_MASTER_PORT_DATA, 4);
	IOWait();
	OutByte(PIC_SLAVE_PORT_DATA, 2);
	IOWait();
 
	OutByte(PIC_MASTER_PORT_DATA, PIC_ICW4_8086);
	IOWait();
	OutByte(PIC_SLAVE_PORT_DATA, PIC_ICW4_8086);
	IOWait();
 
	OutByte(PIC_MASTER_PORT_DATA, masterMask);
	OutByte(PIC_SLAVE_PORT_DATA, slaveMask);
}

VOID MaskPICIRQ(UINT8 irqIndex) {
	UINT16 portIndex;
    UINT8 value;
 
	if (irqIndex < 8) portIndex = PIC_MASTER_PORT_DATA;
	else {
		portIndex = PIC_SLAVE_PORT_DATA;
    	irqIndex -= 8;
	}

	value = InByte(portIndex) | (1 << irqIndex);
	OutByte(portIndex, value); 
}

VOID UnmaskPICIRQ(UINT8 irqIndex) {
	UINT16 portIndex;
    UINT8 value;
 
	if (irqIndex < 8) portIndex = PIC_MASTER_PORT_DATA;
	else {
		portIndex = PIC_SLAVE_PORT_DATA;
    	irqIndex -= 8;
	}

	value = InByte(portIndex) & ~(1 << irqIndex);
	OutByte(portIndex, value); 
}

VOID MaskPIC(VOID) {
	OutByte(PIC_MASTER_PORT_DATA, 0xFF);
	OutByte(PIC_SLAVE_PORT_DATA, 0xFF);
}

VOID UnmaskPIC(VOID) {
	OutByte(PIC_MASTER_PORT_DATA, 0);
	OutByte(PIC_SLAVE_PORT_DATA, 0);
}