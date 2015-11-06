/**
 * @file   main.h
 * 
 * @brief  This file declares main types and macros
 * 
 */

#include <stdio.h>

#define cost_t    unsigned int
#define bool_t    char

#define FALSE     (bool_t )0
#define TRUE      (bool_t )1
#define MAXROUTE       255


/**
 * Defines node's configuration
 */
struct _node_config
{
    FILE *config;		/**< Config file stream */
    struct sockaddr_in inet;	/**< Inet info, including port  */
    cost_t ttl;			/**< Default TTL for routing table entries */
    unsigned int period;	/**< Sending update message period */
    bool_t shorizon;		/**< Using split horizon ? */
};

typedef struct _node_config *node_config_t;
#define node_config_t_len sizeof (struct _node_config)

/**
 * Information about nodes (to be used in graph/table)
 * 
 */
struct _node 
{
    char *name;			/**< Name of the node */
    struct sockaddr_in inet;	/**< Inet related information */
};

typedef struct _node *node_t;
#define node_config_t_len sizeof (struct _node)

/**
 * Route entry for routing table
 * 
 */
struct _route_entry
{
    node_t destination;		/**< Destination */
    node_t nexthop;		/**< Next hop */
    cost_t cost;		/**< Cost */
    unsigned short int ttl;	/**< TTL */
};

typedef struct _route_entry *route_entry_t;
#define route_entry_t_len sizeof (struct _route_entry)

route_entry_t routingtable[MAXROUTE];

/**
 * Each entry of a message. 
 * It is not a pointer because its desirable to be in-memory aligned.
 */

struct _message_entry 
{
    struct uint32_t dest_addr;
    cost_t cost;
} message_entry_t;

message_entry_t message[MAXMSGLEN];

