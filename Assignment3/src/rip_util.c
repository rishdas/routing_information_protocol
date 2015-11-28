#include <rip_main.h>
#include <rip_proto.h>

void rip_util_print_routing_table (void)
{
    int i;
    char buff[INET_ADDRSTRLEN];
    route_entry_t entry;

    fprintf(stdout, "Routing table:\n");
    /* fprintf (stdout,"Initial routing table:\n" */
    /* 	     "Node\tNext Hop\tCost\tTTL\n"); */
    fprintf (stdout,
	     "%-26s %-26s %-5s %-5s\n", "Node", "Next Hop", "Cost", "TTL");
    for (i = 0; i < rip_routing_table_entry_number; i++){
	memset (buff,0,INET_ADDRSTRLEN);
	entry = routingtable[i];
	fprintf (stdout,"%-26s %-26s %-5d %-5d\n", entry->destination->name,
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

void rip_util_record_start_time()
{
    gettimeofday(&start_time, 0);
    return;
}

void rip_util_record_display_convergence_time(bool_t has_rout_tab_changed)
{
    struct timeval convg_time;
    unsigned long convg_time_taken;
    static bool_t cal_needed = TRUE;

    if (has_rout_tab_changed == TRUE) {
	cal_needed = TRUE;
	return;
    }
    if (cal_needed == FALSE) {
	return;
    }
    cal_needed = FALSE;
    gettimeofday(&convg_time, 0);
    convg_time_taken = convg_time.tv_sec - start_time.tv_sec
	+ (convg_time.tv_usec - start_time.tv_usec)/1000000;
    if (convg_time_taken == 0) {
	convg_time_taken = (convg_time.tv_sec - start_time.tv_sec)*1000000
	+ convg_time.tv_usec - start_time.tv_usec;
	printf("Convergence Time(in microseconds): %lu\n", convg_time_taken);
	start_time = convg_time;
	return;
    }
    printf("Convergence Time(in seconds): %lu\n", convg_time_taken);
    start_time = convg_time;
    return;
}
