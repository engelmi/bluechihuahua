#ifndef BLUECHIHUAHUA_ORCH_TYPES_ORCH
#define BLUECHIHUAHUA_ORCH_TYPES_ORCH

#include "node.h"
#include "../types/manager.h"

typedef struct Orchestrator Orchestrator;

struct Orchestrator {
	Manager manager;
	LIST_HEAD(Node, nodes);
};

#endif