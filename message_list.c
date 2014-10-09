#include "net_include.h"
#include <stdio.h>

void copy_msg(message *mp1, message *mp2)
{
	mp1->hid = mp2->hid;
	mp1->seq = mp2->seq;
	mp1->cont = mp2->cont;
	strncpy(mp1->payload, mp2->payload, PAYLOAD_SIZE);
}

void message_gen(message *mp, int hid, int *seq)
{
	mp->hid = hid;
	mp->seq = *seq;
	mp->cont = rand(); /*random number needed*/
	(*seq)++;
}

void list_init(message_list *msg_list)
{
	int i;	
	msg_list->start = WINDOW_SIZE-1;
	msg_list->count = 0;
	
	msg_list->list[WINDOW_SIZE-1].seq = -1;

	for(i = 0; i < WINDOW_SIZE; i++){
		msg_list->valid[i] = false;
	}
}

bool list_get(message_list *msg_list, int seq, message *msg){
	int start_index, start_seq, diff, index;
	message *mp;
	start_index = msg_list->start;
	start_seq = (msg_list->list)[start_index].seq;
	diff = seq - start_seq;
	if(diff < 0 || diff >= WINDOW_SIZE)
		return false;
	else
	{
		index = start_index + diff;
		if(index >= WINDOW_SIZE)
			index -= WINDOW_SIZE;
		mp = &msg_list->list[index];
		if(mp->seq != seq)
		{
			printf("message list_get error\n");
			exit(0);
		}
		copy_msg(msg, mp);
		return true;
	}
}

int list_write(message_list *msg_list, FILE *fw)
{
	int start_index, i, wbyte;
	start_index = msg_list->start+1;

	for(i = 0; i < WINDOW_SIZE; i++, start_index++){
		if(start_index == WINDOW_SIZE)
			start_index = 0;
		if(msg_list->valid[start_index])
		{
			wbyte = fprintf(fw, "%d", msg_list->list[start_index].cont);
			(msg_list->count)--;
			
			printf("write number :%d with sequence: %d\n", msg_list->list[start_index].cont, msg_list->list[start_index].seq);
			
			msg_list->valid[start_index] = false;
			msg_list->list[start_index].cont = -1;
		}
		else
		{
			start_index--;
			if(start_index == -1)
				start_index = WINDOW_SIZE-1;
			msg_list->start = start_index;
			break;
		}
	}
	return 0;
}


bool list_add(message_list *msg_list, message *msg)
{
	int start_index, start_seq, index, diff;
	message *mp;
	start_index = msg_list->start;
	start_seq = (msg_list->list)[start_index].seq;
	diff = msg->seq - start_seq;
	if(diff < 0 || diff >= WINDOW_SIZE)
	{
		printf("list_add received wrong message\n");
		exit(0);
	}
	if(msg_list->count == WINDOW_SIZE)
	{
		printf("list_add error \n");
		exit(0);
	}
	index = start_index + diff;
	if(index >= WINDOW_SIZE)
		index -= WINDOW_SIZE; 

	if(!msg_list->valid[index])
	{
		msg_list->valid[index] = true;
		mp = &msg_list->list[index];
		copy_msg(mp, msg);
		(msg_list->count)++;
		return true;
	}
	else
		return false;
}


void list_display(message_list *msg_list, bool all)
{
	int start_index, i;
	message *mp;
	
	start_index = msg_list->start;
	printf("----------->display, start_index is %d\n", start_index);
	for(i = 0; i <= WINDOW_SIZE; i++, start_index++){
		if(start_index >= WINDOW_SIZE)
			start_index -= WINDOW_SIZE;
		mp = &msg_list->list[start_index];

		if(all || msg_list->valid[start_index])
		{
			printf("%d: %d ", start_index, mp->cont);
		}
		else
		{
			printf("%d: null ", start_index);
		}
		
		printf("\n");
	}
	
}

void list_receive(message_list *msg_list, message *msg, FILE *fw)
{
	if(list_add(msg_list, msg)){
		/*list_display(msg_list, true);*/
		list_write(msg_list, fw);
	}
	list_display(msg_list, false);

}
