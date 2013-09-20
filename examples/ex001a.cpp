#include <iostream>
#include "../cgi.h"

using namespace std;
using namespace cgi;

int main() {
    cout << "Content-Type: text/html\n";
    cout << "\n";
    cout << "<!doctype html>\n";
    cout << "<meta charset=\"UTF-8\">";
    cout << "<h1>Input your name and address</h1>";
    cout << "<form method=\"post\" action=\"ex001b.cgi\">";
    cout << "Name: <input type=\"text\" name=\"txtName\" /> <br />";
    cout << "Address: <input type=\"text\" name=\"txtAddress\" /> <br />";
    cout << "<input type=\"submit\" />";
    cout << "</form>";

    Environment env;
    string const cookie = env["HTTP_COOKIE"];
    if ( cookie != "" ) {
        Cookie c(cookie);
        string name = c["name"];
        string address = c["address"];
        cout << "<h2>Welcome back!</h2>";
        cout << "<ul>";
        cout << "<li>" << name << "</li>";
        cout << "<li>" << name.length() << "</li>";
        cout << "<li>" << address << "</li>";
        cout << "<li>" << address.length() << "</li>";
        cout << "</ul>";
    }
}

