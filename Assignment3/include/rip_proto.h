/* main.c */
void rip_main_parseArgs (int , char **);

/* obj.c */
void *rip_malloc (size_t );
node_config_t rip_obj_new_node_config (void);
void rip_obj_set_node_config (node_config_t, FILE *, 
			      struct sockaddr_in *, cost_t ,
			      unsigned int, bool_t, bool_t);
void rip_obj_set_node_config_inet (char *, char *);
void rip_obj_destroy_node_config (node_config_t);
node_info_t rip_obj_new_node_info (void);
void rip_obj_set_node_info (node_info_t, char *,
			    struct sockaddr_in *);
void rip_obj_destroy_node_info (node_info_t);
route_entry_t rip_obj_new_route_entry (void);
void rip_obj_set_route_entry (route_entry_t, node_info_t,
			      node_info_t, cost_t, 
			      unsigned short int);
void rip_obj_destroy_route_entry (route_entry_t);


/* global variables */
extern node_config_t rip_node_config;
