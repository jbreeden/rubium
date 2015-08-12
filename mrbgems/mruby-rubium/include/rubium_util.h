#ifndef LAMINA_UTIL_H
#define LAMINA_UTIL_H

#include <string>
#include "apr_pools.h"
#include "apr_network_io.h"

apr_socket_t* rubium_util_open_tcp_socket(apr_port_t port, apr_pool_t* pool);
int rubium_util_get_open_port(apr_pool_t* pool);

#endif /* LAMINA_UTIL_H */