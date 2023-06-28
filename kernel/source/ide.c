#include "ide.h"

static IDE_CONTROLLER* Ctrls = 0;
static SIZE_T NumberOfCtrls = 0;
static UINT8 TmpBuffer[2048];

BOOL InstallIDE(VOID) {
	IDE_CONTROLLER* tmpDatas, tmpData;
	PCI_DEVICE pciDev;
	SIZE_T pciDevsCount = PCIGetDevicesCount();
	SIZE_T bar0, bar1, bar2, bar3, bar4;
	for (SIZE_T i = 0; i < pciDevsCount; i++) {
		PCIGetDevice(i, &pciDev);
		if (pciDev.Class == PCI_CLASS_MASS_STG_CTRL && pciDev.SubClass == PCI_SUBCLASS_MSTGCTRL_IDE_CTRL) {
			bar0 = PCIGetBAR0(pciDev.Bus, pciDev.Slot, pciDev.Function);
			bar1 = PCIGetBAR1(pciDev.Bus, pciDev.Slot, pciDev.Function);
			bar2 = PCIGetBAR2(pciDev.Bus, pciDev.Slot, pciDev.Function);
			bar3 = PCIGetBAR3(pciDev.Bus, pciDev.Slot, pciDev.Function);
			bar4 = PCIGetBAR4(pciDev.Bus, pciDev.Slot, pciDev.Function);

			tmpData = IDEInitCtrl(bar0, bar1, bar2, bar3, bar4);
			if (!tmpData.NumberOfDevices) {
				if (tmpData.Channels) FreePhysicalMemory(tmpData.Channels);
				if (tmpData.Devices) FreePhysicalMemory(tmpData.Devices);
				continue;
			}

			tmpDatas = (IDE_CONTROLLER*)ReallocatePhysicalMemory(Ctrls, sizeof(IDE_CONTROLLER) * (NumberOfCtrls + 1));
			if (!tmpDatas) {
				if (tmpData.Channels) FreePhysicalMemory(tmpData.Channels);
				if (tmpData.Devices) FreePhysicalMemory(tmpData.Devices);
				continue;
			}

			Ctrls = tmpDatas;
			Ctrls[NumberOfCtrls] = tmpData;
			++NumberOfCtrls;
		}
	}

	return TRUE;
}

BOOL IDEGetCtrl(SIZE_T index, IDE_CONTROLLER* ctrl) {
	*ctrl = (IDE_CONTROLLER){};
	if (index >= NumberOfCtrls) return FALSE;
	SIZE_T chnlsSize = Ctrls[index].NumberOfChannels * sizeof(IDE_CHANNEL);
	SIZE_T devsSize = Ctrls[index].NumberOfDevices * sizeof(IDE_DEVICE);

	ctrl->Channels = (IDE_CHANNEL*)AllocatePhysicalMemory(chnlsSize);
	if (!ctrl->Channels) return FALSE;

	ctrl->Devices = (IDE_DEVICE*)AllocatePhysicalMemory(devsSize);
	if (!ctrl->Devices) {
		FreePhysicalMemory(ctrl->Channels);
		ctrl->Channels = 0;
		return FALSE;
	}

	CopyMemory(ctrl->Channels, Ctrls[index].Channels, chnlsSize);
	CopyMemory(ctrl->Devices, Ctrls[index].Devices, devsSize);
	ctrl->NumberOfChannels = Ctrls[index].NumberOfChannels;
	ctrl->NumberOfDevices = Ctrls[index].NumberOfDevices;
	return TRUE;
}

BOOL IDEGetChnl(SIZE_T ctrlIndex, SIZE_T chnlType, IDE_CHANNEL* chnl) {
	if (ctrlIndex >= NumberOfCtrls || chnlType >= Ctrls[ctrlIndex].NumberOfChannels) return FALSE;
	*chnl = Ctrls[ctrlIndex].Channels[chnlType];
	return TRUE;
}

BOOL IDEGetDev(SIZE_T ctrlIndex, SIZE_T devIndex, IDE_DEVICE* dev) {
	if (ctrlIndex >= NumberOfCtrls || devIndex >= Ctrls[ctrlIndex].NumberOfDevices) return FALSE;
	*dev = Ctrls[ctrlIndex].Devices[devIndex];
	return TRUE;
}

