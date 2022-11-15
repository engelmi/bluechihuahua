#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>

int main(int argc, char *argv[]) {
	sd_bus_error error = SD_BUS_ERROR_NULL;
	sd_bus_message *m = NULL;
	sd_bus *bus = NULL;
	const char *result;
	int r;

	/* Connect to the user bus */
	r = sd_bus_open_system_remote(&bus, "pi@192.168.178.73");
	if (r < 0) {
		fprintf(stderr, "Failed to connect to user bus: %s\n",
			strerror(-r));
		goto finish;
	}

	/* Issue the method call and store the respons message in m */
	r = sd_bus_call_method(bus,
			       "org.freedesktop.DBus", /* service to contact */
			       "/org/freedesktop/DBus", /* object path */
			       "org.freedesktop.DBus", /* interface name */
			       "GetId", /* method name */
			       &error, /* object to return error in */
			       &m, /* return message on success */
			       "");
	if (r < 0) {
		fprintf(stderr, "Failed to issue method call: %s\n",
			error.message);
		goto finish;
	}

	/* Parse the response message */
	r = sd_bus_message_read(m, "s", &result);
	if (r < 0) {
		fprintf(stderr, "Failed to parse response message: %s\n",
			strerror(-r));
		goto finish;
	}

	printf("%s.\n", result);

finish:
	sd_bus_error_free(&error);
	sd_bus_message_unref(m);
	sd_bus_unref(bus);

	return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}