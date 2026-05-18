#ifndef SMTPPROXY_H
#define SMTPPROXY_H

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

class SMTPProxy
{
private:
    int createListeningSocket(int port);
    int connectToSMTPServer(const std::string &host, int port);
    bool endsWithDisclaimerLine(const std::string &email);
    void handleClient(int clientSock, const std::string &smtpHost, int smtpPort);

    bool isBase64Email(const std::string &email);
    std::string processBase64Email(const std::string &email);

    std::string base64Decode(const std::string &input);
    std::string base64Encode(const std::string &input);
    std::string wrapBase64Lines(const std::string &input);

    std::string recvLineCRLF(int sock);
    int recvSMTPResponse(int sock, std::string &out);
    int parseCode(const char *buf);

    void sendAll(int sock, const std::string &data);

    std::string applyFirewallRules(const std::string &email);
    std::string replaceWords(const std::string &text);
    bool isWordChar(char c);
    bool matchesWordAt(const std::string &text, size_t pos, const std::string &word);
    bool containsForbiddenWord(const std::string &email);

public:
    void start(int listenPort, const std::string &smtpHost, int smtpPort);
};

#endif