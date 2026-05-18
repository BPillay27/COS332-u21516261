#include "Proxy.h"
#include <iostream>

int main()
{
    try
    {
        SMTPProxy proxy;
        proxy.start(55555, "127.0.0.1", 25);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}