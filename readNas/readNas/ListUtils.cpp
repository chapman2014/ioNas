#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include "ListUtils.h"

int List_Nbr(List_T * liste)
{
	return (liste) ? liste->n : 0;
}

void List_Read(List_T * liste, int index, void *data)
{
	if((index < 0) || (index >= liste->n)){
		printf("Wrong list index (read)");
		index = 0;
	}
	memcpy(data, &liste->array[index * liste->size], liste->size);
}