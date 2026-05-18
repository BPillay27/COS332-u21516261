#ifndef SERVICESMNTP_H
#define SERVICESMNTP_H

#include <string>
#include <vector>
#include <arpa/inet.h>
#include <cctype>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

class serviceSMNTP
{
private:
    int parseCode(const char *buf);
    bool verifyUser(int sock, const std::string &user, std::string &response);
    std::string recvLineCRLF(int sock);
    int recvSMTPResponse(int sock, std::string &out);
    bool isPositiveCompletion(int code);
    bool isPositiveIntermediate(int code);
    bool isTransientNegative(int code);
    bool isPermanentNegative(int code);
    bool codeIn(int code, const std::vector<int> &allowed);
    void expectReply(int code, const std::vector<int> &allowed, const std::string &action, const std::string &response);

public:
    void sendEmail(const std::string &user, const std::vector<std::string> &events);
};

#endif