BOOL IDEGetChnlDev(SIZE_T ctrlIndex, SIZE_T chnlType, SIZE_T devType, IDE_DEVICE* dev) {
	if (ctrlIndex >= NumberOfCtrls || chnlType >= Ctrls[ctrlIndex].NumberOfChannels) return FALSE;
	
	SIZE_T j;
	for (SIZE_T i = 0; i < Ctrls[ctrlIndex].NumberOfChannels; i++) {
		for (j = 0; j < Ctrls[ctrlIndex].NumberOfDevices; j++) {
			if (Ctrls[ctrlIndex].Devices[j].ChannelType == chnlType && Ctrls[ctrlIndex].Devices[j].Type == devType) {
				*dev = Ctrls[ctrlIndex].Devices[j];
				return TRUE;
			}
		}
	}

	return FALSE;
}

SIZE_T IDEGetCtrlsCount(VOID) {
	return NumberOfCtrls;
}

SIZE_T IDEGetChnlsCount(SIZE_T ctrlIndex) {
	return ctrlIndex >= NumberOfCtrls ? 0 : Ctrls[ctrlIndex].NumberOfChannels;
}

SIZE_T IDEGetDevsCount(SIZE_T ctrlIndex) {
	if (ctrlIndex >= NumberOfCtrls) return 0;
	return ctrlIndex >= NumberOfCtrls ? 0 : Ctrls[ctrlIndex].NumberOfDevices;
}

UINT8 IDEReadSector(SIZE_T ctrlIndex, SIZE_T devIndex, VOID* destination, UINT32 lba, UINT8 count) {
	if (ctrlIndex >= NumberOfCtrls || devIndex >= Ctrls[ctrlIndex].NumberOfDevices) return IDE_TRANSFER_STATUS_NO_DEVICE;

	IDE_DEVICE* dev = &Ctrls[ctrlIndex].Devices[devIndex];
	IDE_CHANNEL* chnl = &Ctrls[ctrlIndex].Channels[dev->ChannelType];

	UINT8 transferMode, cmd;
	UINT8 lbaIO[6], headIndex, sectorNumber, errVal;
	UINT16 cylIndex, i, j;

	IDEWrite(chnl, ATA_OFF_CTRL, chnl->IntDisabled ? ATA_CTRL_IRQ_DISABLED : 0);

	if ((lba & 0xFF000000)) {
		if (!(dev->Capabilities & 0x200)) return IDE_TRANSFER_STATUS_NO_LBA48;	//	lba 28/48 N/S
		transferMode = TRANSFER_MODE_LBA48;
		lbaIO[0] = lba & 0xFF;
		lbaIO[1] = (lba & 0xFF00) >> 8;
		lbaIO[2] = (lba & 0xFF0000) >> 16;
		lbaIO[3] = (lba & 0xFF000000) >> 24;
		lbaIO[4] = lbaIO[5] = headIndex = 0;
	}
	else if (dev->Capabilities & 0x200) {
		transferMode = TRANSFER_MODE_LBA28;
		lbaIO[0] = lba & 0xFF;
		lbaIO[1] = (lba & 0xFF00) >> 8;
		lbaIO[2] = (lba & 0xFF0000) >> 16;
		lbaIO[3] = lbaIO[4] = lbaIO[5] = 0;
		headIndex = (lba & 0xF000000) >> 24;
	}
	else {
		transferMode = TRANSFER_MODE_CHS;
		sectorNumber = (lba % 63) + 1;
		cylIndex = (lba + 1 - sectorNumber) / (16 * 63);
		lbaIO[0] = sectorNumber;
		lbaIO[1] = cylIndex & 0xFF;
		lbaIO[2] = (cylIndex >> 8) & 0xFF;
		lbaIO[3] = lbaIO[4] = lbaIO[5] = 0;
		headIndex = (UINT8)((lba + 1 - sectorNumber) % (16 * 63) / 63);
	}

	while (IDERead(chnl, ATA_OFF_STATUS) & ATA_STATUS_BUSY) {}

	UINT8 hardDrvSelVal = transferMode == TRANSFER_MODE_CHS ? 0xA0 : 0xE0;
	IDEWrite(chnl, ATA_OFF_HD_DEV_SEL, hardDrvSelVal | (dev->Type << 4) | headIndex);

	if (transferMode == TRANSFER_MODE_LBA48) {
		IDEWrite(chnl, ATA_OFF_SECTORS_CNT1, 0);
		IDEWrite(chnl, ATA_OFF_LBA3, lbaIO[3]);
		IDEWrite(chnl, ATA_OFF_LBA4, lbaIO[4]);
		IDEWrite(chnl, ATA_OFF_LBA5, lbaIO[5]);
	}
	
	IDEWrite(chnl, ATA_OFF_SECTORS_CNT0, count);
	IDEWrite(chnl, ATA_OFF_LBA0, lbaIO[0]);
	IDEWrite(chnl, ATA_OFF_LBA1, lbaIO[1]);
	IDEWrite(chnl, ATA_OFF_LBA2, lbaIO[2]);

	if (transferMode == TRANSFER_MODE_CHS || transferMode == TRANSFER_MODE_LBA28) cmd = ATA_COMMAND_READ_PIO;
	else if (transferMode == TRANSFER_MODE_LBA48) cmd = ATA_COMMAND_READ_PIO_EXT;
	else return IDE_TRANSFER_STATUS_UNKNOWN_MODE;	//	unknown mode

	IDEWrite(chnl, ATA_OFF_CMD, cmd);
	UINT16* buff = (UINT16*)destination;
	for (j = 0; j < count; j++) {
		for (i = 0; i < 256; i++) {
			errVal = IDEPoll(chnl, TRUE);
			if (errVal) return errVal;

			*buff = InWord(chnl->IOBase);
			++buff;
		}
	}

	return IDE_TRANSFER_STATUS_SUCCESS;
}

