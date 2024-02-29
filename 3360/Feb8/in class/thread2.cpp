#include <pthread.h>
#include <iostream>
#include <sys/types.h>

void *increment(void *ptr)
{
	// We're receiving a pointer to void, and we can only receive it if we're casting it to a usable data type
	// Doing this way allows us to cast anything we want

	// This is basically the code for the child thread, because we join the thread after
	int *int_ptr = (int *)ptr;

	*int_ptr = *int_ptr + 10;

	return nullptr;
}

int main()
{
	pthread_t tid[100];
	static int x = 10;

	for (int i = 0; i < 10; i++)
	{
		/*
		First parameter is the thread
		Second is the place in memory to store, nullptr used to let it automatically assign
		Third is the function call
		Fourth is the parameter for the function
		*/
		if (pthread_create(&tid[i], nullptr, increment, (void *)&x) != 0) // If pthread_create() returns 0, it succeeds. If not, there's an error somewhere
		{
			std::cerr << "error creating thread" << std::endl;
			exit(1);
		}
		/*

		Uncommenting the line below will give us the values we expect, but this will not be a multithreaded application.
		This would be the WRONG approach

		*/
		// pthread_join(tid[i], nullptr);
	}

	/*

	The loop below would be the proper solution, but because we don't have any synchronization mechanisms, we're getting process
	interference. This means that some processes are trying to execute at the same time

	Threads share memory, but can have interference
	Processes do NOT share memory

	*/
	for (int i = 0; i < 10; i++)
	{
		pthread_join(tid[i], nullptr);
	}

	/*
	On the exam, we will put wait() in the same loop that we're calling fork() so we can get the same
	results every time.
	*/

	x = x + 10;

	std::cout << "X = " << x << std::endl;

	return 0;
}
