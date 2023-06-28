#pragma once
#ifndef USER_CONFIG
#define USER_CONFIG

#include "typedefs.h"

#define USER_CFG_VIDEO_MODE_TYPE_BIOS			0x00
#define USER_CFG_VIDEO_MODE_TYPE_SVGA_TEXT		0x01
#define USER_CFG_VIDEO_MODE_TYPE_SVGA_GRAPHICS	0x02

#define USER_CFG_KERNEL_FILE_TYPE_ELF			0x00
#define USER_CFG_KERNEL_FILE_TYPE_EXE			0x01

typedef struct ATTRIB(__packed__) {
	uint16_t	VideoModeType;				//	USER_CFG_VIDEO_MODE_TYPE_*
	uint16_t	VideoModeIndex;				//	BIOS video mode index (VideoModeType == USER_CFG_VIDEO_MODE_TYPE_BIOS)
	uint16_t	VideoModeWidth;				//	(VideoModeType != USER_CFG_VIDEO_MODE_TYPE_BIOS)
	uint16_t	VideoModeHeight;			//	(VideoModeType != USER_CFG_VIDEO_MODE_TYPE_BIOS)
	uint16_t	VideoModeBitsPerPixel;		//	(VideoModeType != USER_CFG_VIDEO_MODE_TYPE_BIOS)
	uint16_t	VideoModeFuzzy;				//	If 1, searches for a similar video mode by parameters (VideoModeType != USER_CFG_VIDEO_MODE_TYPE_BIOS)
	uint16_t	KernelFileType;				//	USER_CFG_KERNEL_FILE_TYPE_*
	uint8_t		Reserved[498];
} UserConfig;

#endif