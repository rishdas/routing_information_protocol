#include <rip_main.h>
#include <rip_routing.h>


void rip_routing_init_graph()
{
    unsigned int no_nodes = rip_routing_table_entry_number;
    unsigned int i = 0;
    rip_obj_new_graph();
    rip_obj_new_dist_hop_vector();
    for (i = 0; i < no_nodes; i++) {
	rip_obj_set_dist_hop_vect_ent(i, COST_INFINITY, INVALID_HOP_I);
    }
    //set the self
    rip_obj_set_dist_hop_vect_ent(0, 0, 0);
    return;
}
void rip_routing_relax_edges(unsigned int r_index, unsigned int c_index)
{
    if (dist_hop_vect[c_index].cost >
	dist_hop_vect[r_index].cost + r_graph[r_index][c_index].cost) {
	dist_hop_vect[c_index].cost =
	    dist_hop_vect[r_index].cost + r_graph[r_index][c_index].cost;
	dist_hop_vect[c_index].hop_index = r_index;
    }
}
void rip_routing_bellman_ford()
{
    unsigned int no_nodes = rip_routing_table_entry_number;
    unsigned int i = 0, j = 0;
    for (i = 0; i < no_nodes; i++) {
	for (j = 0; j < no_nodes; j++) {
	    rip_routing_relax_edges(i, j);
	}
    }
}

unsigned int rip_routing_get_index(node_info_t entry)
{
    unsigned int no_nodes = rip_routing_table_entry_number;
    unsigned int i = 0;
    route_entry_t r_entry;
    if (entry == NULL) {
	return 0;
    }
    for(i=0; i < no_nodes; i++) {
	r_entry = routingtable[i];
	if (rip_util_is_equal(r_entry->destination, entry) == TRUE) {
	    return i;
	}
    }
    return -1;
}

void rip_routing_update_graph()
{
    unsigned int neighbor_index = 0;
    unsigned int advert_neig_index = 0;
    unsigned int i = 0;
    neighbor_index = rip_routing_get_index(adtable.neighbor);
    rip_obj_set_graph_entry(neighbor_index, neighbor_index, 0,
			    rip_node_config->ttl*rip_node_config->period);
    rip_obj_set_graph_entry(0, neighbor_index, 1,
			    rip_node_config->ttl*rip_node_config->period);
    dist_hop_vect[neighbor_index].cost = 1;
    dist_hop_vect[neighbor_index].hop_index = neighbor_index;
    for (i = 0; adtable.neightable[i]; i++) {
	advert_neig_index =
	    rip_routing_get_index(adtable.neightable[i]->destination);
	rip_obj_set_graph_entry(neighbor_index, advert_neig_index,
				adtable.neightable[i]->cost,
				rip_node_config->ttl*rip_node_config->period);
    }
}

void rip_routing_print_graph()
{
    unsigned int no_nodes = rip_routing_table_entry_number;
    unsigned int i = 0, j = 0;
    printf("+++++++++++++Topology Graph++++++++++++++\n");
    for (i = 0; i < no_nodes; i++) {
	for (j = 0; j < no_nodes; j++) {
	    printf("\t%u\t%u\t",
		   r_graph[i][j].cost, r_graph[i][j].ttl);
	}
	printf("\n");
    }
}
void rip_routing_print_dist_vector()
{
    unsigned int no_nodes = rip_routing_table_entry_number;
    unsigned int i = 0;
    printf("-------------Distance Vector---------------\n");
    for (i = 0; i< no_nodes; i++) {
	printf(" %u: Cost:%u Next Hop:%u ",
	       i, dist_hop_vect[i].cost, dist_hop_vect[i].hop_index);
    }
    printf("\n");
}
bool_t rip_routing_is_rout_tab_dist_vect_same(unsigned int index)
{
    unsigned int dist_vect_index = dist_hop_vect[index].hop_index;
    if (rip_util_is_equal(routingtable[index]->nexthop,
			  routingtable[dist_vect_index]->destination)) {

	if (routingtable[index]->cost == dist_hop_vect[index].cost) {
	    return TRUE;
	}
    }
    return FALSE;
}
void rip_routing_update_rout_tab_with_dist_vect(unsigned int index)
{
    unsigned int dist_vect_index = dist_hop_vect[index].hop_index;
    node_info_t next_hop;

    next_hop = routingtable[dist_vect_index]->destination;
    routingtable[index]->nexthop = next_hop;
    routingtable[index]->cost = dist_hop_vect[index].cost;
    routingtable[index]->ttl =
	rip_node_config->ttl*rip_node_config->period;
    return;
}
bool_t rip_routing_update_routing_table()
{
    unsigned int no_nodes = rip_routing_table_entry_number;
    bool_t has_changed = FALSE;
    unsigned int i = 0;
    for (i = 0; i<no_nodes; i++) {
	routingtable[i]->ttl = r_graph[0][i].ttl;
	if (r_graph[0][i].ttl == 0) {
	    printf ("\nTTL ZERO: %s\n", routingtable[i]->destination->name);
	}
	if (rip_routing_is_rout_tab_dist_vect_same(i)) {
	    continue;
	} else {
	    has_changed = TRUE;
	    rip_routing_update_rout_tab_with_dist_vect(i);
	}
    }
    return has_changed;
}
void rip_routing_decrement_ttl_routing_table()
{
    unsigned int no_nodes = rip_routing_table_entry_number;
    unsigned int i = 0;

    for (i = 0; i < no_nodes; i++) {
	if (routingtable[i]->cost != COST_INFINITY
	    && routingtable[i]->cost > 0) {
	    routingtable[i]->ttl -= rip_node_config->period;
	    if (routingtable[i]->ttl <= 0) {
		routingtable[i]->cost = COST_INFINITY;
		routingtable[i]->ttl =
		    rip_node_config->ttl * rip_node_config->period;
		routingtable[i]->nexthop = NULL;
		dist_hop_vect[i].cost = COST_INFINITY;
	    }
	}
    }
}
void rip_routing_decrement_ttl()
{
    unsigned int no_nodes = rip_routing_table_entry_number;
    unsigned int i = 0, j = 0;
    for (i = 0; i < no_nodes; i++) {
	for (j = 0; j < no_nodes; j++) {

	    if (r_graph[i][j].cost != COST_INFINITY) {
		
		r_graph[i][j].ttl -= rip_node_config->period;

		if (r_graph[i][j].ttl <= 0) {
		    r_graph[i][j].cost = COST_INFINITY;
		    dist_hop_vect[j].cost = COST_INFINITY;
		}
	    }
	}
    }
    r_graph[0][0].ttl =
	rip_node_config->ttl*rip_node_config->period;
    r_graph[0][0].cost = 0;
    dist_hop_vect[0].cost = 0;
    return;
}

void rip_routing_update_dist_vector()
{
    unsigned int no_nodes = rip_routing_table_entry_number;
    unsigned int i = 0;
    for (i = 0; i < no_nodes; i++) {
	if (dist_hop_vect[i].hop_index == -1) {
	    continue;
	}
	if (dist_hop_vect[i].hop_index != 0 && dist_hop_vect[i].cost > 1) {
	    dist_hop_vect[i].cost = COST_INFINITY;
	}
    }
    return;
}
