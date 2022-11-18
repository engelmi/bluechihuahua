#include "orchestrator.h"

static void orch_append_node(Orchestrator *orch, node *Node) {
	if (node) {
		LIST_INIT(nodes, node);
	}
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