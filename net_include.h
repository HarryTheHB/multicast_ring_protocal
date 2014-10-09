#include <stdio.h>

#include <stdlib.h>

#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h> 
#include <netdb.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>

/*#define PORT	     5855*/
#define PORT	     11011

#define MAX_MESS_LEN 1300
#define MESSAGE_SIZE 1212
#define PAYLOAD_SIZE 1200
#define RTR_SIZE     10
#define WINDOW_SIZE  5
#define NAME_LENGTH  80

#define INIT_TYPE    0
#define TOKEN_TYPE   1
#define MSG_TYPE     2

typedef struct _token{
	int seq;
	int aru;
	int tid;
	int hid;
	int fcc;
	int trt[RTR_SIZE];
}token;

typedef struct _message{
	int hid;
	int seq;
	int cont;
	char payload[PAYLOAD_SIZE];
}message;

typedef struct _initial_message{
	int index;
	int my_ip;
}initial_message;

typedef struct _message_list{
	int start;
	int count;
	message list[WINDOW_SIZE];
	bool valid[WINDOW_SIZE];
}message_list;
