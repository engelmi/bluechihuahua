bs:
	gcc node/node.c types/manager.c types/enum.c -g -Wall -o orch-node `pkg-config --cflags --libs libsystemd`
	gcc client/client.c -g -Wall -o orch-client `pkg-config --cflags --libs libsystemd`
other:
	gcc orchestrator/vtable.c orchestrator/orchestrator.c -g -Wall -o orch `pkg-config --cflags --libs libsystemd`

build: build-service build-client

build-service:
	gcc service.c -g -Wall -o service `pkg-config --cflags --libs libsystemd`

build-client:
	gcc client.c -g -Wall -o client `pkg-config --cflags --libs libsystemd`

build-test:
	gcc test.c -g -Wall -o test `pkg-config --cflags --libs libsystemd`