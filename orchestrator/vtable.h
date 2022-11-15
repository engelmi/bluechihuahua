#ifndef VTABLE_SERVICE
#define VTABLE_SERVICE

#include <systemd/sd-bus.h>

int method_multiply(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
int method_divide(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);

const sd_bus_vtable *get_vtable();

#endif