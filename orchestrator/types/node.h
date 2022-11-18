#ifndef BLUECHIHUAHUA_ORCH_TYPES_NODE
#define BLUECHIHUAHUA_ORCH_TYPES_NODE

#include "../../util/list.h"

#include <systemd/sd-bus.h>

typedef struct Node Node;

struct Node {
	int ref_count;
	sd_bus *peer;
	sd_bus_slot *bus_slot;
	char *name;
	char *object_path;

	LIST_FIELDS(Node, nodes);
	LIST_HEAD(JobTracker, trackers);
};

#endif