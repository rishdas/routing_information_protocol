#include <rip_main.h>
#include <rip_proto.h>

static void rip_net_send_message (message_entry_t *me, int ms)
{
    int i;
    char buff[INET_ADDRSTRLEN];
    
    assert (me != NULL);
    for (i = 0; i < ms; i++) {
	memset (buff, 0, INET_ADDRSTRLEN);
	
	inet_ntop (AF_INET, &(me[i].dest_addr), buff, INET_ADDRSTRLEN);
       
	fprintf (stderr,"message entry: IP = %s cost = %d\n",
		 buff,me[i].cost);
    }
    
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
