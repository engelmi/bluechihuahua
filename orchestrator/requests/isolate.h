#ifndef BLUECHIHUAHUA_ORCH_REQUESTS_ISOLATE
#define BLUECHIHUAHUA_ORCH_REQUESTS_ISOLATE

#include "../types/node.h"
#include "../../types/enum.h"
#include "../../types/manager.h"
#include "../../types/jobtracker.h"

typedef struct {
	Job *job;
	JobResult result;
	JobTracker tracker;

	Node *node;
	sd_bus_message *request;
	sd_bus_slot *request_slot;
	sd_bus_message *reply;
	const char *job_object_path;
} IsolateRequest;

#endif