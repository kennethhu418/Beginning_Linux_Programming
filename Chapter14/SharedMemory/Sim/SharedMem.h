#ifndef __SHARED_MEM_H__
#define __SHARED_MEM_H__

#define SHARED_MEM_NAME_PATTERN   "PC_SHM_NAME_%d"
#define SHARED_MEM_SIZE	  32768 
#define QUEUE_SIZE        5

static void itoa(unsigned int n, char* str, unsigned int base) {
	if(n == 0) {
		*str = '0';
		*(str + 1) = '\0';
		return;
	}

	char* orgStr = str;
	unsigned int r = 0;
	while(n) {
		r = n % base;
		n /= base;
		*(str++) = r < 10 ? (r + '0') : (r - 10 + 'A');
	}
	*(str--) = '\0';
	char a;
	while(orgStr < str) {
		a = *orgStr;
		*orgStr = *str;
		*str = a;
		orgStr++; str--;
	}
}

#endif
