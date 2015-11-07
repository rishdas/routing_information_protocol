#include <rip_main.h>
#include <rip_proto.h>

void rip_net_bind_port (void)
{
    if ((rip_node_config->socket = socket (AF_INET, SOCK_DGRAM,0)) < 0){
	perror ("socket");
	exit (1);
    }
    if ((bind (rip_node_config->socket, 
	       (struct sockaddr *)rip_node_config->inet,
	       sizeof (struct sockaddr))) < 0) {
	perror ("bind");
	exit(1);
    }
    return;
};
