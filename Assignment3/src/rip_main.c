#include <rip_main.h>
#include <rip_proto.h>

node_config_t rip_node_config;
unsigned int rip_routing_table_entry_number;

void rip_main_insert_entry_table_myself (void)
{
    node_info_t myself;
    route_entry_t entry;
    
    rip_routing_table_entry_number = 0;

    myself = rip_obj_new_node_info ();
    entry = rip_obj_new_route_entry ();
    
    myself->name = strdup (MYSELF);
    myself->inet = rip_node_config->inet;
    entry->destination = myself;
    entry->nexthop = myself;
    entry->cost = 0;
    entry->ttl = rip_node_config->ttl * rip_node_config->period;

    routingtable[rip_routing_table_entry_number++] = entry;
    return;
}

int rip_main_parse_config (void)
{
    char line[128];
    char *tk, *l;
    node_info_t info;
    route_entry_t entry;
 
    while (fgets (line, 128, rip_node_config->fconfig)){
	l = line;
	info = rip_obj_new_node_info ();
	entry = rip_obj_new_route_entry ();
	tk = strsep (&l, " ");
	info->name = strdup (tk);
	tk = strsep (&l, " ");
	inet_pton(AF_INET, tk, &(info->inet->sin_addr));
	tk = strsep (&l, "\n");
	if (!strcmp (tk, "yes")){
	    entry->destination = info;
	    entry->nexthop = info;
	    entry->cost = 1;
	}
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

    fclose (rip_node_config->fconfig);
    return 0;
}

void rip_main_parseArgs (int c, char **v) 
{
    int o;
    char *fconfig = NULL;
    char *port = NULL;
    char *ip = NULL;

    /* default config values */
    rip_node_config->period = 30;
    rip_node_config->ttl = 3;
    rip_node_config->debug = TRUE;
    
    opterr = 0;
    
    while ((o = getopt (c, v, "c:u:t:i:p:sd")) != -1) {
	switch (o){
	case 'c': 
	    fconfig = strdup (optarg);
	    assert (fconfig != NULL);
	    break;
	case 'i':
	    ip = strdup (optarg);
	    assert (ip != NULL);
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
    if (port && ip)
	rip_obj_set_node_config_inet (port,ip);
    else{
	fprintf (stderr,"%s: must inform UDP port AND IP address!\n",v[0]);
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


int main (int argc, char **argv)
{

    rip_node_config = rip_obj_new_node_config ();
    rip_main_parseArgs (argc, argv);
    /*if (node_config->debug)*/
    rip_util_print_routing_table ();
    rip_net_bind_port ();

    return 0;
}

