#include "HttpServer.h"

HttpServer::HttpServer(int port)
{
	if (listener.listen(port) != sf::Socket::Done)
	{
		Log("Error", "Couldn't listen on port " + std::to_string(port), (int)ConsoleColor::RED).print();
	}
	Log("Listening", "Port " + std::to_string(port), (int)ConsoleColor::GREEN).print();
}

void HttpServer::launch()
{
	while (listener.accept(client) == sf::Socket::Status::Done)
	{
		char request[REQUEST_SIZE];
		size_t received;
		requestLocation.clear();
		std::string fullRequest;

		if (client.receive(request, REQUEST_SIZE, received) == sf::Socket::Done)
		{
			if (received < REQUEST_SIZE)
				request[received] = '\0';
			fullRequest += request;
			client.setBlocking(false);
			//sleep(milliseconds(100));
			while (client.receive(request, REQUEST_SIZE, received) == sf::Socket::Done)
			{
				if (received < REQUEST_SIZE)
					request[received] = '\0';
				fullRequest += request;
			}
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

			Log("Request (" + client.getRemoteAddress().toString() + ")", requestLocation, (int)ConsoleColor::PINK).print();

			if (fullRequest.find("POST") != std::string::npos)
			{
				requestLocation = requestLocation + "?" + parseContent(fullRequest);
			}

			//cout << "Connected..." << endl;
			/*string page = "HTTP/1.0 200 OK\nContent-Type: text/html\nContent-Length: 59\n\n<TITLE>Exemple</TITLE>\n<P>Ceci est une page d'exemple.</P>";

			client.send(page.c_str(), page.size() + 1);*/
			sendHTML();
			//cout << "Packet sent" << endl;
		}

		client.disconnect();
	}
}

HttpServer& HttpServer::req(std::string url, unsigned short code, std::string mimeType, std::string filename)
{
	UrlChar buffer;
	buffer.url = url;
	buffer.code = code;
	buffer.mimeType = mimeType;
	buffer.filename = filename;
	downloadFile(filename);
	urlChars.push_back(buffer);

	return *this;
}

HttpServer& HttpServer::req(std::string url, unsigned short code, std::string mimeType, std::string filename, std::function<void(UrlChar*)> callback)
{
	UrlChar buffer;
	buffer.url = url;
	buffer.code = code;
	buffer.mimeType = mimeType;
	buffer.filename = filename;
	buffer.callback = callback;
	downloadFile(filename);

	urlChars.push_back(buffer);

	return *this;
}

HttpServer& HttpServer::req(std::string url, unsigned short code, std::string redirectUrl)
{
	UrlChar buffer;
	buffer.url = url;
	buffer.code = code;
	buffer.redirectUrl = redirectUrl;
	buffer.mimeType = "text/plain";

	urlChars.push_back(buffer);

	return *this;
}

HttpServer& HttpServer::req(std::string url, unsigned short code, std::string redirectUrl, std::function<void(UrlChar*)> callback)
{
	UrlChar buffer;
	buffer.url = url;
	buffer.code = code;
	buffer.redirectUrl = redirectUrl;
	buffer.mimeType = "text/plain";
	buffer.callback = callback;

	urlChars.push_back(buffer);

	return *this;
}

std::string HttpServer::parseContent(std::string request)
{
	std::string cl = "Content-Length: ";
	int valueSize = std::stoi(request.substr(request.find(cl) + cl.size(), request.find("\r\n", request.find(cl)) - request.find(cl) - cl.size()));

	std::string res = request.substr(request.find("\r\n\r\n") + 4, valueSize);
	return res;
}

