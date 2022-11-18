#ifndef BLUECHIHUAHUA_ORCH_TYPES
#define BLUECHIHUAHUA_ORCH_TYPES

#include "../types/manager.h"
#include "../types/jobtracker.h"

#include <systemd/sd-bus.h>

typedef struct Orchestrator Orchestrator;
typedef struct Node Node;

struct Orchestrator {
	Manager manager;
	LIST_HEAD(Node, nodes);
};

struct Node {
	int ref_count;
	Orchestrator *orch;
	sd_bus *peer;
	sd_bus_slot *bus_slot;
	char *name;
	char *object_path;
	LIST_FIELDS(Node, nodes);
	LIST_HEAD(JobTracker, trackers);
};

#endif
