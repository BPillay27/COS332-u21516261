// #include <iostream>
// #include <cstdlib>
// #include <ctime>

// void printLine(const std::string &input)
// {
//     std::cout << input << "\n";
// }

// int main()
// {
//     std::srand(std::time(nullptr));

//     int x = std::rand() % 100 + 1;
//     int y = std::rand() % 100 + 1;

//     printLine("Content-Type: text/html\r\n");
//     printLine("<!doctype html>");
//     printLine("<html>");
//     printLine("<head>");
//     printLine("<meta http-equiv=\"Cache-Control\" content=\"no-cache, no-store, must-revalidate\" />");
//     printLine("<meta http-equiv=\"Pragma\" content=\"no-cache\" />");
//     printLine("<meta http-equiv=\"Expires\" content=\"0\" />");
//     printLine("<title>Pick the Larger Number</title>");
//     printLine("</head>");
//     printLine("<body>");

//     printLine("<h2>Click on the larger number...</h2>");

//     if (x > y)
//     {
//         printLine("<a href=\"/right.htm\">" + std::to_string(x) + "</a>");
//         printLine("&nbsp;&nbsp;");
//         printLine("<a href=\"/wrong.htm\">" + std::to_string(y) + "</a>");
//     }
//     else
//     {
//         printLine("<a href=\"/wrong.htm\">" + std::to_string(x) + "</a>");
//         printLine("&nbsp;&nbsp;");
//         printLine("<a href=\"/right.htm\">" + std::to_string(y) + "</a>");
//     }
//     return 0;
// }

#include <iostream>
#include <cstdlib>
#include <ctime>

void printLine(const std::string &input)
{
    std::cout << input << "\n";
}

int main()
{
    std::srand(std::time(nullptr));

    int x = std::rand() % 100 + 1;
    int y = std::rand() % 100 + 1;

    // HTTP header
    printLine("Content-Type: text/html\r\n");

    printLine("<!doctype html>");
    printLine("<html>");
    printLine("<head>");
    printLine("<meta http-equiv=\"Cache-Control\" content=\"no-cache, no-store, must-revalidate\" />");
    printLine("<meta http-equiv=\"Pragma\" content=\"no-cache\" />");
    printLine("<meta http-equiv=\"Expires\" content=\"0\" />");
    printLine("<meta charset=\"utf-8\">");
    printLine("<title>Pick the Larger Number</title>");

    // Embedded CSS
    printLine("<style>");
    printLine("body {");
    printLine("  font-family: Arial, sans-serif;");
    printLine("  background: #f4f6f8;");
    printLine("  text-align: center;");
    printLine("  margin-top: 80px;");
    printLine("}");
    printLine(".card {");
    printLine("  background: white;");
    printLine("  padding: 40px;");
    printLine("  border-radius: 10px;");
    printLine("  display: inline-block;");
    printLine("  box-shadow: 0 4px 15px rgba(0,0,0,0.1);");
    printLine("}");
    printLine(".numbers a {");
    printLine("  font-size: 3rem;");
    printLine("  margin: 20px;");
    printLine("  text-decoration: none;");
    printLine("  color: #0077cc;");
    printLine("  transition: 0.2s;");
    printLine("}");
    printLine(".numbers a:hover {");
    printLine("  color: #ff4d4d;");
    printLine("  transform: scale(1.1);");
    printLine("}");
    printLine("</style>");

    printLine("</head>");
    printLine("<body>");
    printLine("<div class=\"card\">");
    printLine("<h2>Click on the larger number</h2>");
    printLine("<div class=\"numbers\">");

    if (x > y)
    {
        printLine("<a href=\"/right.htm\">" + std::to_string(x) + "</a>");
        printLine("<a href=\"/wrong.htm\">" + std::to_string(y) + "</a>");
    }
    else
    {
        printLine("<a href=\"/wrong.htm\">" + std::to_string(x) + "</a>");
        printLine("<a href=\"/right.htm\">" + std::to_string(y) + "</a>");
    }

    printLine("</div>");
    printLine("</div>");
    printLine("</body>");
    printLine("</html>");

    return 0;
}
