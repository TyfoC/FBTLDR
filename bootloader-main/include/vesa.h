#pragma once
#ifndef VESA
#define VESA

#include "typedefs.h"

#define VESA_SIGNATURE				0x32454256//	0x41534556

#define VESA_ATTRIBUTE_GRAPHICS		0x10
#define VESA_ATTRIBUTE_LFB			0x80

typedef struct ATTRIB(__packed__) {
	uint32_t	Signature;
	uint16_t	Version;
	uint32_t	OemNamePtr;
	uint32_t	Capabilities;
	uint32_t	VideoModePtr;
	uint16_t	TotalMemory;
	uint16_t	OemSoftwareRevision;
	uint32_t	OemVendorNamePtr;
	uint32_t	OemPoductNamePtr;
	uint32_t	OemProductRevisionPtr;
	uint8_t		Reserved[222];
	uint8_t		OemData[256];
} VesaInfoBlock;

typedef struct ATTRIB(__packed__) {
	uint16_t	Attributes;
	uint8_t		FirstWindow;
	uint8_t		SecondWindow;
	uint16_t	Granularity;
	uint16_t	WindowSize;
	uint16_t	FirstSegment;
	uint16_t	SecondSegment;
	uint32_t	WindowFuncPtr;
	uint16_t	BytesPerScanLine;
	uint16_t	Width;
	uint16_t	Height;
	uint8_t		CharWidth;
	uint8_t		CharHeight;
	uint8_t		PlanesCount;
	uint8_t		BitsPerPixel;
	uint8_t		BanksCount;
	uint8_t		MemoryModel;
	uint8_t		BankSize;
	uint8_t		ImagePagesCount;
	uint8_t		Reserved0;
	uint8_t		RedMaskSize;
	uint8_t		RedFieldPosition;
	uint8_t		GreenMaskSize;
	uint8_t		GreenFieldPosition;
	uint8_t		BlueMaskSize;
	uint8_t		BlueFieldPosition;
	uint8_t		ReservedMaskSize;
	uint8_t		ReservedFieldPosition;
	uint8_t		DirectColorModeInfo;
	uint32_t	LFBAddress;
	uint32_t	OffscreenMemoryOffset;
	uint16_t	OffscreenMemorySize;
	uint8_t		Reserved1[206];
} VesaModeInfo;

bool_t GetVesaInfoBlock(VesaInfoBlock* infoBlock);
bool_t GetVesaModeInfo(uint16_t modeNumber, VesaModeInfo* modeInfo);
bool_t SetVesaMode(uint16_t modeNumber);

#endif