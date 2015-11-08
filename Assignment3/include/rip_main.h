/**
 * @file   main.h
 * 
 * @brief  This file declares main types and macros
 * 
 */

#include <ctype.h>
#include <stdio.h>
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


#define cost_t    unsigned int
#define bool_t    char

#define FALSE     (bool_t )0
#define TRUE      (bool_t )1
#define MAXROUTE       255
#define COST_INFINITY  9999
#define MYSELF    "me"
#define DEF_PERIOD 30
#define DEF_TTL 3

/**
 * Defines node's configuration
 */
struct _node_config
{
    FILE *fconfig;		/**< Config file stream */
    struct sockaddr_in *inet;	/**< Inet info, including port  */
    int socket;			/**< UDP socket  */
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
    node_info_t destination;		/**< Destination */
    node_info_t nexthop;		/**< Next hop */
    cost_t cost;		/**< Cost */
    unsigned short int ttl;	/**< TTL */
};

typedef struct _route_entry *route_entry_t;
#define route_entry_t_len sizeof (struct _route_entry)

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
#define message_entry_t_len sizeof (struct _message_entry_t_len)

message_entry_t message[MAXROUTE];
