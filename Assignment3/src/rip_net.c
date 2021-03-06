#include <rip_main.h>
#include <rip_proto.h>

static void rip_net_send_message (message_entry_t *, int);

static void rip_net_send_message (message_entry_t *me, int ms)
{
    int i;
 
    assert (me != NULL);
    
    for (i = 0; i < rip_routing_table_entry_number; i++) {
	if (routingtable[i]->cost == 1) {
	    if ((sendto (rip_node_config->ssock,
			 me, ms * message_entry_t_len, 0,
			 (struct sockaddr *)routingtable[i]->destination->inet,
			 sizeof (struct sockaddr))) < 0) {
		perror ("sendto:");
		exit(1);
	    };
	};
    };
    return;
}

void rip_net_bind_port (void)
{
    if ((rip_node_config->rsock = socket (AF_INET, SOCK_DGRAM,0)) < 0){
	perror ("receive socket");
	exit (1);
    } 
    fcntl (rip_node_config->rsock, F_SETFL, O_NONBLOCK);
    if ((bind (rip_node_config->rsock, 
	       (struct sockaddr *)rip_node_config->inet,
	       sizeof (struct sockaddr))) < 0) {
	perror ("bind");
	exit(1);
    }
    /* Semantically, the send socket creation should not be here */
    /* but being pragmatic, it is :-) */
    if ((rip_node_config->ssock = socket (AF_INET, SOCK_DGRAM,0)) < 0){
	perror ("send socket");
	exit (1);
    }
    rip_obj_init_last_update_sent();
    return;
};
void rip_net_build_split_message(message_entry_t *message, int *ms,
				 unsigned int ni)
{
    unsigned int i;
    for (i = 0; i < rip_routing_table_entry_number; i++) {
	if ((rip_routing_get_index(routingtable[i]->nexthop)) == ni) {
	    continue;
	}
	if (routingtable[i]->nexthop && routingtable[i]->cost >= 0) {
	    message[*ms].dest_addr = 
		routingtable[i]->destination->inet->sin_addr;
	    message[(*ms)++].cost = routingtable[i]->cost;
	}
    }
}
void rip_net_send_split_advertisement()
{
    unsigned int i;
    int ms = 0;
    message_entry_t message[MAXROUTE];
    
    for (i = 0; i < rip_routing_table_entry_number; i++) {
	if (routingtable[i]->cost == 1) {
	    memset (&message, 0, MAXROUTE * message_entry_t_len);
	    ms = 0;
	    rip_net_build_split_message(message, &ms, i);
	    if ((sendto (rip_node_config->ssock,
			 message, ms * message_entry_t_len, 0,
			 (struct sockaddr *)routingtable[i]->destination->inet,
			 sizeof (struct sockaddr))) < 0) {
		perror ("sendto:");
		exit(1);
	    };
	    /* printf("DEBUG : split sendto :%s %d\n", */
	    /* 	   routingtable[i]->destination->name, ms); */
	};
    };
    return;
}
void rip_net_send_advertisement (void)
{
    int i;
    int ms = 0;
    message_entry_t message[MAXROUTE];
    if (rip_node_config->shorizon) {
	/* printf("DEBUG :Taking split horizon path\n"); */
	rip_net_send_split_advertisement();
	rip_obj_set_last_update_sent();
	return;
    }
    memset (&message, 0, MAXROUTE * message_entry_t_len);
    for (i = 0; i < rip_routing_table_entry_number; i++) {
	if (routingtable[i]->nexthop && routingtable[i]->cost > 0) {
	    message[ms].dest_addr = 
		routingtable[i]->destination->inet->sin_addr;
	    message[ms++].cost = routingtable[i]->cost;
	}
    }
    rip_net_send_message (message, ms);
    rip_obj_set_last_update_sent();
    return;
}

int rip_net_recv_advertisement (node_info_t nd, message_entry_t *me) 
{
    int ret, numfd, entry_len;
    /* static int cnt = 2; */
    struct sockaddr_in nodeaddr;
    socklen_t nodeaddr_len;
    fd_set socket_set;
    struct timeval timeout;

    FD_ZERO (&socket_set);
    FD_SET (rip_node_config->rsock, &socket_set);
    /* /\* First time run: timeout = (period/2) *\/ */
    /* /\* Second time: timeout = period *\/ */
    /* cnt = (cnt <= 1) ? 1 : cnt; */
    timeout.tv_sec = rip_node_config->period;
    timeout.tv_usec = 0;

    ret = numfd = 0;
    entry_len = message_entry_t_len;

    if ((numfd = select (rip_node_config->rsock+1, &socket_set, NULL, NULL, 
			 &timeout)) < 0) {
	return -1;
    }
    if (numfd == 0) {
	return 0;
    }
    if (numfd) {
	nodeaddr_len = sizeof (struct sockaddr_in);       
	ret = recvfrom (rip_node_config->rsock, me, MAXROUTE * entry_len,
			0, (struct sockaddr *)&nodeaddr, &nodeaddr_len);

	nd->name = rip_net_get_name(nodeaddr.sin_addr);
	rip_obj_set_inet (nd->inet, &nodeaddr);
    }

    /* return the number of message_entry in the received message */
    return (ret/entry_len) ;
}

char *rip_net_inet_ntop (struct in_addr in)
{
    char *ret;
    
    ret = (char *) rip_malloc (INET_ADDRSTRLEN);
    inet_ntop (AF_INET, &in, ret, INET_ADDRSTRLEN);
    return ret;
}

char *rip_net_get_name (struct in_addr in)
{
    char *ret = NULL;
    unsigned int no_nodes = rip_routing_table_entry_number;
    unsigned int i = 0;

    for (i = 0; i < no_nodes; i++) {
	if (routingtable[i]->destination->inet->sin_addr.s_addr
	    == in.s_addr) {
	    ret = strdup(routingtable[i]->destination->name);
	    break;
	}
    }
    return ret;
}
