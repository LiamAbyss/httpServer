#include <SFML/Network.hpp>
#include <string>
#include <iostream>
#include <map>
#include <fstream>

#include "HttpServer.h"

using namespace std;
using namespace sf;

int main()
{
	HttpServer server(5789);
	server.req("/home", 200, "text/html", "webSrc/index.html")
		.req("/index.css", 200, "text/css", "webSrc/index.css")
		.req("/index.js", 200, "application/javascript", "webSrc/index.js")
		.req("/sendmail", 200, "text/html", "webSrc/sendmail.html")
		.req("/img", 200, "image/png", "webSrc/img.png")
		.req("/", 301, "/home");
	server.launch();

	return 0;
}