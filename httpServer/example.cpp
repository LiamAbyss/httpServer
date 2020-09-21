#include <SFML/Network.hpp>
#include <string>
#include <iostream>
#include <map>
#include <fstream>

#include "HttpServer.h"

using namespace std;
using namespace sf;

void doStuff(UrlChar* chars)
{
	// Changes the body variable of the page (in this case the body of the fake mail)
	chars->var->at("$body") = "Well, I guess not.";

	// Redirects the user to "/img"
	/*chars->code = 301;
	chars->redirectUrl = "/img";*/
}

int main()
{
	// Create the server and set it on port 8080
	HttpServer server(8080);
	
	// Prepare the server responses
	server.req("/home", 200, "text/html", "webSrc/index.html")
		.req("/index.css", 200, "text/css", "webSrc/index.css")
		.req("/index.js", 200, "application/javascript", "webSrc/index.js")
		.req("/sendmail", 200, "text/html", "webSrc/sendmail.html", doStuff) // Uses doStuff(UrlChar*) as a callback
		.req("/img", 200, "image/png", "webSrc/img.png")
		.req("/ogg", 200, "audio/ogg", "webSrc/mettaton.ogg")
		.req("/", 301, "/home")
		.launch(); // Launches the server

	return 0;
}