UINT8 IDEWriteSector(SIZE_T ctrlIndex, SIZE_T devIndex, VOID* source, UINT32 lba, UINT8 count) {
	if (ctrlIndex >= NumberOfCtrls || devIndex >= Ctrls[ctrlIndex].NumberOfDevices) return IDE_TRANSFER_STATUS_NO_DEVICE;

	IDE_DEVICE* dev = &Ctrls[ctrlIndex].Devices[devIndex];
	IDE_CHANNEL* chnl = &Ctrls[ctrlIndex].Channels[dev->ChannelType];

	UINT8 transferMode, cmd;
	UINT8 lbaIO[6], headIndex, sectorNumber, errVal;
	UINT16 cylIndex, i, j;

	IDEWrite(chnl, ATA_OFF_CTRL, chnl->IntDisabled ? ATA_CTRL_IRQ_DISABLED : 0);

	if ((lba & 0xFF000000)) {
		if (!(dev->Capabilities & 0x200)) return IDE_TRANSFER_STATUS_NO_LBA48;	//	lba 28/48 N/S
		transferMode = TRANSFER_MODE_LBA48;
		lbaIO[0] = lba & 0xFF;
		lbaIO[1] = (lba & 0xFF00) >> 8;
		lbaIO[2] = (lba & 0xFF0000) >> 16;
		lbaIO[3] = (lba & 0xFF000000) >> 24;
		lbaIO[4] = lbaIO[5] = headIndex = 0;
	}
	else if (dev->Capabilities & 0x200) {
		transferMode = TRANSFER_MODE_LBA28;
		lbaIO[0] = lba & 0xFF;
		lbaIO[1] = (lba & 0xFF00) >> 8;
		lbaIO[2] = (lba & 0xFF0000) >> 16;
		lbaIO[3] = lbaIO[4] = lbaIO[5] = 0;
		headIndex = (lba & 0xF000000) >> 24;
	}
	else {
		transferMode = TRANSFER_MODE_CHS;
		sectorNumber = (lba % 63) + 1;
		cylIndex = (lba + 1 - sectorNumber) / (16 * 63);
		lbaIO[0] = sectorNumber;
		lbaIO[1] = cylIndex & 0xFF;
		lbaIO[2] = (cylIndex >> 8) & 0xFF;
		lbaIO[3] = lbaIO[4] = lbaIO[5] = 0;
		headIndex = (UINT8)((lba + 1 - sectorNumber) % (16 * 63) / 63);
	}

	while (IDERead(chnl, ATA_OFF_STATUS) & ATA_STATUS_BUSY) {}

	UINT8 hardDrvSelVal = transferMode == TRANSFER_MODE_CHS ? 0xA0 : 0xE0;
	IDEWrite(chnl, ATA_OFF_HD_DEV_SEL, hardDrvSelVal | (dev->Type << 4) | headIndex);

	if (transferMode == TRANSFER_MODE_LBA48) {
		IDEWrite(chnl, ATA_OFF_SECTORS_CNT1, 0);
		IDEWrite(chnl, ATA_OFF_LBA3, lbaIO[3]);
		IDEWrite(chnl, ATA_OFF_LBA4, lbaIO[4]);
		IDEWrite(chnl, ATA_OFF_LBA5, lbaIO[5]);
	}
	
	IDEWrite(chnl, ATA_OFF_SECTORS_CNT0, count);
	IDEWrite(chnl, ATA_OFF_LBA0, lbaIO[0]);
	IDEWrite(chnl, ATA_OFF_LBA1, lbaIO[1]);
	IDEWrite(chnl, ATA_OFF_LBA2, lbaIO[2]);

	if (transferMode == TRANSFER_MODE_CHS || transferMode == TRANSFER_MODE_LBA28) cmd = ATA_COMMAND_WRITE_PIO;
	else if (transferMode == TRANSFER_MODE_LBA48) cmd = ATA_COMMAND_WRITE_PIO_EXT;
	else return IDE_TRANSFER_STATUS_UNKNOWN_MODE;	//	unknown mode

	IDEWrite(chnl, ATA_OFF_CMD, cmd);
	UINT16* buff = (UINT16*)source;
	for (j = 0; j < count; j++) {
		for (i = 0; i < 256; i++) {
			errVal = IDEPoll(chnl, FALSE);
			if (errVal) return errVal;

			OutWord(chnl->IOBase, *buff);
			++buff;
		}
	}

	IDEPoll(chnl, FALSE);
	return IDE_TRANSFER_STATUS_SUCCESS;
}

