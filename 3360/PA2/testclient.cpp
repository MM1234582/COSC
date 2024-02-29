#include <unistd.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

struct student
{
    char name[50];
    int psid;
    int grade;
};

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    std::string buffer;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    student st;
    std::string name2 = "David";
    for (int i = 0; i < name2.size(); i++)
    {
        st.name[i] = name2[i];
    }
    st.psid = 12345;
    st.grade = 53;

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

    int msgSize = sizeof(st);

    n = write(sockfd, &st, sizeof(student));
    if (n < 0)
    {
        std::cerr << "ERROR writing to socket" << std::endl;
        exit(0);
    }
    /*
    n = write(sockfd, buffer.c_str(), msgSize);
    if (n < 0)
    {
        std::cerr << "ERROR writing to socket" << std::endl;
        exit(0);
    }
    */
    char test;

    n = read(sockfd, &test, sizeof(char));
    if (n < 0)
    {
        std::cerr << "ERROR reading from socket" << std::endl;
        exit(0);
    }
    /*
    char *tempBuffer = new char[msgSize + 1];
    bzero(tempBuffer, msgSize + 1);
    n = read(sockfd, tempBuffer, msgSize);
    if (n < 0)
    {
        std::cerr << "ERROR reading from socket" << std::endl;
        exit(0);
    }
    buffer = tempBuffer;
    delete[] tempBuffer;

    std::cout << "Message from server: " << buffer << std::endl;
    */
    close(sockfd);

    std::cout << test << std::endl;
    return 0;
}
