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
	pthread_t tid;
	static int x = 10;
	/*
	First parameter is the thread
	Second is the place in memory to store, nullptr used to let it automatically assign
	Third is the function call
	Fourth is the parameter for the function
	*/
	if (pthread_create(&tid, nullptr, increment, (void *)&x) != 0) // If pthread_create() returns 0, it succeeds. If not, there's an error somewhere
	{
		std::cerr << "error creating thread" << std::endl;
		exit(1);
	}
	pthread_join(tid, nullptr);

	x = x + 10;

	std::cout << "X = " << x << std::endl;

	return 0;
}
