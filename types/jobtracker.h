#ifndef BLUECHIHUAHUA_TYPES_JOBTRACKER
#define BLUECHIHUAHUA_TYPES_JOBTRACKER

#include "../util/list.h"

typedef void (*job_tracker_callback)(sd_bus_message *m, const char *result,
				     void *userdata);

typedef struct JobTracker JobTracker;

struct JobTracker {
	const char *object_path;
	job_tracker_callback callback;
	void *userdata;
	LIST_FIELDS(JobTracker, trackers);
};

#endif