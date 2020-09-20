#pragma once
#include <SFML/Network.hpp>
#include <string>
#include <iostream>
#include <map>
#include <fstream>
#include <functional>
#include "Log.h"

#define REQUEST_SIZE 5000

class UrlChar
{
public:
	std::string url;
	std::string redirectUrl;
	unsigned short code;
	std::string mimeType;
	std::string filename;
	std::function<void(UrlChar*)> callback;
	std::map<std::string, std::string>* var;
};

class HttpServer
{
public:
	HttpServer(int port = sf::Socket::AnyPort);
	void launch();

	HttpServer& req(std::string url, unsigned short code, std::string mimeType, std::string filename);
	HttpServer& req(std::string url, unsigned short code, std::string mimeType, std::string filename, std::function<void(UrlChar*)> callback);
	HttpServer& req(std::string url, unsigned short code, std::string redirectUrl);
	HttpServer& req(std::string url, unsigned short code, std::string redirectUrl, std::function<void(UrlChar*)> callback);

private:

	std::string parseContent(std::string request);
	void sendHTML();

	std::vector<UrlChar> urlChars;

	std::map<std::string, std::string> parseUrlLocation(std::string url);

	std::string percentDecode(std::string s);

	sf::TcpListener listener;
	sf::TcpSocket client;
	std::string requestLocation;
};


std::vector<std::string> sToVect(std::string s, std::string delimiter);
std::string replace(std::string& s, std::string a, std::string b);