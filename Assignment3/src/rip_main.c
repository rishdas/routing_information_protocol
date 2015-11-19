#include <rip_main.h>
#include <rip_proto.h>
#include <rip_routing.h>


void rip_main_insert_entry_table_myself (void)
{
    node_info_t myself;
    route_entry_t entry;
    
    rip_routing_table_entry_number = 0;

    myself = rip_obj_new_node_info ();
    entry = rip_obj_new_route_entry ();
    
//    myself->name = rip_net_inet_ntop (rip_node_config->inet->sin_addr);
    myself->name = rip_obj_new_char_buf(MAX_HOST_LENGTH);
    gethostname(myself->name, MAX_HOST_LENGTH);
    myself->inet = rip_node_config->inet;
    entry->destination = myself;
    entry->nexthop = myself;
    entry->cost = 0;
    entry->ttl = rip_node_config->ttl * rip_node_config->period;

    routingtable[rip_routing_table_entry_number++] = entry;
    return;
}
void rip_main_init_graph_distance_vector()
{
    unsigned int i = 0;
    unsigned int no_nodes = rip_routing_table_entry_number;

    rip_routing_init_graph();
    for (i = 0; i < no_nodes; i++) {
	rip_obj_set_graph_entry(0, i,
				routingtable[i]->cost, routingtable[i]->ttl);
	rip_obj_set_dist_hop_vect_ent(i , routingtable[i]->cost,
				      rip_routing_get_index(
					  routingtable[i]->nexthop));
    }
    return;
}
void rip_main_fill_the_addr(node_info_t *info, char *name)
{
    struct addrinfo *host_addr_info_list;
    struct addrinfo *host_addr_info;
    char            addr[100];

    getaddrinfo(name, NULL, NULL, &host_addr_info_list);

    for (host_addr_info = host_addr_info_list; host_addr_info != NULL;
	 host_addr_info = host_addr_info->ai_next) {

	if (host_addr_info->ai_family == AF_INET) {
	    (*info)->inet->sin_addr =
		((struct sockaddr_in *)host_addr_info->ai_addr)->sin_addr;
	    (*info)->inet->sin_family = AF_INET;
	    (*info)->inet->sin_port = rip_node_config->inet->sin_port;
	    (*info)->name = strdup(name);
	    break;
	}
	
    }
    return;
}
int rip_main_parse_config (void)
{
    char            line[128];
    char            *tk, *l;
    node_info_t     info;
    route_entry_t   entry;

    
    while (fgets (line, 128, rip_node_config->fconfig)){
	l = line;
	info = rip_obj_new_node_info ();
	entry = rip_obj_new_route_entry ();
	/* get IP from first column, and populate node_info_t->inet also */
	/* with address family and destination port */
	tk = strsep (&l, " ");
	rip_main_fill_the_addr(&info, tk);
	
        /* get yes/no. If 'yes', add node as destination and nexthop  */
	/* with cost = 1 */
	tk = strsep (&l, "\n");
	if (!strcmp (tk, "yes")){
	    entry->destination = info;
	    entry->nexthop = info;
	    entry->cost = 1;
	}
	/* If 'no', add node only as destination (nexthop is NULL), and */
	/* cost to infinity */
	else if (!strcmp (tk,"no")){
	    entry->destination = info;
	    entry->cost = COST_INFINITY;
	} else {
	    fprintf (stderr,"config line error: %s\n",line);
	    return -1;
	}
	/* default TTL */
	entry->ttl = rip_node_config->ttl * rip_node_config->period;
	routingtable[rip_routing_table_entry_number++] = entry;
    }
    rip_main_init_graph_distance_vector();
    fclose (rip_node_config->fconfig);
    return 0;
}

