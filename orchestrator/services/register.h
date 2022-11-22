#include <systemd/sd-bus-vtable.h>

static int method_peer_orchestrator_register(sd_bus_message *m, void *userdata,
					     sd_bus_error *ret_error);

static const sd_bus_vtable peer_orchestrator_vtable[] = {
	SD_BUS_VTABLE_START(0),
	SD_BUS_METHOD("Register", "s", "", method_peer_orchestrator_register,
		      0),
	SD_BUS_VTABLE_END
};