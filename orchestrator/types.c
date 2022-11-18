#include "types.h"
#include "../types/jobtracker.h"

static Node *node_new(Orchestrator *orch) {
	Node *node = malloc0(sizeof(Node));
	if (node) {
		node->orch = orch;
		node->ref_count = 1;
		LIST_INIT(nodes, node);
	}
	return node;
}

static Node *node_ref(Node *node) {
	node->ref_count++;
	return node;
}

static void node_unref(Node *node) {
	node->ref_count--;

	if (node->ref_count == 0) {
		if (node->peer)
			sd_bus_flush_close_unref(node->peer);
		if (node->bus_slot)
			sd_bus_slot_unref(node->bus_slot);
		if (node->name)
			free(node->name);
		if (node->object_path)
			free(node->object_path);
		free(node);
	}
}
_SD_DEFINE_POINTER_CLEANUP_FUNC(Node, node_unref);

static void node_add_job_tracker(Node *node, JobTracker *tracker,
				 const char *object_path,
				 job_tracker_callback callback,
				 void *userdata) {
	tracker->object_path = object_path;
	tracker->callback = callback;
	tracker->userdata = userdata;
	LIST_PREPEND(trackers, node->trackers, tracker);
}

static int orch_get_n_nodes(Orchestrator *orch) {
	Node *node;
	int n_nodes = 0;

	LIST_FOREACH(nodes, node, orch->nodes)
	n_nodes++;

	return n_nodes;
}

static void orch_add_node(Orchestrator *orch, Node *node) {
	LIST_APPEND(nodes, orch->nodes, node_ref(node));
}

static void orch_remove_node(Orchestrator *orch, Node *node) {
	LIST_REMOVE(nodes, orch->nodes, node);
	node_unref(node);
}

static Node *orch_find_node(Orchestrator *orch, const char *name) {
	Node *node;

	LIST_FOREACH(nodes, node, orch->nodes) {
		if (node->name != NULL && strcmp(node->name, name) == 0)
			return node;
	}

	return NULL;
}
