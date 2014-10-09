#include "message_list.h"
#include <stdio.h>

int main()
{
	int i, seq;
	message_list *msg_list;
	FILE *fw;
	message *msg;
	seq = 0;
	
	msg = malloc(MAX_MESS_LEN);
	if((fw = fopen("test.out", "w")) == NULL){
                        perror("fopen");
                        exit(0);
                    }
	srand(time(NULL));
	msg_list = malloc(8 + MAX_MESS_LEN * WINDOW_SIZE + WINDOW_SIZE * sizeof(bool));
	list_init(msg_list);

	for(i = 0; i < 6; i++)
	{
		message_gen(msg, 0, &seq);
		list_receive(msg_list, msg, fw);
	}
	list_display(msg_list, true);
	free(msg);
	free(msg_list);
	fclose(fw);
	return 0;

}