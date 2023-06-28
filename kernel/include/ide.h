#pragma once
#ifndef IDE_CTRL
#define IDE_CTRL

#include "pit.h"
#include "pci.h"

#define ATA_STATUS_ERROR					0x01
#define ATA_STATUS_INDEX					0x02
#define ATA_STATUS_CORRECT_DATA				0x04
#define ATA_STATUS_DATA_REQ_READY			0x08
#define ATA_STATUS_DRIVE_SEEK_DONE			0x10
#define ATA_STATUS_DRIVE_WRITE_FAULT		0x20
#define ATA_STATUS_DRIVE_READY				0x40
#define ATA_STATUS_BUSY						0x80

#define ATA_ERROR_NO_ADDR_MARK				0x01
#define ATA_ERROR_TRACK0_NOT_FOUND			0x02
#define ATA_ERROR_CMD_ABORTED				0x04
#define ATA_ERROR_MEDIA_CHANGE_REQ			0x08
#define ATA_ERROR_ID_MARK_NOT_FOUND			0x10
#define ATA_ERROR_MEDIA_CHANGED				0x20
#define ATA_ERROR_UNCORR_DATA				0x40
#define ATA_ERROR_BAD_BLOCK					0x80

#define ATA_COMMAND_READ_PIO				0x20
#define ATA_COMMAND_READ_PIO_EXT			0x24
#define ATA_COMMAND_READ_DMA				0xC8
#define ATA_COMMAND_READ_DMA_EXT			0x25
#define ATA_COMMAND_WRITE_PIO				0x30
#define ATA_COMMAND_WRITE_PIO_EXT			0x34
#define ATA_COMMAND_WRITE_DMA				0xCA
#define ATA_COMMAND_WRITE_DMA_EXT			0x35
#define ATA_COMMAND_CACHE_FLUSH				0xE7
#define ATA_COMMAND_CACHE_FLUSH_EXT			0xEA
#define ATA_COMMAND_PACKET					0xA0
#define ATA_COMMAND_IDENTIFY_PACKET			0xA1
#define ATA_COMMAND_IDENTIFY				0xEC
#define ATAPI_COMMAND_READ					0xA8	//	ATAPI
#define ATAPI_COMMAND_EJECT					0x1B	//	ATAPI

#define ATA_CTRL_IRQ_DISABLED				0x02	//	nIEN (no interrupt enabled)

#define ATA_IDENT_DEV_TYPE					0x00
#define ATA_IDENT_CYLINDERS					0x02
#define ATA_IDENT_HEADS						0x06
#define ATA_IDENT_SECTORS					0x0C
#define ATA_IDENT_SERIAL					0x14
#define ATA_IDENT_MODEL						0x36
#define ATA_IDENT_CAPABILITIES				0x62
#define ATA_IDENT_FLD_VALID					0x6A
#define ATA_IDENT_MAX_LBA					0x78
#define ATA_IDENT_CMD_SETS					0xA4
#define ATA_IDENT_MAX_LBA_EXT				0xC8

#define IDE_CHNL_PRIMARY					0x00
#define IDE_CHNL_SECONDARY					0x01
#define IDE_CHNL_NO_CHNL					0xFF
#define IDE_DEV_MASTER						0x00
#define IDE_DEV_SLAVE						0x01
#define IDE_DEV_NO_DRIVE					0xFF
#define IDE_DEV_ATA							0x00
#define IDE_DEV_ATAPI						0x01

#define ATA_OFF_DATA						0x00	//	RW
#define ATA_OFF_ERROR						0x01	//	RO
#define ATA_OFF_FEATURES					0x01	//	WO
#define ATA_OFF_SECTORS_CNT0				0x02	//	RW
#define ATA_OFF_LBA0						0x03	//	RW
#define ATA_OFF_LBA1						0x04	//	RW
#define ATA_OFF_LBA2						0x05	//	RW
#define ATA_OFF_HD_DEV_SEL					0x06	//	RW
#define ATA_OFF_CMD							0x07	//	WO
#define ATA_OFF_STATUS						0x07	//	RW
#define ATA_OFF_SECTORS_CNT1				0x08
#define ATA_OFF_LBA3						0x09
#define ATA_OFF_LBA4						0x0A
#define ATA_OFF_LBA5						0x0B
#define ATA_OFF_CTRL						0x0C	//	WO
#define ATA_OFF_ALT_STATUS					0x0C	//	RO	(prim ? bar1 : bar3) + 2
#define ATA_OFF_DEV_ADDR					0x0D

#define IDE_COMP_PRIMARY_CHNL_IO_BASE		0x1F0
#define IDE_COMP_PRIMARY_CHNL_CTRL_BASE		0x3F6
#define IDE_COMP_SECONDARY_CHNL_IO_BASE		0x170
#define IDE_COMP_SECONDARY_CHNL_CTRL_BASE	0x176

#define IDE_CHANNELS_PER_CTRL				2
#define IDE_DEVICES_PER_CHNL				2

#define IDE_DEV_MODEL_LENGTH				40
#define IDE_DEV_MODEL_SIZE					41

