#include "Proxy.h"

bool SMTPProxy::isBase64Email(const std::string &email)
{
    std::string lower;

    for (size_t i = 0; i < email.length(); i++)
    {
        lower += std::tolower((unsigned char)email[i]);
    }

    return lower.find("content-transfer-encoding: base64") != std::string::npos;
}

int SMTPProxy::parseCode(const char *buf)
{
    if (!isdigit((unsigned char)buf[0]) ||
        !isdigit((unsigned char)buf[1]) ||
        !isdigit((unsigned char)buf[2]))
    {
        throw std::runtime_error("Invalid SMTP response");
    }

    return (buf[0] - '0') * 100 + (buf[1] - '0') * 10 + (buf[2] - '0');
}

void SMTPProxy::sendAll(int sock, const std::string &data)
{
    size_t totalSent = 0;

    while (totalSent < data.length())
    {
        ssize_t sent = send(
            sock,
            data.c_str() + totalSent,
            data.length() - totalSent,
            0);

        if (sent <= 0)
        {
            throw std::runtime_error("send failed");
        }

        totalSent += sent;
    }
}

std::string SMTPProxy::recvLineCRLF(int sock)
{
    std::string line;
    char c;

    while (true)
    {
        ssize_t r = recv(sock, &c, 1, 0);

        if (r == 0)
        {
            throw std::runtime_error("connection closed");
        }

        if (r < 0)
        {
            throw std::runtime_error("recv failed");
        }

        line.push_back(c);

        if (line.size() >= 2 &&
            line[line.size() - 2] == '\r' &&
            line[line.size() - 1] == '\n')
        {
            break;
        }
    }

    return line;
}

int SMTPProxy::recvSMTPResponse(int sock, std::string &out)
{
    out = recvLineCRLF(sock);

    if (out.size() < 4)
    {
        throw std::runtime_error("short SMTP response");
    }

    int code = parseCode(out.c_str());
    char sep = out[3];

    if (sep == '-')
    {
        std::string line;

        do
        {
            line = recvLineCRLF(sock);
            out += line;
        } while (!(line.size() >= 4 &&
                   parseCode(line.c_str()) == code &&
                   line[3] == ' '));
    }

    return code;
}

int SMTPProxy::createListeningSocket(int port)
{
    int listenSock = socket(AF_INET, SOCK_STREAM, 0);

    if (listenSock < 0)
    {
        throw std::runtime_error("could not create listening socket");
    }

    int opt = 1;
    setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listenSock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        close(listenSock);
        throw std::runtime_error("bind failed");
    }

    if (listen(listenSock, 5) < 0)
    {
        close(listenSock);
        throw std::runtime_error("listen failed");
    }

    return listenSock;
}

int SMTPProxy::connectToSMTPServer(const std::string &host, int port)
{
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSock < 0)
    {
        throw std::runtime_error("could not create SMTP server socket");
    }

    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr) <= 0)
    {
        close(serverSock);
        throw std::runtime_error("invalid SMTP server address");
    }

    if (connect(serverSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        close(serverSock);
        throw std::runtime_error("could not connect to SMTP server");
    }

    return serverSock;
}

bool SMTPProxy::isWordChar(char c)
{
    return std::isalnum((unsigned char)c) || c == '_';
}

bool SMTPProxy::matchesWordAt(const std::string &text, size_t pos, const std::string &word)
{
    if (pos + word.length() > text.length())
    {
        return false;
    }

    // Case-insensitive comparison
    for (size_t i = 0; i < word.length(); i++)
    {
        char textChar = std::tolower((unsigned char)text[pos + i]);
        char wordChar = std::tolower((unsigned char)word[i]);

        if (textChar != wordChar)
        {
            return false;
        }
    }

    if (pos > 0 && isWordChar(text[pos - 1]))
    {
        return false;
    }

    if (pos + word.length() < text.length() && isWordChar(text[pos + word.length()]))
    {
        return false;
    }

    return true;
}

