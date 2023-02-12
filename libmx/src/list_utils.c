#include "../inc/libmx.h"

void * valueOf(t_list * list, int i){
	int j = 0;
	for (t_list*temp = list; temp; temp = temp->next){
		if (j++ == i) return temp->data;
	}
	return NULL;
}

int indexOf(t_list * list, void * value){
	int i = 0;
	for (t_list * temp = list; temp; temp = temp->next, i++)
	{
		if (!mx_strcmp(value, temp->data)) return i;
	}
	return -1;
}

void mx_clear_list(t_list **list, void(*free_content)(void * ptr)){
	if(!list) return;
	t_list * temp = *list;
	t_list * copy = temp;
	while (temp){
		copy = temp->next;
		if (free_content){
			free_content(temp->data);
		}
		free(temp);
		temp = copy;
	}
	*list = temp;
}



