#ifndef __WORKITEM__H_
#define __WORKITEM__H_

#define MAX_WORK_ITEM_LEN 512
#define ITEM_STARTING_ID_EACH_PRODUCER  300000

#include <cstring>

typedef struct __WorkItem
{
	unsigned int	id;
	char	data[MAX_WORK_ITEM_LEN];

	__WorkItem& operator=(const __WorkItem& another) {
		this->id = another.id;
		memcpy(this->data, another.data, MAX_WORK_ITEM_LEN);
		return *this;
	}
}WorkItem;

#endif
