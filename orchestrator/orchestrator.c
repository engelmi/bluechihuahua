#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <systemd/sd-bus.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "vtable.h"
#include "../util/consts.h"

int create_master_socket(int port) {
	int fd;
	struct sockaddr_in servaddr;

	fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
	if (fd < 0) {
		int errsv = errno;
		fprintf(stderr, "Failed to create socket: %m\n");
		return -errsv;
	}

	int yes = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		int errsv = errno;
		fprintf(stderr, "Failed to create socket: %m\n");
		return -errsv;
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	if (bind(fd, &servaddr, sizeof(servaddr)) < 0) {
		int errsv = errno;
		fprintf(stderr, "Failed to bind socket: %m\n");
		return -errsv;
	}

	if ((listen(fd, SOMAXCONN)) != 0) {
		int errsv = errno;
		fprintf(stderr, "Failed to listed socket: %m\n");
		return -errsv;
	}

	return fd;
}

static int accept_handler(sd_event_source *s, int fd, uint32_t revents,
			  void *userdata) {
	int nfd = -1;

	fprintf(stdout, "fuuuck\n");
	nfd = accept4(fd, NULL, NULL, SOCK_NONBLOCK | SOCK_CLOEXEC);
	if (nfd < 0) {
		if (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK)
			return 0;
		else {
			int errsv = errno;
			fprintf(stderr, "Failed to accept: %m\n");
			return -errsv;
		}
	}
	return 1;
}

int main(int argc, char *argv[]) {
	sd_bus_slot *slot = NULL;
	sd_bus *bus = NULL;
	sd_event_source *event_source = NULL;
	int r;

	/* Connect to the system bus this time */
	r = sd_bus_open_user(&bus);
	if (r < 0) {
		fprintf(stderr, "Failed to connect to system bus: %s\n",
			strerror(-r));
		goto finish;
	}

	/* Install the object */
	r = sd_bus_add_object_vtable(
		bus, &slot, "/com/redhat/Calculator", /* object path */
		"com.redhat.Calculator", /* interface name */
		get_vtable(), NULL);
	if (r < 0) {
		fprintf(stderr, "Failed to issue method call: %s\n",
			strerror(-r));
		goto finish;
	}

	/* Take a well-known service name so that clients can find us */
	r = sd_bus_request_name(bus, "com.redhat.Calculator", 0);
	if (r < 0) {
		fprintf(stderr, "Failed to acquire service name: %s\n",
			strerror(-r));
		goto finish;
	}

	int accept_fd = -1;
	accept_fd = create_master_socket(1999);
	if (accept_fd < 0) {
		return EXIT_FAILURE;
	}

	sd_event *event = NULL;
	r = sd_event_default(&event);
	if (r < 0) {
		fprintf(stderr, "Failed to create event: %s\n", strerror(-r));
		return EXIT_FAILURE;
	}

	r = sd_bus_attach_event(bus, event, SD_EVENT_PRIORITY_NORMAL);
	if (r < 0) {
		fprintf(stderr, "Failed to attach bus to event: %s\n",
			strerror(-r));
		return EXIT_FAILURE;
	}

	r = sd_event_add_io(event, &event_source, accept_fd, EPOLLIN,
			    accept_handler, "test");
	if (r < 0) {
		fprintf(stderr, "Failed to add io event: %s\n", strerror(-r));
		return EXIT_FAILURE;
	}

	r = sd_event_loop(event);
	if (r < 0) {
		fprintf(stderr, "Event loop failed: %s\n", strerror(-r));
		return EXIT_FAILURE;
	}

finish:
	sd_bus_slot_unref(slot);
	sd_bus_unref(bus);

	return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}