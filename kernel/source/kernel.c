extern void KernelMain(unsigned dataAddress) {
	char* buff = (char*)0xb8000;
	unsigned off = 160 * 16;
	buff[off] = 'P';
	buff[off + 2] = 'r';
	buff[off + 4] = 'e';
	buff[off + 6] = 's';
	buff[off + 8] = 's';
	buff[off + 10] = ' ';
	buff[off + 12] = 'F';
	__asm__ __volatile__("jmp ."::"a"(dataAddress));
	while (1);
}