#include <rip_main.h>
#include <rip_proto.h>
#include <rip_routing.h>


void *rip_up(void *ptr) 
{
    route_entry_t re;
    int           i;
    bool_t        has_rout_tab_changed = TRUE;
    
    while (TRUE) {
	pthread_mutex_lock (&lock);
	if (!adtable.ready) {
	    goto end_loop;
	}
	if (adtable.is_empty) {
	    if (rip_util_is_update_required()) {
		rip_net_send_advertisement ();
		rip_util_print_routing_table();
		/* printf("Send adv empty\n"); */
	    }
	    adtable.ready = FALSE;
	    adtable.is_empty = TRUE;
	    goto end_loop;
	}
	printf ("rip_up(): Update from %s\n",adtable.neighbor->name);
	
	pthread_mutex_lock(&graph_lock);
	/*Print tables before update*/
	/* rip_routing_print_graph(); */
	/* rip_routing_print_dist_vector(); */
	rip_routing_update_dist_vector();
	/* rip_routing_print_dist_vector(); */
	rip_routing_update_graph();
	/* rip_routing_print_dist_vector(); */
	rip_routing_bellman_ford();
	has_rout_tab_changed = rip_routing_update_routing_table();

	/* rip_routing_print_dist_vector(); */
	/*Print tables after update*/
	rip_util_print_routing_table();
	/* rip_routing_print_dist_vector(); */
	/* rip_routing_print_graph(); */
	
	rip_util_record_display_convergence_time(has_rout_tab_changed);

	pthread_mutex_unlock(&graph_lock);
	printf("-----------------------------------\n");
	printf ("rip_up(): Update from %s\n",adtable.neighbor->name);
	for (i = 0; adtable.neightable[i]; i++) {
	    printf ("\tDestination = %s Cost = %d\n", 
		    adtable.neightable[i]->destination->name,
		    adtable.neightable[i]->cost);
	    rip_obj_destroy_route_entry (adtable.neightable[i]);
	}
	printf("-----------------------------------\n");
	memset (&adtable, 0, advert_entry_t_len);
	adtable.ready = FALSE;
	adtable.is_empty = TRUE;
	if (has_rout_tab_changed == TRUE
	    || rip_util_is_update_required()) {
	    rip_net_send_advertisement ();
	    /* printf("Send adv\n"); */
	}
	
    end_loop:
	pthread_mutex_unlock (&lock);
    }
}

void *rip_up_ttl(void *ptr)
{
    while(TRUE)
    {
	sleep(rip_node_config->period);
	/* printf("Updating TTL\n"); */
	pthread_mutex_lock(&graph_lock);
	rip_routing_decrement_ttl();
	pthread_mutex_unlock(&graph_lock);
    }
}
