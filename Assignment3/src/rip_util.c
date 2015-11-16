#include <rip_main.h>
#include <rip_proto.h>

void rip_util_print_routing_table (void)
{
    int i;
    char buff[INET_ADDRSTRLEN];
    route_entry_t entry;
 
    fprintf (stdout,"Initial routing table:\n"
	     "Node      \tNext Hop\tCost\tTTL\n");
    for (i = 0; i < rip_routing_table_entry_number; i++){
	memset (buff,0,INET_ADDRSTRLEN);
	entry = routingtable[i];
	fprintf (stdout,"%s\t%s\t%d\t%d\n", entry->destination->name,
		 (entry->nexthop) ? 
		 entry->nexthop->name : "Null        ", entry->cost, 
		 entry->ttl);	
    };
    return;
}

bool_t rip_util_is_equal(node_info_t node_a, node_info_t node_b)
{
    if (node_a == NULL) {
	return FALSE;
    }
    if (node_b == NULL) {
	return FALSE;
    }
    if (node_a->inet->sin_addr.s_addr
	== node_b->inet->sin_addr.s_addr) {
	return TRUE;
    }
    return FALSE;
}

bool_t rip_util_is_update_required()
{
    struct timeval curr_time;
    unsigned long  time_since_last_update;
    
    if (last_update_sent.tv_sec == 0
	|| last_update_sent.tv_usec == 0) {
	gettimeofday(&last_update_sent, 0);
	return TRUE;
    }
    gettimeofday(&curr_time, 0);
    time_since_last_update = (last_update_sent.tv_sec - curr_time.tv_sec)
	+(last_update_sent.tv_usec - curr_time.tv_usec)/1000000;

    if (time_since_last_update >= 30) {
	return TRUE;
    }
    return FALSE;
}
