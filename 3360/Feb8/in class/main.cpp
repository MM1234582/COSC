#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int x = 10;
    if (fork() != 0) // Parent waits
    {
        wait(nullptr);
    }
    x = x + 10;
    std::cout << "X = " << x << std::endl;

        return 0;
}