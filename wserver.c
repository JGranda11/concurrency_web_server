#include <stdio.h>
#include <stdlib.h>
#include "request.h"
#include "io_helper.h"
#include <pthread.h>
#include "pool_request.h"

char default_root[] = ".";

double get_seconds()
{
	struct timeval t;
	int rc = gettimeofday(&t, NULL);
	assert(rc == 0);
	return (double)((double)t.tv_sec + (double)t.tv_usec / 1e6);
}
//
// ./wserver [-d <basedir>] [-p <portnum>]
//

static buffer_t request_buffer;

void *routine_thread(void *arg)
{
	while (1)
	{
		int conn_fd = borrar(&request_buffer);
		double time1 = get_seconds();
		while ((get_seconds() - time1) < 3)
		{
			sleep(1);
		}
		request_handle(conn_fd);
		close_or_die(conn_fd);
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	int c;
	char *root_dir = default_root;
	int port = 10000;
	int num_threads = 1;
	int cap_buffer = 1;

	while ((c = getopt(argc, argv, "d:p:t:b:")) != -1)
		switch (c)
		{
		case 'd':
			root_dir = optarg;
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case 't':
			num_threads = atoi(optarg);
			break;
		case 'b':
			cap_buffer = atoi(optarg);
			break;
		default:
			fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-t # threads] [-b buffers-capacity]\n");
			exit(1);
		}

	// run out of this directory
	chdir_or_die(root_dir);

	// now, get to work
	int listen_fd = open_listen_fd_or_die(port);

	inicializar(&request_buffer, cap_buffer);

	pthread_t *thread_pool = malloc(sizeof(pthread_t) * num_threads);

	for (int i = 0; i < num_threads; i++)
	{
		int resultado = pthread_create(&thread_pool[i], NULL, routine_thread, NULL);
		if (resultado != 0)
		{
			fprintf(stderr, "Error , no creo %d \n", i);
			exit(1);
		}
	}

	while (1)
	{
		struct sockaddr_in client_addr;
		int client_len = sizeof(client_addr);
		int conn_fd = accept_or_die(listen_fd, (sockaddr_t *)&client_addr, (socklen_t *)&client_len);

		insertar(&request_buffer, conn_fd);
	}

	destruir(&request_buffer);

	free(thread_pool);
	return 0;
}
