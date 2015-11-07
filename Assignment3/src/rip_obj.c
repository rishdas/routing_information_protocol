#include <main.h>
#include <proto.h>

void *rip_malloc (void *m, size_t l)
{
    m = malloc (l);
    assert (m != NULL);
    memset (m, 0, l);
    
    return m;
}

node_config_t rip_obj_new_node_config (void)
{
    node_config_t ret;
    
    ret = (node_config_t) rip_malloc (node_config_t_len);
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
    
    ret = (node_info_t) rip_malloc (node_info_t_len);
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
    
    ret = (route_entry_t)rip_malloc (route_entry_t_len);
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