/*
	Warning: disables the IRQ of the selected channels (bit#1 (0x02) - nIEN(no interrupt enabled))
	BAR0 - Primary channel base address						(0x1F0/???)
	BAR1 - Primary channel control port base address		(0x3F6/???)
	BAR2 - Secondary channel base address					(0x170/???)
	BAR3 - Secondary channel control port base address		(0x176/???)
	BAR4 - Bus Master IDE									(???)

	Input:
		devices - array of 4 elements
	Output:
		devices - initialized devices
		Number of initialized devices
*/
IDE_CONTROLLER IDEInitCtrl(SIZE_T bar0, SIZE_T bar1, SIZE_T bar2, SIZE_T bar3, SIZE_T bar4) {
	IDE_CONTROLLER result = {};
	result.Channels = (IDE_CHANNEL*)AllocatePhysicalMemory(IDE_CHANNELS_PER_CTRL * sizeof(IDE_CHANNEL));
	if (!result.Channels) return result;
	IDEInitChnls(bar0, bar1, bar2, bar3, bar4, &result.Channels[IDE_CHNL_PRIMARY], &result.Channels[IDE_CHNL_SECONDARY]);
	result.NumberOfChannels = IDE_CHANNELS_PER_CTRL;
	
	SIZE_T devsCnt = 0;
	SIZE_T maxDevsCnt = IDE_CHANNELS_PER_CTRL * IDE_DEVICES_PER_CHNL;
	IDE_DEVICE devices[maxDevsCnt];
	
	IDEInitDevs(&result.Channels[IDE_CHNL_PRIMARY], &devices[IDE_DEV_MASTER], &devices[IDE_DEV_SLAVE]);
	for (SIZE_T i = 0; i < IDE_DEVICES_PER_CHNL; i++) if (devices[i].Type != IDE_DEV_NO_DRIVE) devices[devsCnt++] = devices[i];
	SIZE_T prevDevsCnt = devsCnt, maxIndex = devsCnt + IDE_DEVICES_PER_CHNL;

	IDEInitDevs(&result.Channels[IDE_CHNL_SECONDARY], &devices[devsCnt + IDE_DEV_MASTER], &devices[devsCnt + IDE_DEV_SLAVE]);
	for (SIZE_T i = prevDevsCnt; i < maxIndex; i++) if (devices[i].Type != IDE_DEV_NO_DRIVE) devices[devsCnt++] = devices[i];

	SIZE_T devsSize = devsCnt * sizeof(IDE_DEVICE);
	result.Devices = (IDE_DEVICE*)AllocatePhysicalMemory(devsSize);
	if (!result.Devices) {
		FreePhysicalMemory(result.Channels);
		result.NumberOfChannels = 0;
		return result;
	}

	CopyMemory(result.Devices, &devices[0], devsSize);
	result.NumberOfDevices = devsCnt;
	return result;
}

