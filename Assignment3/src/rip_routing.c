#include <rip_main.h>
#include <rip_routing.h>


void rip_routing_init_graph()
{
    unsigned int no_nodes = rip_routing_table_entry_number+1;
    unsigned int i = 0;
    rip_obj_new_graph();
    rip_obj_new_dist_hop_vector();
    for (i = 0; i < no_nodes; i++) {
	rip_obj_set_dist_hop_vect_ent(dist_hop_vect, i,
				      COST_INFINITY, INVALID_HOP_I);
    }
    //set the self
    rip_obj_set_dist_hop_vect_ent(dist_hop_vect, 0,
				  0, 0);
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
    unsigned int no_nodes = rip_routing_table_entry_number + 1;
    unsigned int i = 0, j = 0;
    for (i = 0; i < no_nodes; i++) {
	for (j = 0; j < no_nodes; j++) {
	    rip_routing_relax_edges(i, j);
	}
    }
}
