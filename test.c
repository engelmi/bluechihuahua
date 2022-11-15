#include <alloca.h>
#include <endian.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <systemd/sd-daemon.h>
#include <systemd/sd-event.h>

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

static int io_handler(sd_event_source *es, int fd, uint32_t revents,
		      void *userdata) {
	void *buffer;
	ssize_t n;
	int sz;

	/* UDP enforces a somewhat reasonable maximum datagram size of 64K, we can just allocate the buffer on the stack */
	if (ioctl(fd, FIONREAD, &sz) < 0)
		return -errno;
	buffer = alloca(sz);

	n = recv(fd, buffer, sz, 0);
	if (n < 0) {
		if (errno == EAGAIN)
			return 0;

		return -errno;
	}

	if (n == 5 && memcmp(buffer, "EXIT\n", 5) == 0) {
		/* Request a clean exit */
		sd_event_exit(sd_event_source_get_event(es), 0);
		return 0;
	}

	fwrite(buffer, 1, n, stdout);
	fflush(stdout);
	return 0;
}

int main(int argc, char *argv[]) {
	union {
		struct sockaddr_in in;
		struct sockaddr sa;
	} sa;
	sd_event_source *event_source = NULL;
	sd_event *event = NULL;
	int fd = -1, r;
	sigset_t ss;

	r = sd_event_default(&event);
	if (r < 0)
		goto finish;

	if (sigemptyset(&ss) < 0 || sigaddset(&ss, SIGTERM) < 0 ||
	    sigaddset(&ss, SIGINT) < 0) {
		r = -errno;
		goto finish;
	}

	/* Block SIGTERM first, so that the event loop can handle it */
	if (sigprocmask(SIG_BLOCK, &ss, NULL) < 0) {
		r = -errno;
		goto finish;
	}

	/* Let's make use of the default handler and "floating" reference features of sd_event_add_signal() */
	r = sd_event_add_signal(event, NULL, SIGTERM, NULL, NULL);
	if (r < 0)
		goto finish;
	r = sd_event_add_signal(event, NULL, SIGINT, NULL, NULL);
	if (r < 0)
		goto finish;

	/* Enable automatic service watchdog support */
	r = sd_event_set_watchdog(event, true);
	if (r < 0)
		goto finish;

	fd = create_master_socket(7777);
	if (fd < 0) {
		r = -errno;
		goto finish;
	}

	r = sd_event_add_io(event, &event_source, fd, EPOLLIN, io_handler,
			    NULL);
	if (r < 0)
		goto finish;

	(void)sd_notifyf(false,
			 "READY=1\n"
			 "STATUS=Daemon startup completed, processing events.");

	r = sd_event_loop(event);

finish:
	event_source = sd_event_source_unref(event_source);
	event = sd_event_unref(event);

	if (fd >= 0)
		(void)close(fd);

	if (r < 0)
		fprintf(stderr, "Failure: %s\n", strerror(-r));

	return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
