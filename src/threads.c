// Beacontalk - peer to peer chat-program that sends data over Wi-Fi without associations.
// Copyright (C) 2012 Rick van de Loo

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
