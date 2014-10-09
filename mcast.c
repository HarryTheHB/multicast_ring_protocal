#include "net_include.h"
#include "recv_dbg.h"


void mcast(int num_of_packets, int machine_index, int number_of_machines, int loss_rate);
void prepare(char *mess_buf, int *type, struct sockaddr_in *send_addr, int to);

int main(int argc, char **argv)
{
	return 0;
}


void mcast(int num_of_packets, int machine_index, int number_of_machines, int loss_rate)
{
	/*variable - */
	struct sockaddr_in name;
    struct sockaddr_in send_addr;

    int                mcast_addr;
    int 			   next_addr;
    int                my_addr;

    struct ip_mreq     mreq;
    unsigned char      ttl_val;

    int                ss,sr;
    fd_set             mask;
    fd_set             dummy_mask,temp_mask;
    int                bytes;
    int                num;
    char               mess_buf[MAX_MESS_LEN];

    FILE               *fw;

	int                local_aru;
    int                tid;
    int                hid;
    char               *start;
    int                *type_ptr;
    int                type;
    bool			   flag;

    char               my_name[NAME_LENGTH] = {'\0'};
    struct hostent     h_ent, *p_h_ent;
    /*variable - user define */
    message_list       *msg_list_ptr;
    message            *msg_ptr;
    token              *token_ptr, *last_token_ptr;
    initial_message     *init_msg_ptr;

    /*initialize - local variable*/
    recv_dbg_init(loss_rate, machine_index);
    
    msg_list_ptr = malloc(8 + MESSAGE_SIZE * WINDOW_SIZE + WINDOW_SIZE * sizeof(bool));

    hid = machine_index;
    tid = 0;
    next_addr = -1;

    init_msg_ptr = (initial_message *)&mess_buf[sizeof(int)];
	msg_ptr = (message *)&mess_buf[sizeof(int)];
	token_ptr = (token *)&mess_buf[sizeof(int)];
    
	flag = false;
    /*get my ip address*/
    gethostname(my_name, NAME_LENGTH );
    printf("My host name is %s.\n", my_name);
    p_h_ent = gethostbyname(my_name);
    if ( p_h_ent == NULL ) {
        printf("myip: gethostbyname error.\n");
        exit(1);
    }

    memcpy( &h_ent, p_h_ent, sizeof(h_ent));
    memcpy( &my_addr, h_ent.h_addr_list[0], sizeof(my_addr) );

    
	/*initialize - udp*/
    mcast_addr = 225 << 24 | 0 << 16 | 1 << 8 | 1; /* (225.0.1.1) */

    sr = socket(AF_INET, SOCK_DGRAM, 0); /* socket for receiving */
    if (sr<0) {
        perror("Mcast: socket");
        exit(1);
    }

    name.sin_family = AF_INET;
    name.sin_addr.s_addr = INADDR_ANY;
    name.sin_port = htons(PORT);

    if ( bind( sr, (struct sockaddr *)&name, sizeof(name) ) < 0 ) {
        perror("Mcast: bind");
        exit(1);
    }

    mreq.imr_multiaddr.s_addr = htonl( mcast_addr );

     /* the interface could be changed to a specific interface if needed */
    mreq.imr_interface.s_addr = htonl( INADDR_ANY );

    if (setsockopt(sr, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&mreq, 
        sizeof(mreq)) < 0) 
    {
        perror("Mcast: problem in setsockopt to join multicast address" );
    }

    ss = socket(AF_INET, SOCK_DGRAM, 0); /* Socket for sending */
    if (ss<0) {
        perror("Mcast: socket");
        exit(1);
    }

    ttl_val = 1;
    if (setsockopt(ss, IPPROTO_IP, IP_MULTICAST_TTL, (void *)&ttl_val, 
        sizeof(ttl_val)) < 0) 
    {
        printf("Mcast: problem in setsockopt of multicast ttl %d - ignore in WinNT or Win95\n", ttl_val );
    }

    send_addr.sin_family = AF_INET;
    send_addr.sin_addr.s_addr = htonl(mcast_addr);  /* mcast address */
    send_addr.sin_port = htons(PORT);

    FD_ZERO( &mask );
    FD_ZERO( &dummy_mask );
    FD_SET( sr, &mask );

	/* start_mcast*/
    if(hid == 1)
    {
    	*start = 'y';
    	sendto(ss, start, sizeof(char), 0,
    		(struct sockaddr *)&send_addr, sizeof(send_addr));
    }
    else
    {
    	recv(sr, start, sizeof(char), 0);
    	if(*start != 'y')
    	{
    		printf("receive wrong start message\n");
    		exit(0);
    	}
    }

    /*mcast establish ring*/
    for(;;)
    {
    	type = INIT_TYPE;
    	prepare(mess_buf, &type, &send_addr, mcast_addr);
    	init_msg_ptr->my_ip = my_addr;
    	init_msg_ptr->index = hid;
    	sendto(ss, mess_buf, MAX_MESS_LEN, 0,
    		(struct sockaddr *)&send_addr, sizeof(send_addr) );


    	if((hid == 1 || flag) && next_addr != -1)
    	{
    		type = TOKEN_TYPE;
 			prepare(mess_buf, &type, &send_addr, next_addr);
 			sendto(ss, mess_buf, MAX_MESS_LEN, 0,
    			(struct sockaddr *)&send_addr, sizeof(send_addr) );
    	}

    	temp_mask = mask;
    	num = select( FD_SETSIZE, &temp_mask, &dummy_mask, &dummy_mask, NULL);
		if(num > 0) 
		{
			if(FD_ISSET(sr, &temp_mask)) 
			{
				bytes = recv_dbg(sr, mess_buf, MAX_MESS_LEN, 0);
				type_ptr = (int *)mess_buf;



				if(*type_ptr == TOKEN_TYPE)
				{

					if (hid == 1)
					{
						break;
					}
					else
					{
						flag = true;
					}
				}
				else if(*type_ptr == MSG_TYPE)
				{
					if(!flag && (init_msg_ptr->index == hid+1 
						|| (init_msg_ptr->index == number_of_machines && hid == 1)))
					{
						next_addr = init_msg_ptr->my_ip;
						flag = true;
					}
				}

			}
		}   	

    }


	/*mcast main*/
    for(;;)
    {

    	printf("ring established!\n");
    	break;
	/*if receive token*/



	/*if receive message*/



    }




    free(init_msg_ptr);
    free(msg_list_ptr);
    fclose(fw);
}

void prepare(char *mess_buf, int *type, struct sockaddr_in *send_addr, int to)
{
	memset(mess_buf, '/0', MAX_MESS_LEN);
	send_addr->sin_addr.s_addr = htonl(to);
	memcpy(mess_buf, type, sizeof(int));
}