#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <string>
#include <strings.h>

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

void fireman(int)
{
   while (waitpid(-1, NULL, WNOHANG) > 0)
      ;
}

int main(int argc, char *argv[])
{
   int sockfd, newsockfd, portno, clilen;
   struct sockaddr_in serv_addr, cli_addr;

   // Check the commandline arguments
   if (argc != 2)
   {
      std::cerr << "Port not provided" << std::endl;
      exit(0);
   }

   // Create the socket
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0)
   {
      std::cerr << "Error opening socket" << std::endl;
      exit(0);
   }

   // Populate the sockaddr_in structure
   bzero((char *)&serv_addr, sizeof(serv_addr));
   portno = atoi(argv[1]);
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);

   // Bind the socket with the sockaddr_in structure
   if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
   {
      std::cerr << "Error binding" << std::endl;
      exit(0);
   }

   // Set the max number of concurrent connections
   listen(sockfd, 5);
   clilen = sizeof(cli_addr);

   // Accept a new connection
   signal(SIGCHLD, fireman);
   while (true)
   {
      newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
      if (fork() == 0)
      {
         // Close the listening socket
         close(sockfd);
         if (newsockfd < 0)
         {
            std::cerr << "Error accepting new connections" << std::endl;
            exit(0);
         }

         infoFromMain info;
         int n = 0;
         n = read(newsockfd, &info, sizeof(infoFromMain));
         if (n < 0)
         {
            std::cerr << "Error reading from socket" << std::endl;
            exit(0);
         }

         cout << "infoFromMain's string: " << info.schedule << endl;
         cout << "infoFromMain's task size: " << info.tasks.size() << endl;
         cout << "infoFromMain's 1st task name: " << info.tasks.at(0).name << endl;

         string test = info.tasks.at(0).name;
         n = write(newsockfd, &test, sizeof(char));
         if (n < 0)
         {
            std::cerr << "Error writing to socket" << std::endl;
            exit(0);
         }
         close(newsockfd);
         exit(0);
      }
      else if (fork() < 0)
      {
         perror("Fork failed.");
         close(newsockfd);
         close(sockfd);
      }
      close(newsockfd);
   }
   return 0;
}
