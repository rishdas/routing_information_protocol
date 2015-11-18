/* rip_main.c */
void rip_main_insert_entry_table_myself (void);
void rip_main_parseArgs (int , char **);
int rip_main_parse_config (void);
int rip_main_loop (void);


/* rip_obj.c */
void *rip_malloc (size_t );
node_config_t rip_obj_new_node_config (void);
void rip_obj_set_node_config (node_config_t, FILE *, 
			      struct sockaddr_in *, cost_t ,
			      unsigned int, bool_t, bool_t);
void rip_obj_set_node_config_inet (char *, char *);
void rip_obj_set_inet (struct sockaddr_in *, struct sockaddr_in *);
void rip_obj_set_inet_from_addr (struct sockaddr_in *, struct in_addr *); 
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
void rip_obj_push_recv_advertisement (node_info_t, message_entry_t *, int);


/* rip_util.c */
void rip_util_print_routing_table (void);

/* rip_net.c */
void rip_net_bind_port (void);
void rip_net_send_advertisement (void);
char *rip_net_inet_ntop (struct in_addr );

/* rip_up.c */
void *rip_up(void *);
void *rip_up_ttl(void *);

/* global variables */
extern node_config_t rip_node_config;
extern unsigned int rip_routing_table_entry_number;
extern route_entry_t routingtable[];
extern advert_entry_t adtable;
extern pthread_mutex_t lock;
extern pthread_mutex_t graph_lock;
