#include <iostream>
#include <unistd.h>

int main()
{
    for (int i = 0; i < 4; i++)
    {
        int pid = fork();
        if (pid == 0)
        {
            std::cout << "I am the child process " << i + 1 << std::endl;
            for (int j = 0; j < i; j++)
            {
                pid = fork();
                if (pid == 0)
                {
                    std::cout << "I am the grandchild process from child process " << i + 1 << std::endl;
                    sleep(5);
                    _exit(0);
                }
                wait(nullptr);
            }
            for (int j = 0; j < i; j++)
            {
                wait(nullptr);
            }
            sleep(5);
            _exit(0);
        }
    }
    for (int i = 0; i < 4; i++)
    {
        wait(nullptr);
    }
    sleep(5);
    _exit(0);

    return 0;
}