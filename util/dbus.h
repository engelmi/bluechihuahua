#ifndef BLUECHIHUAHUA_DBUS
#define BLUECHIHUAHUA_DBUS

#include "memory.h"
#include "consts.h"

#include <assert.h>
#include <systemd/sd-bus.h>
#include <systemd/sd-event.h>

/*

*/

#define BUS_DEFINE_PROPERTY_GET2(function, bus_type, data_type, get1, get2) \
	int function(sd_bus *bus, const char *path, const char *interface,  \
		     const char *property, sd_bus_message *reply,           \
		     void *userdata, sd_bus_error *error) {                 \
		data_type *data = userdata;                                 \
                                                                            \
		assert(bus);                                                \
		assert(reply);                                              \
		assert(data);                                               \
                                                                            \
		return sd_bus_message_append(reply, bus_type,               \
					     get2(get1(data)));             \
	}

#define ident(x) (x)
#define BUS_DEFINE_PROPERTY_GET(function, bus_type, data_type, get1) \
	BUS_DEFINE_PROPERTY_GET2(function, bus_type, data_type, get1, ident)

#define ref(x) (*(x))
#define BUS_DEFINE_PROPERTY_GET_REF(function, bus_type, data_type, get) \
	BUS_DEFINE_PROPERTY_GET2(function, bus_type, data_type, ref, get)

#define BUS_DEFINE_PROPERTY_GET_ENUM(function, name, type) \
	BUS_DEFINE_PROPERTY_GET_REF(function, "s", type, name##_to_string)

/*

*/

#define _cleanup_sd_event_ _cleanup_(sd_event_unrefp)
#define _cleanup_sd_event_source_ _cleanup_(sd_event_source_unrefp)
#define _cleanup_sd_bus_ _cleanup_(sd_bus_unrefp)
#define _cleanup_sd_bus_slot_ _cleanup_(sd_bus_slot_unrefp)
#define _cleanup_sd_bus_message_ _cleanup_(sd_bus_message_unrefp)

/*

*/

#define ORCHESTRATOR_BUS_NAME "com.redhat.Orchestrator"
#define ORCHESTRATOR_OBJECT_PATH "/com/redhat/Orchestrator"
#define ORCHESTRATOR_NODES_OBJECT_PATH_PREFIX "/com/redhat/Orchestrator/node"
#define ORCHESTRATOR_JOBS_OBJECT_PATH_PREFIX "/com/redhat/Orchestrator/job"
#define ORCHESTRATOR_IFACE "com.redhat.Orchestrator"
#define ORCHESTRATOR_NODE_IFACE "com.redhat.Orchestrator.Node"
#define ORCHESTRATOR_PEER_IFACE "com.redhat.Orchestrator.Peer"

#define JOB_IFACE "com.redhat.Orchestrator.Job"

#define NODE_BUS_NAME "com.redhat.Orchestrator.Node"
#define NODE_PEER_OBJECT_PATH "/com/redhat/Orchestrator/Node"
#define NODE_PEER_JOBS_OBJECT_PATH_PREFIX "/com/redhat/Orchestrator/Node/job"
#define NODE_IFACE "com.redhat.Orchestrator.Node"
#define NODE_PEER_IFACE "com.redhat.Orchestrator.Node.Peer"

#define DEFAULT_DBUS_TIMEOUT (USEC_PER_SEC * 30)

#define SYSTEMD_BUS_NAME "org.freedesktop.systemd1"
#define SYSTEMD_OBJECT_PATH "/org/freedesktop/systemd1"
#define SYSTEMD_MANAGER_IFACE "org.freedesktop.systemd1.Manager"

#endif