#ifndef BLUECHIHUAHUA_TYPES
#define BLUECHIHUAHUA_TYPES

#include "enum.h"
#include "../util/list.h"

#include <systemd/sd-bus.h>

typedef struct Job Job;
typedef struct Manager Manager;

typedef int (*job_start_callback)(Job *job);
typedef int (*job_cancel_callback)(Job *job);
typedef void (*job_destroy_callback)(Job *job);

struct Job {
	int ref_count;
	int type;
	JobState state;
	JobResult result;
	Manager *manager;
	sd_bus_slot *bus_slot;
	uint32_t id;
	char *object_path;

	sd_bus_message *source_message;

	job_start_callback start_cb;
	job_cancel_callback cancel_cb;
	job_destroy_callback destroy_cb;

	LIST_FIELDS(Job, jobs);
};

extern Job *job_new(Manager *manager, int job_type, size_t job_size);
extern Job *job_ref(Job *job);
extern void job_unref(Job *job);
_SD_DEFINE_POINTER_CLEANUP_FUNC(Job, job_unref);

struct Manager {
	sd_event *event;
	sd_bus *bus;
	uint32_t next_job_id;
	char *job_path_prefix;
	char *manager_path;
	char *manager_iface;

	Job *current_job;
	sd_event_source *job_source;
	LIST_HEAD(Job, jobs);
};

int manager_queue_job(Manager *manager, int job_type, size_t job_size,
		      sd_bus_message *source_message,
		      job_start_callback start_cb,
		      job_cancel_callback cancel_cb,
		      job_destroy_callback destroy_cb, Job **job_out);
void manager_finish_job(Manager *manager, Job *job);

#endif