void HttpServer::sendHTML()
{
	std::string filename;
	std::string head;
	std::map<std::string, std::string> parsedUrl = parseUrlLocation(requestLocation);

	bool found = false;
	for(auto &f : urlChars)
	{
		if(parsedUrl["$$Location"] == f.url)
		{
			found = true;

			f.var = &parsedUrl;
			if (f.callback)
				f.callback(&f);

			head = "HTTP/1.1 " + std::to_string(f.code) + "\nContent-Type: " + f.mimeType + "\n";
			if(f.code >= 300 && f.code < 400)
			{
				head += "Location: " + f.redirectUrl + "\n";
			}
			filename = f.filename;
			Log("Response (" + client.getRemoteAddress().toString() + ")", requestLocation + " FOUND", (int)ConsoleColor::GREEN).print();
			break;
		}
	}
	if(!found)
	{
		Log("Response (" + client.getRemoteAddress().toString() + ")", requestLocation + " NOT FOUND", (int)ConsoleColor::RED).print();

		for(auto &f : urlChars)
		{
			if(f.code == 404 || f.url == "default")
			{
				found = true;
				
				f.var = &parsedUrl;
				if(f.callback)
					f.callback(&f);

				head = "HTTP/1.1 " + std::to_string(f.code) + "\nContent-Type: " + f.mimeType +"\n";
				filename = f.filename;
			}
		}
	}

	if (found)
	{
		std::string page = savedFiles[filename];
		/*std::ifstream file(filename.c_str(), std::ifstream::binary);
		bool started = false;
		while (file.good())
		{
			if (!started)
			{
				started = !started;
			}
			else page += '\n';
			std::string buffer;
			std::getline(file, buffer);
			page += buffer;
		}*/
		for (auto& var : parsedUrl)
		{
			while (page.find(var.first) != std::string::npos)
				page = replace(page, var.first, var.second);
		}
		head += "Content-Length: " + std::to_string(page.size()) + "\n\n" + page;

		client.send(head.c_str(), head.size() + 1);
	}
	else
	{
		std::string s = "HTTP/1.1 404 NOT FOUND\nContent-Type: text/html\nContent-Length: 155\n\n<!DOCTYPE HTML/>\n<html>\n<head>\n<title>Page Not Found</title>\n<meta charset=\"iso-8859-1\"/>\n</head><body>\n<h1>Error 404 : Page Not Found</h1>\n</body>\n</html>";
		client.send(s.c_str(), s.size() + 1);
	}
}

std::map<std::string, std::string> HttpServer::parseUrlLocation(std::string url)
{
	std::map<std::string, std::string> res;
	res.emplace("$$Location", url.substr(0, url.find("?")));
	if (url.find("?") != std::string::npos)
	{
		url = url.substr(url.find("?") + 1, url.size());
		std::vector<std::string> pairs = sToVect(url, "&");

		for (auto& p : pairs)
		{
			res.emplace("$" + p.substr(0, p.find("=")), percentDecode(p.substr(p.find("=") + 1, p.size())));
		}
	}

	return res;
}

std::string HttpServer::percentDecode(std::string s)
{
	std::map<std::string, std::string> reserved = {
		{"%21", "!"},
		{"%22", "\""},
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

	for (auto& f : reserved)
	{
		while (s.find(f.first) != std::string::npos)
		{
			s = replace(s, f.first, f.second);
		}
	}
	return s;
}

void HttpServer::downloadFile(std::string filename)
{
	std::string fileStr;
	if (savedFiles.find(filename) == savedFiles.end())
	{
		std::ifstream file(filename.c_str(), std::ifstream::binary);
		bool started = false;
		while (file.good())
		{
			if (!started)
			{
				started = !started;
			}
			else fileStr += '\n';
			std::string buffer;
			std::getline(file, buffer);
			fileStr += buffer;
		}
		savedFiles.emplace(filename, fileStr);
	}
}

std::vector<std::string> sToVect(std::string s, std::string delimiter)
{
	size_t pos = 0;
	std::string token;
	std::vector<std::string> res;
	while ((pos = s.find(delimiter)) != std::string::npos)
	{
		token = s.substr(0, pos);
		res.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	if (!s.empty())
		res.push_back(s);
	return res;
}

std::string replace(std::string& s, std::string a, std::string b)
{
	return(s.replace(s.find(a), a.length(), b));
}