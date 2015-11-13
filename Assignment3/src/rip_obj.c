#include <rip_main.h>
#include <rip_proto.h>
#include <rip_routing.h>

void *rip_malloc (size_t l)
{
    void *m;

    m = malloc (l);
    assert (m != NULL);
    memset (m, 0, l);
    
    return m;
}

node_config_t rip_obj_new_node_config (void)
{
    node_config_t ret;
    
    ret = (node_config_t) rip_malloc (node_config_t_len);
    ret->inet = (struct sockaddr_in *) rip_malloc (sizeof (struct sockaddr_in));
    return ret;
}

void rip_obj_set_node_config (node_config_t conf, FILE *f, 
			      struct sockaddr_in *i, cost_t t,
			      unsigned int p, bool_t s, bool_t d)
{
    assert (conf != NULL);
    conf->fconfig = f;
    conf->inet = i;
    conf->ttl = t;
    conf->period = p;
    conf->shorizon = s;
    conf->debug = d;
}

void rip_obj_set_node_config_inet (char *p, char *i)
{
    struct ifaddrs *ifarr, *ifp;
    struct sockaddr_in *in;

    getifaddrs (&ifarr);
    for (ifp = ifarr; ifp; ifp = ifp->ifa_next) {
	in = (struct sockaddr_in *)ifp->ifa_addr;
	if (AF_INET != in->sin_family) {
	    continue;
	}
	if (strcmp (ifp->ifa_name,i) != 0) {
	    continue;
	}
	rip_node_config->inet->sin_addr.s_addr = in->sin_addr.s_addr;
	rip_node_config->inet->sin_family = AF_INET;
	rip_node_config->inet->sin_port = htons (atoi (p));
	
	break;
    }
    return;
}

void rip_obj_set_inet (struct sockaddr_in *dest, struct sockaddr_in *src) 
{
    dest->sin_addr.s_addr = src->sin_addr.s_addr;
    dest->sin_port = rip_node_config->inet->sin_port;
    dest->sin_family = AF_INET;

    return;
}

void rip_obj_set_inet_from_addr (struct sockaddr_in *dest, struct in_addr *src) 
{
    struct sockaddr_in in;
    
    in.sin_addr = *src;
    rip_obj_set_inet (dest, &in);
    return;
}

void rip_obj_destroy_node_config (node_config_t conf)
{
    assert (conf != NULL);
    if (conf->fconfig)
	fclose (conf->fconfig);
    if (!conf->debug)
	fclose (stderr);
    free (conf);
    return;
}

node_info_t rip_obj_new_node_info (void)
{
    node_info_t ret;
    struct sockaddr_in *in;
    
    ret = (node_info_t) rip_malloc (node_info_t_len);
    in = (struct sockaddr_in *) rip_malloc (sizeof (struct sockaddr_in));
    ret->inet = in;
    return ret;
}

void rip_obj_set_node_info (node_info_t info, char *n,
			    struct sockaddr_in *i)
{
    assert (info != NULL);
    info->name = n;
    info->inet = i;
    return;
}

void rip_obj_destroy_node_info (node_info_t info)
{
    assert (info != NULL);
    if (info->name)
	free (info->name);
    free (info);
    return;
}

route_entry_t rip_obj_new_route_entry (void)
{
    route_entry_t ret;
    
    ret = (route_entry_t) rip_malloc (route_entry_t_len);
    return ret;
}

void rip_obj_set_route_entry (route_entry_t entry, node_info_t d,
			      node_info_t n, cost_t c, 
			      unsigned short int t)
{
    assert (entry != NULL);
    entry->destination = d;
    entry->nexthop = n;
    entry->cost = c;
    entry->ttl = t;
    return;
}

void rip_obj_destroy_route_entry (route_entry_t entry)
{
    assert (entry != NULL);
    if (entry->destination)
	rip_obj_destroy_node_info (entry->destination);
    if (entry->nexthop)
	rip_obj_destroy_node_info (entry->nexthop);
    free (entry);
    return;
}

void rip_obj_push_recv_advertisement (node_info_t n, message_entry_t *me, int me_len) 
{
    int i;
    
    /* Attention: the rip_up() thread must free adtable members and clean up adtable 
    /* ( memset(&adtable, 0,....) ) */
    /* after fetching information from it */
    pthread_mutex_lock (&lock);
    adtable.neighbor = n;
    for (i = 0; i < me_len;  i++) {
	adtable.neightable[i] = rip_obj_new_route_entry ();
	adtable.neightable[i]->destination = rip_obj_new_node_info ();
	adtable.neightable[i]->destination->name = rip_net_inet_ntop (me[i].dest_addr);
	rip_obj_set_inet_from_addr (adtable.neightable[i]->destination->inet, &me[i].dest_addr);
	adtable.neightable[i]->nexthop = NULL; /* not used here */
	adtable.neightable[i]->cost = me[i].cost;
    }
    pthread_mutex_lock (&lock);

    return;
}


void rip_obj_new_graph()
{
    unsigned int no_nodes;
    unsigned int i = 0, j = 0;
    no_nodes = rip_routing_table_entry_number;
    if (no_nodes <= 1) {
	r_graph = NULL;
	return;
    }
    r_graph = (route_graph_t) rip_malloc(no_nodes * sizeof(route_graph_row_t));
    for (i = 0; i < no_nodes; i++) {
	r_graph[i] = (route_graph_row_t)
	    rip_malloc(no_nodes * sizeof(route_graph_entry_t));
    }
    /*Initialize Entries*/
    for (i = 0; i < no_nodes; i++) {
	for (j = 0; j < no_nodes; j++) {
	    r_graph[i][j].cost = COST_INFINITY;
	    r_graph[i][j].ttl = rip_node_config->ttl * rip_node_config->period;
	}
    }
    return;
}

void rip_obj_set_graph_entry(unsigned int r_index,
			     unsigned int c_index, cost_t c,
			     unsigned short int ttl)
{
    if (r_graph == NULL){
	//Log
	return;
    }
    r_graph[r_index][c_index].cost = c;
    r_graph[r_index][c_index].ttl = ttl;
    return;
}
void rip_obj_destroy_route_graph()
{
    unsigned int i = 0;
    unsigned int no_nodes = rip_routing_table_entry_number;
    if (r_graph == NULL) {
	//Log
	return;
    }
    for (i = 0; i <= no_nodes; i++) {
	if(r_graph[i] == NULL) {
	    //Log
	    continue;
	}
	free(r_graph[i]);
    }
    free(r_graph);
    return;
}
void rip_obj_new_dist_hop_vector()
{
    dist_hop_vect =
	(route_dist_hop_vect_t)rip_malloc((rip_routing_table_entry_number+1)
					  *sizeof(route_dist_hop_t));
    return;
}
void rip_obj_set_dist_hop_vect_ent(unsigned int index, cost_t c,
				   unsigned int hop_index)
{
    dist_hop_vect[index].cost = c;
    dist_hop_vect[index].hop_index = hop_index;
    return;
}
void rip_obj_destroy_dist_hop_vector(route_dist_hop_vect_t dist_hop_vect)
{
    if (dist_hop_vect == NULL) {
	//Log
	return;
    }
    free(dist_hop_vect);
    return;
}
