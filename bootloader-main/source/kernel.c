#include <kernel.h>

GdtRegister KernelGdtRegister = {};
GdtEntry KernelGdt[KERNEL_GDT_ENTRIES_COUNT] = {};