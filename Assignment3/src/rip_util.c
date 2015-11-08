#include <rip_main.h>
#include <rip_proto.h>

void rip_util_print_routing_table (void)
{
    int i;
    char buff[INET_ADDRSTRLEN];
    route_entry_t entry;
 
    fprintf (stdout,"Initial routing table:\n"
	     "Node\tNext Hop\t\tCost\tTTL\n");
    for (i = 0; i < rip_routing_table_entry_number; i++){
	memset (buff,0,INET_ADDRSTRLEN);
	entry = routingtable[i];
	if (entry->nexthop != NULL) {
	    inet_ntop (AF_INET,&(entry->nexthop->inet->sin_addr), 
		       buff,INET_ADDRSTRLEN);
	} else { 
	    strcpy (buff, "  Null  ");
	}
	fprintf (stdout,"%s\t%s  (%s)\t%d\t%d\n", entry->destination->name,
		 (entry->nexthop) ? 
		 entry->nexthop->name : "Null", buff, entry->cost, 
		 entry->ttl);
	
    };
    return;
}

    
