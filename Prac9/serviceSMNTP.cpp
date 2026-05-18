#include "serviceSMNTP.h"

int serviceSMNTP::parseCode(const char *buf)
{
    // Function to check if there is a success code else throws an exception
    if (!isdigit((unsigned char)buf[0]) || !isdigit((unsigned char)buf[1]) || !isdigit((unsigned char)buf[2]))
        throw std::runtime_error("invalid SMTP response");

    return (buf[0] - '0') * 100 + (buf[1] - '0') * 10 + (buf[2] - '0');
}

bool serviceSMNTP::verifyUser(int sock, const std::string &user, std::string &response)
{
    std::string msg = "VRFY " + user + "\r\n";
    send(sock, msg.c_str(), msg.length(), 0);

    int code = recvSMTPResponse(sock, response);

    if (code == 250 || code == 251 || code == 252)
    {
        return true;
    }

    return false;
}

std::string serviceSMNTP::recvLineCRLF(int sock)
{
    // Function to receive response from SMTP SERVER and checks the size to see if valid response for error handling
    std::string s;
    char c;

    while (true)
    {
        ssize_t r = recv(sock, &c, 1, 0);
        if (r <= 0)
            throw std::runtime_error("recv error/closed");

        s.push_back(c);

        if (s.size() >= 2 && s.substr(s.size() - 2) == "\r\n")
            break;
    }

    return s;
}

int serviceSMNTP::recvSMTPResponse(int sock, std::string &out)
{
    // Will check if the response is correct for that command
    out = recvLineCRLF(sock);

    if (out.size() < 4)
        throw std::runtime_error("short SMTP response");

    int code = parseCode(out.c_str());
    char sep = out[3];

    if (sep == '-')
    {
        std::string line;
        do
        {
            line = recvLineCRLF(sock);
            out += line;
        } while (!(line.size() >= 4 && parseCode(line.c_str()) == code && line[3] == ' '));
    }

    return code;
}

bool serviceSMNTP::isPositiveCompletion(int code)
{
    return code >= 200 && code < 300;
}

bool serviceSMNTP::isPositiveIntermediate(int code)
{
    return code >= 300 && code < 400;
}

bool serviceSMNTP::isTransientNegative(int code)
{
    return code >= 400 && code < 500;
}

bool serviceSMNTP::isPermanentNegative(int code)
{
    return code >= 500 && code < 600;
}

bool serviceSMNTP::codeIn(int code, const std::vector<int> &allowed)
{
    for (size_t i = 0; i < allowed.size(); ++i)
    {
        if (allowed[i] == code)
            return true;
    }

    return false;
}

void serviceSMNTP::expectReply(int code, const std::vector<int> &allowed, const std::string &action, const std::string &response)
{
    if (codeIn(code, allowed))
        return;

    if (isTransientNegative(code))
    {
        throw std::runtime_error(action + " temporary failure: " + response);
    }

    if (isPermanentNegative(code))
    {
        throw std::runtime_error(action + " permanent failure: " + response);
    }

    throw std::runtime_error(action + " unexpected reply: " + response);
}

void serviceSMNTP::sendEmail(const std::string &user, const std::vector<std::string> &events)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(25); // SMTP Port
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Connection to SMTP server failed. Is Postfix running?" << std::endl;
        return;
    }

    std::string response;
    int code = recvSMTPResponse(sock, response);
    expectReply(code, std::vector<int>(1, 220), "server greeting", response);

    std::string msg = "HELO localhost\r\n"; // The SMTP Handshake
    send(sock, msg.c_str(), msg.length(), 0);
    code = recvSMTPResponse(sock, response);
    expectReply(code, std::vector<int>(1, 250), "HELO", response);

    std::string vrfyResponse = "";
    bool vrfyOk = verifyUser(sock, user, vrfyResponse);

    if (vrfyOk)
    {
        std::cout << "VRFY response: " << vrfyResponse;
    }
    else
    {
        std::cout << "VRFY failed or disabled: " << vrfyResponse;
    }

    msg = "MAIL FROM:<" + user + "@localhost>\r\n";
    send(sock, msg.c_str(), msg.length(), 0);
    code = recvSMTPResponse(sock, response);
    expectReply(code, std::vector<int>(1, 250), "MAIL FROM", response);

    msg = "RCPT TO:<" + user + "@localhost>\r\n";
    send(sock, msg.c_str(), msg.length(), 0);
    code = recvSMTPResponse(sock, response);
    expectReply(code, std::vector<int>(1, 250), "RCPT TO", response);

    msg = "DATA\r\n";
    send(sock, msg.c_str(), msg.length(), 0);
    code = recvSMTPResponse(sock, response);
    expectReply(code, std::vector<int>(1, 354), "DATA", response);

    // Construct the email body
    std::string content = "Subject: Event Reminder\r\n\r\n";
    content += "Events happening in 6 days:\n";

    for (const auto &e : events)
        content += "- " + e + "\n";

    content += "\r\n.\r\n"; // The period ends the DATA section

    msg = content;
    send(sock, msg.c_str(), msg.length(), 0);
    code = recvSMTPResponse(sock, response);
    expectReply(code, std::vector<int>(1, 250), "email content", response);

    msg = "QUIT\r\n";
    send(sock, msg.c_str(), msg.length(), 0);
    code = recvSMTPResponse(sock, response);
    expectReply(code, std::vector<int>(1, 221), "QUIT", response);

    close(sock);
}
