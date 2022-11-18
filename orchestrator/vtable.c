#include "vtable.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <systemd/sd-bus.h>

int method_multiply(sd_bus_message *m, void *userdata,
		    sd_bus_error *ret_error) {
	int64_t x, y;
	int r;

	/* Read the parameters */
	r = sd_bus_message_read(m, "xx", &x, &y);
	if (r < 0) {
		fprintf(stderr, "Failed to parse parameters: %s\n",
			strerror(-r));
		return r;
	}

	fprintf(stderr, "multiplying %ldx%ld", x, y);
	/* Reply with the response */
	return sd_bus_reply_method_return(m, "x", x * y);
}

int method_divide(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
	int64_t x, y;
	int r;

	/* Read the parameters */
	r = sd_bus_message_read(m, "xx", &x, &y);
	if (r < 0) {
		fprintf(stderr, "Failed to parse parameters: %s\n",
			strerror(-r));
		return r;
	}

	/* Return an error on division by zero */
	if (y == 0) {
		sd_bus_error_set_const(ret_error,
				       "net.poettering.DivisionByZero",
				       "Sorry, can't allow division by zero.");
		return -EINVAL;
	}

	return sd_bus_reply_method_return(m, "x", x / y);
}

const sd_bus_vtable *get_vtable() {
	static const sd_bus_vtable calculator_vtable[] = {
		SD_BUS_VTABLE_START(0),
		SD_BUS_METHOD("Multiply", "xx", "x", method_multiply,
			      SD_BUS_VTABLE_UNPRIVILEGED),
		SD_BUS_METHOD("Divide", "xx", "x", method_divide,
			      SD_BUS_VTABLE_UNPRIVILEGED),
		SD_BUS_VTABLE_END
	};
	return calculator_vtable;
}