/*
	BAR0 - Primary channel base address						(0x1F0/???)
	BAR1 - Primary channel control port base address		(0x3F6/???)
	BAR2 - Secondary channel base address					(0x170/???)
	BAR3 - Secondary channel control port base address		(0x176/???)
	BAR4 - Bus Master IDE									(???)
*/
VOID IDEInitChnls(SIZE_T bar0, SIZE_T bar1, SIZE_T bar2, SIZE_T bar3, SIZE_T bar4, IDE_CHANNEL* primaryChnl, IDE_CHANNEL* secondaryChnl) {
	IDEInitChnl(IDE_CHNL_PRIMARY, bar0, bar1, bar4, primaryChnl);
	IDEInitChnl(IDE_CHNL_SECONDARY, bar2, bar3, bar4, secondaryChnl);
}

VOID IDEInitChnl(SIZE_T chnlType, SIZE_T ioBaseAddr, SIZE_T ctrlBaseAddr, SIZE_T BMIDEBaseAddr, IDE_CHANNEL* chnl) {
	chnl->Type = chnlType & 1;
	if (ioBaseAddr) chnl->IOBase = ioBaseAddr & 0xFFFFFFFC;
	else chnl->IOBase = chnl->Type == IDE_CHNL_PRIMARY ? IDE_COMP_PRIMARY_CHNL_IO_BASE : IDE_COMP_SECONDARY_CHNL_IO_BASE;
	
	if (ctrlBaseAddr) chnl->CtrlBase = ioBaseAddr & 0xFFFFFFFC;
	else chnl->CtrlBase = chnl->Type == IDE_CHNL_PRIMARY ? IDE_COMP_PRIMARY_CHNL_CTRL_BASE : IDE_COMP_SECONDARY_CHNL_CTRL_BASE;

	if (BMIDEBaseAddr) chnl->BMIDEBase = (BMIDEBaseAddr & 0xFFFFFFFC) + (chnl->Type == IDE_CHNL_PRIMARY ? 0 : 8);
}

/*
	Warning: disables the IRQ of the selected channel (bit#1 (0x02) - nIEN(no interrupt enabled))
	dev->Type == IDE_DEV_NO_DRIVE means no drive
*/
VOID IDEInitDevs(const IDE_CHANNEL* chnl, IDE_DEVICE* devMaster, IDE_DEVICE* devSlave) {
	if (!IDEInitDev(chnl, IDE_DEV_MASTER, devMaster)) devMaster->Type = IDE_DEV_NO_DRIVE;
	if (!IDEInitDev(chnl, IDE_DEV_SLAVE, devSlave)) devSlave->Type = IDE_DEV_NO_DRIVE;
}

/*
	Warning: disables the IRQ of the selected channel (bit#1 (0x02) - nIEN(no interrupt enabled))
*/
BOOL IDEInitDev(const IDE_CHANNEL* chnl, SIZE_T devIndex, IDE_DEVICE* dev) {
	IDEWrite(chnl, ATA_OFF_CTRL, ATA_CTRL_IRQ_DISABLED);
	UINT8 errVal = 0, devType = IDE_DEV_ATA, status, cLow, cHigh;
	devIndex &= 1;

	IDEWrite(chnl, ATA_OFF_HD_DEV_SEL, (UINT8)(0xA0 | (devIndex << 4)));
	Sleep(1);

	IDEWrite(chnl, ATA_OFF_CMD, ATA_COMMAND_IDENTIFY);
	Sleep(1);

	if (IDERead(chnl, ATA_OFF_STATUS) == 0) return FALSE;				// no device

	while (TRUE) {
		status = IDERead(chnl, ATA_OFF_STATUS);
		if (status & ATA_STATUS_ERROR) {								//	devType != ATA
			errVal = 1;
			break;
		}

		if (!(status & ATA_STATUS_BUSY) && (status & ATA_STATUS_DATA_REQ_READY)) break;
	}

	if (errVal) {
		cLow = IDERead(chnl, ATA_OFF_LBA1);
		cHigh = IDERead(chnl, ATA_OFF_LBA2);

		if ((cLow == 0x14 && cHigh == 0xEB) || (cLow == 0x69 && cHigh == 0x96)) devType = IDE_DEV_ATAPI;
		else return FALSE;												//	unknown device type

		IDEWrite(chnl, ATA_OFF_CMD, ATA_COMMAND_IDENTIFY_PACKET);
		Sleep(1);														
	}
	
	IDEReadBuffer(chnl, TmpBuffer, ATA_OFF_DATA, 128);

	dev->Channel = (IDE_CHANNEL*)chnl;
	dev->ChannelType = chnl->Type;
	dev->Index = devIndex;
	dev->Type = devType;
	dev->Signature = *(UINT16*)&TmpBuffer[ATA_IDENT_DEV_TYPE];
	dev->Capabilities = *(UINT16*)&TmpBuffer[ATA_IDENT_CAPABILITIES];
	dev->CmdSets = *(UINT32*)&TmpBuffer[ATA_IDENT_CMD_SETS];
	dev->LengthInSectors = *(UINT32*)&TmpBuffer[dev->CmdSets & 0x4000000 ? ATA_IDENT_MAX_LBA_EXT : ATA_IDENT_MAX_LBA];
	
	for (UINT8 k = 0; k < IDE_DEV_MODEL_LENGTH; k += 2) {
		dev->Model[k] = TmpBuffer[ATA_IDENT_MODEL + k + 1];
		dev->Model[k + 1] = TmpBuffer[ATA_IDENT_MODEL + k];
	}

	dev->Model[IDE_DEV_MODEL_LENGTH] = 0;
	return TRUE;
}