void rip_main_parse_args (int c, char **v) 
{
    int o;
    char *fconfig = NULL;
    char *port = NULL;
    char *iface = NULL;

    /* default config values */
    rip_node_config->period = DEF_PERIOD;
    rip_node_config->ttl = DEF_TTL;
    rip_node_config->debug = TRUE;
    iface = strdup ("eth0");

    opterr = 0;
    
    while ((o = getopt (c, v, "c:u:t:i:p:sd")) != -1) {
	switch (o){
	case 'c': 
	    fconfig = strdup (optarg);
	    assert (fconfig != NULL);
	    break;
	case 'i':
	    free (iface);
	    iface = strdup (optarg);
	    assert (iface != NULL);
	    break;
	case 'u':
	    port = strdup (optarg);
	    assert (port != NULL);
	    break;
	case 't':
	    rip_node_config->ttl = atoi (optarg);
	    break;
	case 'p':
	    rip_node_config->period = atoi (optarg);
	    break;
	case 's':
	    rip_node_config->shorizon = TRUE;
	    break;
	case 'd':
	    rip_node_config->debug = TRUE;
	    break;
	}
    }
    
    if (!fconfig){
	fprintf (stderr,"%s: must inform config file!\n",v[0]);
	exit (1);
    }
    if (port) {
	rip_obj_set_node_config_inet (port, iface);
    } else {
	fprintf (stderr, "%s: must inform UDP port !!\n", v[0]);
	exit (1);
    }    
/* How to access inet information (IP address and port) in 
   rip_node_config->inet
   ***
   char b[INET_ADDRSTRLEN];
   printf ("Port = %d ", ntohs (rip_node_config->inet->sin_port));
   inet_ntop (AF_INET, &(rip_node_config->inet->sin_addr), b,
   INET_ADDRSTRLEN);
   printf ("IP = %s\n",b);
*/
    rip_main_insert_entry_table_myself ();
    if ((rip_node_config->fconfig = fopen (fconfig, "r")) == NULL ){
	perror ("fopen");
	exit (1);
    }
    if (rip_main_parse_config ()){
	fprintf (stderr,"%s: error parsing config file\n",v[0]);
	exit (1);
    }
    if (!rip_node_config->debug){
	fclose (stderr);
	freopen ("rip.log","a+",stderr);
    }

    return;
}

int rip_main_loop (void) 
{
    int ret = 0;
    static int cnt = 0;
    message_entry_t message[MAXROUTE];
    int message_entry_num;
    node_info_t node;

    /* sender node information */
    node = rip_obj_new_node_info ();

    while (TRUE) {

	pthread_mutex_lock (&lock);
	if (adtable.ready) {
            /* TRUE: the thread didn't read the adverstisement */
	    pthread_mutex_unlock (&lock);	
	    continue;
	}
	adtable.ready = FALSE;
	adtable.is_empty = TRUE;
	pthread_mutex_unlock (&lock);	
	node->name = NULL;
	memset (message, 0, MAXROUTE * message_entry_t_len);
	if ((message_entry_num =
	    rip_net_recv_advertisement (node,message)) < 0) {
	    ret = -1;
	    break;
	}
	if (message_entry_num == 0) {
	    pthread_mutex_lock (&lock);
	    adtable.is_empty = TRUE;
	    adtable.ready = TRUE;
	    pthread_mutex_unlock (&lock);
	} else if (node->name) {
	    /* "push" received advertisement to rip_up() through */
	    /* adtable[], from the second loop and so */
	    rip_obj_push_recv_advertisement (node,message,message_entry_num);
	}
    }
    return ret;
}

int main (int argc, char **argv)
{
    pthread_t rip_up_thread;
    pthread_t rip_up_ttl_thread;

    rip_node_config = rip_obj_new_node_config ();

    rip_main_parse_args (argc, argv);

    /*if (node_config->debug)*/
    rip_util_print_routing_table ();

    /* Initialize adtable */
    memset (&adtable, 0, advert_entry_t_len);
    adtable.ready = FALSE;

    rip_net_bind_port ();

    if ((pthread_create (&rip_up_thread, NULL, rip_up, NULL)) != 0) {
	perror ("pthread_create");
	exit (1);
    }
    if ((pthread_create (&rip_up_ttl_thread, NULL, rip_up_ttl, NULL)) != 0) {
	perror ("pthread_create");
	exit (1);
    }
    
    rip_main_loop ();

    return 0;
}
