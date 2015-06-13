#ifndef __WORKITEM_H__
#define __WORKITEM_H__

#include <unistd.h>
#include <cstring>

#define DATA_LEN	512  /*MUST be multiple of sizeof(int)*/
#define DATA_SEG_COUNT		DATA_LEN/sizeof(int)
#define CHECKSUM_PATTERN	0x0000AF76

class WorkItem
{
private:
	char	m_data[DATA_LEN];
	unsigned short	m_checksum;
	char	m_name[30];

	unsigned short generateChecksum() {
		unsigned int	result = 0;
		unsigned int *dataptr = (unsigned int*)&m_data[0];
		for(int i = 0; i < DATA_SEG_COUNT; ++i) {
			result ^= *dataptr;
			dataptr++;
		}

		result ^= CHECKSUM_PATTERN;
		if(result & 0x80000000u) {
			result <<= 1;
			result |= 1;
		}
		else {
			result <<= 1;
		}

		return (0xFFFFu & ((result >> 16) ^ result));
	}

public:
	WorkItem(const char* name, const char* data = NULL, int dataLen = 0)
	{
		strncpy(this->m_name, name, 30);
		memset(m_data, 0, sizeof(m_data));
		if(data)
			memcpy(this->m_data, data, dataLen);
		m_checksum = generateChecksum();
	}

	bool validate() {
		unsigned short targetChecksum = generateChecksum();
		return targetChecksum == this->m_checksum;
	}
};

#endif