UINT8 IDERead(const IDE_CHANNEL* chnl, UINT8 offset) {
	UINT8 result;
	if (offset > 0x07 && offset < 0x0C) IDEWrite(chnl, ATA_OFF_CTRL, 0x80 | (UINT8)chnl->IntDisabled);
	if (offset < 0x08) result = InByte(chnl->IOBase + offset);
	else if (offset < 0x0C) result = InByte(chnl->IOBase  + offset - 0x06);
	else if (offset < 0x0E) result = InByte(chnl->CtrlBase  + offset - 0x0A);
	else if (offset < 0x16) result = InByte(chnl->BMIDEBase + offset - 0x0E);
	if (offset > 0x07 && offset < 0x0C) IDEWrite(chnl, ATA_OFF_CTRL, (UINT8)chnl->IntDisabled);
	return result;
}

VOID IDEWrite(const IDE_CHANNEL* chnl, UINT8 offset, UINT8 value) {
	if (offset > 0x07 && offset < 0x0C) IDEWrite(chnl, ATA_OFF_CTRL, 0x80 | (UINT8)chnl->IntDisabled);
	if (offset < 0x08) OutByte(chnl->IOBase  + offset, value);
	else if (offset < 0x0C) OutByte(chnl->IOBase  + offset - 0x06, value);
	else if (offset < 0x0E) OutByte(chnl->CtrlBase  + offset - 0x0A, value);
	else if (offset < 0x16) OutByte(chnl->BMIDEBase + offset - 0x0E, value);
	if (offset > 0x07 && offset < 0x0C) IDEWrite(chnl, ATA_OFF_CTRL, (UINT8)chnl->IntDisabled);
}

VOID IDEReadBuffer(const IDE_CHANNEL* chnl, VOID* buffer, UINT8 offset, SIZE_T quadsCount) {
	if (offset > 0x07 && offset < 0x0C) IDEWrite(chnl, ATA_OFF_CTRL, 0x80 | (UINT8)chnl->IntDisabled);

	SIZE_T portIndex = offset;
	if (offset < 0x08) portIndex += chnl->IOBase;
	else if (offset < 0x0C) portIndex += chnl->IOBase - 0x06;
	else if (offset < 0x0E) portIndex += chnl->IOBase - 0x0A;
	else if (offset < 0x16) portIndex += chnl->BMIDEBase - 0x0E;
	else return;

	UINT32* buff32 = (UINT32*)buffer;
	for (SIZE_T i = 0; i < quadsCount; i++) buff32[i] = InLong(portIndex);

	if (offset > 0x07 && offset < 0x0C) IDEWrite(chnl, ATA_OFF_CTRL, (UINT8)chnl->IntDisabled);
}

/*
	Returns IDE_POLL_STATUS_*
*/
UINT8 IDEPoll(const IDE_CHANNEL* chnl, BOOL advencedCheck) {
	for (SIZE_T i = 0; i < 4; i++) IDERead(chnl, ATA_OFF_ALT_STATUS);
	while (IDERead(chnl, ATA_OFF_STATUS) & ATA_STATUS_BUSY) {}
	if (advencedCheck) {
		UINT8 status = IDERead(chnl, ATA_OFF_STATUS);
		if (status & ATA_STATUS_ERROR) return IDE_POLL_STATUS_ERROR;
		else if (status & ATA_STATUS_DRIVE_WRITE_FAULT) return IDE_POLL_STATUS_DRIVE_WRITE_FAULT;
		else if (!(status & ATA_STATUS_DATA_REQ_READY)) return IDE_POLL_STATUS_DATA_REQ_NOT_READY;
	}

	return IDE_POLL_STATUS_SUCCESS;
}