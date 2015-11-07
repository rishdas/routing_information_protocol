#include <rip_main.h>
#include <rip_proto.h>

node_config_t rip_node_config;

void rip_main_parseArgs (int c, char **v) 
{
    int o;
    char *fconfig = NULL;
    char *port = NULL;
    char *ip = NULL;

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
    if ((rip_node_config->fconfig = fopen (fconfig, "r")) == NULL ){
	perror ("fopen");
	exit (1);
    }
    if (0/*rip_main_parse_config ()*/){
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
    
}

