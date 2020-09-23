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
	// Redirects the user to "/home" if he didn't click the button
	auto& var = *chars->var;
	if(var.find("$body") == var.end() || var.find("$sender") == var.end() || var.find("$recipient") == var.end())
	{
		/*chars->code = 301;
		chars->redirectUrl = "/home";*/
		return;
	}

	// Changes the body variable of the page (in this case the body of the fake mail)
	chars->var->at("$body") = "Well, I guess not.";
}

int main()
{
	// Create the server and set it on port 8080
	HttpServer server(8080);
	server.logs = false;
	// Prepare the server responses
	server.req("/home", 200, "text/html", "webSrc/index.html")
		.req("/index.css", 200, "text/css", "webSrc/index.css")
		.req("/index.js", 200, "application/javascript", "webSrc/index.js")
		.req("/sendmail", 200, "text/html", "webSrc/sendmail.html", doStuff) // Uses doStuff(UrlChar*) as a callback
		.req("/img", 200, "image/png", "webSrc/img.png")
		.req("/ogg", 200, "audio/ogg", "webSrc/mettaton.ogg")
		.req("/loaderio-4db54377b4e344be0bc09288db5301eb.txt", 200, "text/plain", "webSrc/loaderio-4db54377b4e344be0bc09288db5301eb.txt")
		.req("/", 301, "/home").launch(1);
	return 0;
}