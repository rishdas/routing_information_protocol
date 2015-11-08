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
    return;
};

void rip_net_send_advertisement (void)
{
    int i;
    int ms = 0;
    message_entry_t message[MAXROUTE];

    memset (&message, 0, MAXROUTE * message_entry_t_len);
    for (i = 0; i < rip_routing_table_entry_number; i++) {
	if (routingtable[i]->nexthop && routingtable[i]->cost > 0) {
	    message[ms].dest_addr = 
		routingtable[i]->nexthop->inet->sin_addr;
	    message[ms++].cost = routingtable[i]->cost;
	}
    }
    rip_net_send_message (message, ms);
    return;
}
