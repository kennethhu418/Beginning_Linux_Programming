#ifndef __SHARED_MEM_H__
#define __SHARED_MEM_H__

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Message queue name must start with '/'
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define SHARED_MEM_NAME_PATTERN   "/PC_MESSAGE_Q_NAME_%d"
#define QUEUE_SIZE       10 

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
