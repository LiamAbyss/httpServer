#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <iostream>
#include <map>
#include <fstream>

#include "sendMail.h"

using namespace std;
using namespace sf;

TcpListener listener;
TcpSocket client;
string requestLocation;

string replace(string& s, string a, string b)
{
	return(s.replace(s.find(a), a.length(), b));
}

vector<string> sToVect(string s, string delimiter)
{
	size_t pos = 0;
	string token;
	vector<string> res;
	while ((pos = s.find(delimiter)) != string::npos)
	{
		token = s.substr(0, pos);
		res.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	if (!s.empty())
		res.push_back(s);
	return res;
}

string percentDecode(string s)
{
	map<string, string> reserved = {
		{"%21", "!"},
		{"%23", "#"},
		{"%24", "$"},
		{"%25", "%"},
		{"%26", "&"},
		{"%27", "'"},
		{"%28", "("},
		{"%29", ")"},
		{"%2A", "*"},
		{"%2B", "+"},
		{"%2C", ","},
		{"%2F", "/"},
		{"%3A", ":"},
		{"%3B", ";"},
		{"%3D", "="},
		{"%3F", "?"},
		{"%40", "@"},
		{"%5B", "["},
		{"%5D", "]"},
		{"+", " "},
		{"%E7", "ç"},
		{"%E9", "é"},
		{"%E8", "è"},
		{"%E0", "à"},
		{"%EA", "ê"},
		{"%E2", "â"},
		{"%F9", "ù"},
		{"%FB", "û"},
		{"%EB", "ë"},
		{"%EF", "ï"},
		{"%C7", "Ç"},
		{"%C9", "É"},
		{"%C0", "À"},
		{"%C8", "È"},
		{"%CA", "Ê"},
		{"%CB", "Ë"},
		{"%C2", "Â"}
	};

	for(auto& f : reserved)
	{
		while(s.find(f.first) != string::npos)
		{
			s = replace(s, f.first, f.second);
		}
	}
	return s;
}

map<string, string> parseUrlLocation(string url)
{
	map<string, string> res;
	res.emplace("$$Location", url.substr(0, url.find("?")));
	if (url.find("?") != string::npos)
	{
		url = url.substr(url.find("?") + 1, url.size());
		vector<string> pairs = sToVect(url, "&");

		for (auto& p : pairs)
		{
			res.emplace("$" + p.substr(0, p.find("=")), percentDecode(p.substr(p.find("=") + 1, p.size())));
		}
	}
	
	return res;
}

void sendHTML()
{
	string filename;
	string head;
	map<string, string> parsedUrl = parseUrlLocation(requestLocation);
	if (parsedUrl["$$Location"] == "/accueil")
	{
		head = "HTTP/1.1 200 OK\nContent-Type: text/html\n";
		filename = "index.html";
	}
	else if (parsedUrl["$$Location"] == "/bonjour")
	{
		head = "HTTP/1.1 200 OK\nContent-Type: text/html\n";
		Mail_ mail;
		mail.sender = parsedUrl["$sender"];
		mail.recipient = parsedUrl["$recipient"];
		mail.password = parsedUrl["$password"];
		mail.subject = parsedUrl["$subject"];
		mail.body = parsedUrl["$body"];
		//mail.send();
		filename = "bonjour.html";
	}
	else if (parsedUrl["$$Location"] == "/img")
	{
		head = "HTTP/1.1 200 OK\nContent-Type: image/png\n";
		filename = "img.png";
	}
	else if (parsedUrl["$$Location"] == "/index.css")
	{
		head = "HTTP/1.1 200 OK\nContent-Type: text/plain\n";
		filename = "index.css";
	}
	else if(parsedUrl["$$Location"] == "/")
	{
		head = "HTTP/1.1 301 REDIRECT\nContent-Type: text/html\nLocation: /accueil\n";
		filename = "error.html";	
	}
	else
	{
		head = "HTTP/1.1 404 NOT FOUND\nContent-Type: text/html\n"; //use Location: <url> + 3XX code to redirect
		filename = "error.html";
	}
	string page;
	ifstream file(filename.c_str(), ifstream::binary);
	bool started = false;
	while(file.good())
	{
		if (!started)
		{
			started = !started;
		}
		else page += '\n';
		string buffer;
		getline(file, buffer);
		page += buffer;
	}
	for(auto &var : parsedUrl)
	{
		while(page.find(var.first) != string::npos)
			page = replace(page, var.first, var.second);
	}
	head += "Content-Length: " + to_string(page.size()) + "\n\n" + page;

	client.send(head.c_str(), head.size() + 1);
}

string parseContent(string request)
{
	string cl = "Content-Length: ";
	int valueSize = stoi(request.substr(request.find(cl) + cl.size(), request.find("\r\n", request.find(cl)) - request.find(cl) - cl.size()));

	string res = request.substr(request.find("\r\n\r\n") + 4, valueSize);
	return res;
}

void serverListen()
{
	if(listener.listen(5789) != Socket::Done)
	{
	
	}

	while (listener.accept(client) == Socket::Status::Done)
	{
		char request[500];
		size_t received;
		requestLocation.clear();
		string fullRequest;

		if (client.receive(request, 500, received) == Socket::Done)
		{
			fullRequest += request;
			client.setBlocking(false);
			//sleep(milliseconds(100));
			while (client.receive(request, 500, received) == Socket::Done)
				fullRequest += request;
			client.setBlocking(true);
			int i = -1;
			bool ok = false;
			while (fullRequest[i + 1] != '\n' && fullRequest[i + 1] != '\0')
			{
				i++;
				if (i >= 1 && fullRequest[i - 1] == ' ' || ok)
					ok = true;
				else
					continue;
				requestLocation += fullRequest[i];
				if (fullRequest[i + 1] == ' ')
					break;
			}

			if (fullRequest.find("POST") != string::npos)
			{
				requestLocation = requestLocation + "?" + parseContent(fullRequest);
			}

			cout << "Connected..." << endl;
			/*string page = "HTTP/1.0 200 OK\nContent-Type: text/html\nContent-Length: 59\n\n<TITLE>Exemple</TITLE>\n<P>Ceci est une page d'exemple.</P>";

			client.send(page.c_str(), page.size() + 1);*/
			sendHTML();
			cout << "Packet sent" << endl;
		}
		
		client.disconnect();
	}
}

int main()
{
	Thread listener(&serverListen);

	listener.launch();

	return 0;
}