#include <unistd.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

struct task
{
    char name;
    int WCET;
    int period;
    int initWCET;
    task(char id, int worst, int per)
    {
        name = id;
        WCET = worst;
        initWCET = worst;
        period = per;
    }
};

struct infoFromMain
{
    std::vector<task> tasks;
    double *utilization;
    int hyperperiod;
    string schedule;
};

class TaskInterval
{
public:
    char name;
    int startTime;
    int endTime;
    bool stopped;
};

int main(int argc, char *argv[])
{
    vector<infoFromMain> input;
    string line;
    /*
    while (getline(cin, line))
    {
        stringstream ss(line);
        char name;
        int worst;
        int per;

        infoFromMain tempInfo;

        // Gather input data
        while (ss >> name >> worst >> per)
        {
            task temp = task(name, worst, per);

            tempInfo.tasks.push_back(temp);
        }
        input.push_back(tempInfo);
    }
    */

    char name = 'D';
    int worst = 7;
    int per = 10;
    task temp = task(name, worst, per);

    cout << "Task Info:" << endl;
    cout << "\tName: " << temp.name << endl;
    cout << "\tWorst: " << temp.WCET << endl;
    cout << "\tPer: " << temp.period << endl;
    infoFromMain tempInfo;
    tempInfo.tasks.push_back(temp);
    tempInfo.schedule = "Testing!";

    int sockfd, portno, n;
    std::string buffer;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc != 3)
    {
        std::cerr << "usage " << argv[0] << " hostname port" << std::endl;
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "ERROR opening socket" << std::endl;
        exit(0);
    }
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        std::cerr << "ERROR, no such host" << std::endl;
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR connecting" << std::endl;
        exit(0);
    }

    n = write(sockfd, &tempInfo, sizeof(infoFromMain));
    if (n < 0)
    {
        std::cerr << "ERROR writing to socket" << std::endl;
        exit(0);
    }

    char test;

    n = read(sockfd, &test, sizeof(char));
    if (n < 0)
    {
        std::cerr << "ERROR reading from socket" << std::endl;
        exit(0);
    }
    close(sockfd);

    std::cout << test << std::endl;
    return 0;
}