#define IDE_POLL_STATUS_SUCCESS				0
#define IDE_POLL_STATUS_ERROR				1
#define IDE_POLL_STATUS_DRIVE_WRITE_FAULT	2
#define IDE_POLL_STATUS_DATA_REQ_NOT_READY	3

#define IDE_TRANSFER_STATUS_SUCCESS			0
#define IDE_TRANSFER_STATUS_NO_DEVICE		0xFD
#define IDE_TRANSFER_STATUS_NO_LBA48		0xFE
#define IDE_TRANSFER_STATUS_UNKNOWN_MODE	0xFF

#define TRANSFER_MODE_CHS					0
#define TRANSFER_MODE_LBA28					1
#define TRANSFER_MODE_LBA48					2

#define IDE_MAX_SECTORS_PER_OPERATION		0xFF

#define IDE_PRIMARY_IRQ						0x0E
#define IDE_SECONDARY_IRQ					0x0F

typedef struct {
	UINT16	Type;					//	IDE_CHNL_TYPE_*
	UINT16	IOBase;
	UINT16	CtrlBase;
	UINT16	BMIDEBase;
	UINT16	IntDisabled;
} IDE_CHANNEL;

typedef struct {
	IDE_CHANNEL*	Channel;
	UINT8			ChannelType;	//	IDE_CHNL_TYPE_PRIMARY/IDE_CHNL_TYPE_SECONDARY
	UINT8			Index;			//	IDE_DEV_MASTER/IDE_DEV_SLAVE or IDE_DEV_NO_DRIVE
	UINT8			Type;			//	IDE_DEV_ATA/IDE_DEV_ATAPI
	UINT16			Signature;
	UINT16			Capabilities;
	UINT32			CmdSets;
	UINT32			LengthInSectors;
	CHAR			Model[IDE_DEV_MODEL_SIZE];
} IDE_DEVICE;

typedef struct {
	IDE_CHANNEL*	Channels;
	IDE_DEVICE*		Devices;
	SIZE_T			NumberOfDevices;
	SIZE_T			NumberOfChannels;
} IDE_CONTROLLER;

typedef struct ATTRIB(__packed__) {
	UINT8	Operation;
	UINT8	Reserved0;
	UINT32	LBA;
	UINT32	Length;
	UINT8	Reserved1;
	UINT8	CtrlCode;
} ATAPI_PACKET;

BOOL InstallIDE(VOID);
BOOL IDEGetCtrl(SIZE_T index, IDE_CONTROLLER* ctrl);
BOOL IDEGetChnl(SIZE_T ctrlIndex, SIZE_T chnlType, IDE_CHANNEL* chnl);
BOOL IDEGetDev(SIZE_T ctrlIndex, SIZE_T devIndex, IDE_DEVICE* dev);
BOOL IDEGetChnlDev(SIZE_T ctrlIndex, SIZE_T chnlType, SIZE_T devType, IDE_DEVICE* dev);
BOOL IDEWaitIRQ(SIZE_T ctrlIndex, SIZE_T devIndex);
BOOL IDEIRQInvoked(SIZE_T ctrlIndex, SIZE_T devIndex);
BOOL IDEClearIRQ(SIZE_T ctrlIndex, SIZE_T devIndex);
SIZE_T IDEGetCtrlsCount(VOID);
SIZE_T IDEGetChnlsCount(SIZE_T ctrlIndex);
SIZE_T IDEGetDevsCount(SIZE_T ctrlIndex);

UINT8 IDEReadSector(SIZE_T ctrlIndex, SIZE_T devIndex, VOID* destination, UINT32 lba, UINT8 count);
UINT8 IDEWriteSector(SIZE_T ctrlIndex, SIZE_T devIndex, VOID* source, UINT32 lba, UINT8 count);

IDE_CONTROLLER IDEInitCtrl(SIZE_T bar0, SIZE_T bar1, SIZE_T bar2, SIZE_T bar3, SIZE_T bar4);
VOID IDEInitChnls(SIZE_T bar0, SIZE_T bar1, SIZE_T bar2, SIZE_T bar3, SIZE_T bar4, IDE_CHANNEL* primaryChnl, IDE_CHANNEL* secondaryChnl);
VOID IDEInitChnl(SIZE_T chnlType, SIZE_T ioBaseAddr, SIZE_T ctrlBaseAddr, SIZE_T BMIDEBaseAddr, IDE_CHANNEL* chnl);
VOID IDEInitDevs(const IDE_CHANNEL* chnl, IDE_DEVICE* devMaster, IDE_DEVICE* devSlave);
BOOL IDEInitDev(const IDE_CHANNEL* chnl, SIZE_T devIndex, IDE_DEVICE* dev);

UINT8 IDERead(const IDE_CHANNEL* chnl, UINT8 offset);
VOID IDEWrite(const IDE_CHANNEL* chnl, UINT8 offset, UINT8 value);
VOID IDEReadBuffer(const IDE_CHANNEL* chnl, VOID* buffer, UINT8 offset, SIZE_T quadsCount);
UINT8 IDEPoll(const IDE_CHANNEL* chnl, BOOL advencedCheck);

#endif