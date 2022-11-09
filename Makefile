build: build-service build-client

build-service:
	gcc service.c -g -Wall -o service `pkg-config --cflags --libs libsystemd`

build-client:
	gcc client.c -g -Wall -o client `pkg-config --cflags --libs libsystemd`