#include <iostream>
#include "../cgi.h"

using namespace std;
using namespace cgi;

int main()
{
    Form form;
    string name = form["txtName"];
    string address = form["txtAddress"];

    DateTime dt;
    dt.addSeconds(60 * 5);

    Cookie c1("name", name);
    c1.setExpires(dt);

    Cookie c2("address", address);
    c2.setExpires(dt);

    cout << "Content-Type: text/html\n";
    cout << c1.header() << "\n";
    cout << c2.header() << "\n";
    cout << "\n";
    cout << "<!doctype html>\n";
    cout << "<meta charset=\"UTF-8\">";
    cout << "<h1>Hello</h1>";
    cout << "Name: " << Utility::htmlEncode(name) << "<br />";
    cout << "Address: " << Utility::htmlEncode(address) << "<br />";
    cout << "<a href=\"ex001a.cgi\">return</a>";
}

