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

