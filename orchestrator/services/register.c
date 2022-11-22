#include "register.h"
#include "node.h"
#include "../../types/manager.h"
#include "../types/node.h"
#include "../types/orchestrator.h"
#include "../../util/dbus.h"

#include <stdio.h>

static int method_peer_orchestrator_register(sd_bus_message *m, void *userdata,
					     sd_bus_error *ret_error) {
	Node *node = userdata;
	Orchestrator *orch = node->orch;
	Manager *manager = (Manager *)orch;
	Node *existing;
	int r;
	char *name;
	char description[100];

	/* Read the parameters */
	r = sd_bus_message_read(m, "s", &name);
	if (r < 0) {
		fprintf(stderr, "Failed to parse parameters: %s\n",
			strerror(-r));
		return r;
	}

	if (node->name != NULL)
		return sd_bus_reply_method_errorf(
			m, SD_BUS_ERROR_ADDRESS_IN_USE, "Can't register twice");

	existing = orch_find_node(node->orch, name);
	if (existing != NULL)
		return sd_bus_reply_method_errorf(
			m, SD_BUS_ERROR_ADDRESS_IN_USE,
			"Node name already registered");

	node->name = strdup(name);
	if (node->name == NULL)
		return sd_bus_reply_method_errorf(m, SD_BUS_ERROR_NO_MEMORY,
						  "No memory");

	r = asprintf(&node->object_path, "%s/%s",
		     ORCHESTRATOR_NODES_OBJECT_PATH_PREFIX, name);
	if (r < 0)
		return sd_bus_reply_method_errorf(m, SD_BUS_ERROR_NO_MEMORY,
						  "No memory");

	strcpy(description, "node-");
	strncat(description, name,
		sizeof(description) - strlen(description) - 1);
	(void)sd_bus_set_description(node->peer, description);

	r = sd_bus_add_object_vtable(manager->bus, &node->bus_slot,
				     node->object_path, ORCHESTRATOR_NODE_IFACE,
				     node_vtable, node);
	if (r < 0) {
		fprintf(stderr, "Failed to add peer bus vtable: %s\n",
			strerror(-r));
		return EXIT_FAILURE;
	}

	printf("Registered node on fd %d as '%s'\n", sd_bus_get_fd(node->peer),
	       name);

	return sd_bus_reply_method_return(m, "");
}