std::string SMTPProxy::replaceWords(const std::string &text)
{
    std::string result;
    size_t i = 0;

    while (i < text.length())
    {
        // Phrase replacements must come first
        if (matchesWordAt(text, i, "very good"))
        {
            result += "plusgood";
            i += 9;
        }
        else if (matchesWordAt(text, i, "very fast"))
        {
            result += "plusfast";
            i += 9;
        }
        else if (matchesWordAt(text, i, "very bad"))
        {
            result += "plusungood";
            i += 8;
        }

        // Single-word replacements
        else if (matchesWordAt(text, i, "better"))
        {
            result += "gooder";
            i += 6;
        }
        else if (matchesWordAt(text, i, "rapid"))
        {
            result += "speedful";
            i += 5;
        }
        else if (matchesWordAt(text, i, "quick"))
        {
            result += "speedful";
            i += 5;
        }
        else if (matchesWordAt(text, i, "stole"))
        {
            result += "stealed";
            i += 5;
        }
        else if (matchesWordAt(text, i, "warm"))
        {
            result += "uncold";
            i += 4;
        }
        else if (matchesWordAt(text, i, "fast"))
        {
            result += "speedful";
            i += 4;
        }
        else if (matchesWordAt(text, i, "slow"))
        {
            result += "unspeedful";
            i += 4;
        }
        else if (matchesWordAt(text, i, "best"))
        {
            result += "goodest";
            i += 4;
        }
        else if (matchesWordAt(text, i, "bad"))
        {
            result += "ungood";
            i += 3;
        }
        else if (matchesWordAt(text, i, "ran"))
        {
            result += "runned";
            i += 3;
        }
        else
        {
            result += text[i];
            i++;
        }
    }

    return result;
}

bool SMTPProxy::endsWithDisclaimerLine(const std::string &email)
{
    const std::string disclaimer = "Please do not take anything in this email seriously!";

    std::string temp = email;

    // Remove trailing CR/LF characters only.
    while (!temp.empty() && (temp[temp.length() - 1] == '\r' || temp[temp.length() - 1] == '\n'))
    {
        temp.erase(temp.length() - 1);
    }

    if (temp.length() < disclaimer.length())
    {
        return false;
    }

    size_t start = temp.length() - disclaimer.length();

    if (temp.substr(start) != disclaimer)
    {
        return false;
    }

    if (start == 0)
    {
        return true;
    }

    char before = temp[start - 1];

    return before == '\n' || before == '\r';
}

bool SMTPProxy::containsForbiddenWord(const std::string &email)
{
    for (size_t i = 0; i < email.length(); i++)
    {
        if (matchesWordAt(email, i, "Illuminati"))
        {
            return true;
        }
    }

    return false;
}

