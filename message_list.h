#ifndef MESSAGE_LIST
#define MESSAGE_LIST
#include "net_include.h"

void copy_msg(message *mp1, message *mp2);
void message_gen(message *mp, int hid, int *seq);

void list_init(message_list *msg_list);

int  list_write(message_list *msg_list, FILE *fw);
bool list_get(message_list *msg_list, int seq, message *msg);
bool list_add(message_list *msg_list, message *msg);
void list_receive(message_list *msg_list, message *msg, FILE *fw);

void list_display(message_list *msg_list, bool all);

#endif