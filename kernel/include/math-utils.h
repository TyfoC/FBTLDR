#pragma once
#ifndef MATH_UTILS
#define MATH_UTILS

#include "typedefs.h"

PTRDIFF_T IntAlignUp(PTRDIFF_T value, SIZE_T alignment);
SIZE_T UintAlignUp(SIZE_T value, SIZE_T alignment);

#endif