std::string SMTPProxy::base64Decode(const std::string &input)
{
    const std::string chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::string cleanInput;

    for (size_t i = 0; i < input.length(); i++)
    {
        char c = input[i];

        if (std::isalnum((unsigned char)c) || c == '+' || c == '/' || c == '=')
        {
            cleanInput += c;
        }
    }

    std::string output;
    int val = 0;
    int valb = -8;

    for (size_t i = 0; i < cleanInput.length(); i++)
    {
        char c = cleanInput[i];

        if (c == '=')
        {
            break;
        }

        size_t pos = chars.find(c);

        if (pos == std::string::npos)
        {
            continue;
        }

        val = (val << 6) + (int)pos;
        valb += 6;

        if (valb >= 0)
        {
            output.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }

    return output;
}

std::string SMTPProxy::base64Encode(const std::string &input)
{
    const std::string chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::string output;
    int val = 0;
    int valb = -6;

    for (size_t i = 0; i < input.length(); i++)
    {
        unsigned char c = input[i];

        val = (val << 8) + c;
        valb += 8;

        while (valb >= 0)
        {
            output.push_back(chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }

    if (valb > -6)
    {
        output.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }

    while (output.length() % 4)
    {
        output.push_back('=');
    }

    return output;
}

std::string SMTPProxy::wrapBase64Lines(const std::string &input)
{
    std::string output;

    for (size_t i = 0; i < input.length(); i++)
    {
        output += input[i];

        if ((i + 1) % 76 == 0)
        {
            output += "\r\n";
        }
    }

    if (output.length() < 2 ||
        output.substr(output.length() - 2) != "\r\n")
    {
        output += "\r\n";
    }

    return output;
}

std::string SMTPProxy::processBase64Email(const std::string &email)
{
    size_t splitPos = email.find("\r\n\r\n");

    if (splitPos == std::string::npos)
    {
        return email;
    }

    std::string headers = email.substr(0, splitPos + 4);
    std::string encodedBody = email.substr(splitPos + 4);

    std::string decodedBody = base64Decode(encodedBody);

    if (containsForbiddenWord(decodedBody))
    {
        decodedBody = "Hello world\r\n";
    }
    else
    {
        decodedBody = replaceWords(decodedBody);

        const std::string disclaimer =
            "Please do not take anything in this email seriously!";

        if (!endsWithDisclaimerLine(decodedBody))
        {
            decodedBody += "\r\n\r\n";
            decodedBody += disclaimer;
            decodedBody += "\r\n";
        }
    }

    std::string encodedModifiedBody = base64Encode(decodedBody);
    encodedModifiedBody = wrapBase64Lines(encodedModifiedBody);

    return headers + encodedModifiedBody;
}

std::string SMTPProxy::applyFirewallRules(const std::string &email)
{
     if (isBase64Email(email))
    {
        return processBase64Email(email);
    }
    
    if (containsForbiddenWord(email))
    {
        return "Hello world\r\n";
    }

    const std::string disclaimer = "Please do not take anything in this email seriously!";

    std::string modified = replaceWords(email);

    if (!endsWithDisclaimerLine(modified))
    {
        modified += "\r\n\r\n";
        modified += disclaimer;
    }

    return modified;
}

void SMTPProxy::handleClient(int clientSock, const std::string &smtpHost, int smtpPort)
{
    int serverSock = connectToSMTPServer(smtpHost, smtpPort);

    try
    {
        std::string response;

        recvSMTPResponse(serverSock, response);
        sendAll(clientSock, response);

        while (true)
        {
            std::string clientLine = recvLineCRLF(clientSock);

            if (clientLine == "DATA\r\n" || clientLine == "data\r\n")
            {
                sendAll(serverSock, clientLine);

                recvSMTPResponse(serverSock, response);
                sendAll(clientSock, response);

                std::string emailContent;

                while (true)
                {
                    std::string dataLine = recvLineCRLF(clientSock);

                    if (dataLine == ".\r\n")
                    {
                        break;
                    }

                    emailContent += dataLine;
                }

                emailContent = applyFirewallRules(emailContent);

                if (emailContent.length() < 2 ||
                    emailContent.substr(emailContent.length() - 2) != "\r\n")
                {
                    emailContent += "\r\n";
                }


                sendAll(serverSock, emailContent);

                sendAll(serverSock, ".\r\n");

                recvSMTPResponse(serverSock, response);
                sendAll(clientSock, response);
            }
            else
            {
                sendAll(serverSock, clientLine);

                recvSMTPResponse(serverSock, response);
                sendAll(clientSock, response);

                if (clientLine == "QUIT\r\n" || clientLine == "quit\r\n")
                {
                    break;
                }
            }
        }
    }
    catch (...)
    {
        close(serverSock);
        throw;
    }

    close(serverSock);
}

void SMTPProxy::start(int listenPort, const std::string &smtpHost, int smtpPort)
{
    int listenSock = createListeningSocket(listenPort);

    std::cout << "SMTP proxy listening on port " << listenPort << std::endl;
    std::cout << "Forwarding to SMTP server " << smtpHost << ":" << smtpPort << std::endl;

    while (true)
    {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        int clientSock = accept(
            listenSock,
            (struct sockaddr *)&clientAddr,
            &clientLen);

        if (clientSock < 0)
        {
            std::cerr << "accept failed" << std::endl;
            continue;
        }

        std::cout << "Client connected" << std::endl;

        try
        {
            handleClient(clientSock, smtpHost, smtpPort);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Proxy error: " << e.what() << std::endl;
        }

        close(clientSock);
        std::cout << "Client disconnected" << std::endl;
    }

    close(listenSock);
}