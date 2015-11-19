/**
 * @file   main.h
 * 
 * @brief  This file declares main types and macros
 * 
 */

#include <ctype.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <ifaddrs.h>
#include <pthread.h>
#include <signal.h>

#define cost_t    unsigned int
#define bool_t    char

#define FALSE     (bool_t )0
#define TRUE      (bool_t )1
#define MAXROUTE       255
#define MAXNODE        255
#define COST_INFINITY  9999
#define INVALID_HOP_I  -1
#define MYSELF    "me"
#define DEF_PERIOD 30
#define DEF_TTL 3
#define MAX_HOST_LENGTH 255

/**
 * Defines node's configuration
 */
struct _node_config
{
    FILE *fconfig;		/**< Config file stream */
    struct sockaddr_in *inet;	/**< Inet info, including port  */
    int rsock;			/**< Receive UDP socket  */
    int ssock;			/**< Send UDP socket */
    cost_t ttl;			/**< Default TTL for routing table entries */
    unsigned int period;	/**< Sending update message period */
    bool_t shorizon;		/**< Using split horizon ? */
    bool_t debug;		/**< Is in debug mode ? */
};

typedef struct _node_config *node_config_t;
#define node_config_t_len sizeof (struct _node_config)

/**
 * Information about nodes (to be used in graph/table)
 * 
 */
struct _node_info
{
    char *name;			/**< Name of the node */
    struct sockaddr_in *inet;	/**< Inet related information */
};

typedef struct _node_info *node_info_t;
#define node_info_t_len sizeof (struct _node_info)

/**
 * Route entry for routing table
 * 
 */
struct _route_entry
{
    node_info_t destination;	/**< Destination */
    node_info_t nexthop;	/**< Next hop */
    cost_t cost;		/**< Cost */
    unsigned short int ttl;	/**< TTL */
};

typedef struct _route_entry *route_entry_t;
#define route_entry_t_len sizeof (struct _route_entry)
node_config_t rip_node_config;
unsigned int rip_routing_table_entry_number;
route_entry_t routingtable[MAXROUTE];

/**
 * Advertisement received from neighbor. This struct is 
 * populated from message. advert_entry_t isn't a pointer
 */
struct _advert_entry
{
    node_info_t neighbor;		/**< Advertisement sender */
    route_entry_t neightable[MAXROUTE]; /**< Advertised table */
    bool_t ready;                       /**< Advertised message ready for thread*/
    bool_t is_empty;                    /*is advent_entry buff empty*/
};

typedef struct _advert_entry advert_entry_t;
#define advert_entry_t_len sizeof (struct _advert_entry)
advert_entry_t adtable;

/**
 * Each entry of a message. 
 * It is not a pointer because its desirable to be in-memory aligned.
 */

struct _message_entry 
{
    struct in_addr dest_addr;
    cost_t cost;
};

typedef struct _message_entry message_entry_t;
#define message_entry_t_len sizeof (struct _message_entry)

typedef struct _route_graph_entry
{
    cost_t             cost;
    unsigned short int ttl;
} route_graph_entry_t;

typedef route_graph_entry_t*  route_graph_row_t;
typedef route_graph_entry_t** route_graph_t;
route_graph_t r_graph;

typedef struct _route_dist_hop
{
    cost_t       cost;
    unsigned int hop_index;
} route_dist_hop_t;
typedef route_dist_hop_t* route_dist_hop_vect_t;

route_dist_hop_vect_t dist_hop_vect;

/* Mutex */
pthread_mutex_t lock;
pthread_mutex_t graph_lock;
/*Time tracking*/
struct timeval   last_update_sent;
