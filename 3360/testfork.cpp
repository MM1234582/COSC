#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cmath>

using namespace std;

int main()
{
    cout << "I am the parent process" << endl;
    for (int i = 0; i < 3; i++)
    {
        if (fork() == 0)
        {
            cout << "\tI am the child process " << i << endl;
            for (int j = 0; j < 2 * (i + 1); j++)
            {
                if (fork() == 0)
                {
                    cout << "\t\tI am a granchild process from child process " << i << endl;
                    _exit(0);
                }
                wait(nullptr);
            }
            _exit(0);
        }
        wait(nullptr);
    }
    return 0;
}