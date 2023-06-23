#include <terminal.h>

static const char HorizontalTabString[] = "    ";
static const char VerticalTabString[] = "\n\n\n\n";
static char ConvBuffer[INT_TO_STR_BUFF_LENGTH];

void PutString(const char* str) {
	char* ptr = (char*)&str[0];
	while (*ptr) {
		PutChar(*ptr);
		ptr++;
	}
}

void PrintFormatted(const char* fmt, ...) {
	size_t* ptrArg = (size_t*)((size_t)&fmt + sizeof(fmt));
	for (size_t i = 0; fmt[i]; i++) {
		if (fmt[i] == '%') {
			++i;
			if (fmt[i] == 'c') PutChar((char)*ptrArg++);
			else if (fmt[i] == 's') PutString((char*)*ptrArg++);
			else if (fmt[i] =='d') {
				IntToString((ptrdiff_t)*ptrArg++, 10, ConvBuffer);
				PutString(ConvBuffer);
			}
			else if (fmt[i] =='u') {
				UintToString((size_t)*ptrArg++, 10, ConvBuffer);
				PutString(ConvBuffer);
			}
			else if (fmt[i] =='x') {
				UintToString((size_t)*ptrArg++, 16, ConvBuffer);
				PutString(ConvBuffer);
			}
			else PutChar('%');
		}
		else if (fmt[i] == '\t') PutString(HorizontalTabString);
		else if (fmt[i] == '\v') PutString(VerticalTabString);
		else PutChar(fmt[i]);
	}
}