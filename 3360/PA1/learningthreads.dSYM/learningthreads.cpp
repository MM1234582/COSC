#include <iostream>
#include <string>
#include <unistd.h> //Fork?

using namespace std;

/*
    Fork allows us to create a copy image of our process
    Child process doesn't start from the beginning?

    Needs to be paired with another function, sleep()
*/
int main()
{
    int i = 0;
    fork();

    i++;

    cout << i << endl;

    return 0;
}