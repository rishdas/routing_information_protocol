#include <main.h>
#include <proto.h>

node_config_t rip_node_config;

void rip_main_parseArgs (int c, char **v) 
{
    int o;
    char *fconfig = NULL;
    char *port = NULL;
    
    opterr = 0;
    
    while ((o = getopt (c, v, "c:u:t:p:sd")) != -1) {
	switch (o){
	case 'c': 
	    fconfig = strdup (optarg);
	    assert (fconfig != NULL);
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
    if ((rip_node_config->fconfig = fopen (fconfig, "r")) == NULL ){
	perror ("fopen");
	exit (1);
    }
    
    
    
    return;
}


int main (int argc, char **argv)
{

    rip_node_config = rip_obj_new_node_config ();
    rip_main_parseArgs (argc, argv);
    
}

