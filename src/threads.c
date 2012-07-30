/* threads.c */
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "threads.h"

void thread_init( void (*f)(int) )
{
	void server_thread ()
	{
		(*f)(1);
		pthread_exit(0);
	}
	void client_thread ()
	{
		(*f)(2);
		pthread_exit(0);
	}
	pthread_t thread1, thread2;

	pthread_create (&thread1, NULL, (void *) &server_thread, NULL);
	pthread_create (&thread2, NULL, (void *) &client_thread, NULL);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
}
