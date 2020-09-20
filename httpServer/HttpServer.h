#pragma once
#include <SFML/Network.hpp>
#include <string>
#include <iostream>
#include <map>
#include <fstream>
#include <functional>
#include "Log.h"

#define REQUEST_SIZE 5000

/** 
 * Contains all the characteristics of the action related to the url.
 */
class UrlChar
{
public:
	/** 
	 * \public
	 * The url part after the hostname.
	 */
	std::string url;

	/** 
	 * \public
	 * The url to redirect to if response code is 3XX.
	 */
	std::string redirectUrl;

	/** 
	 * \public
	 * The response code to the request.
	 */
	unsigned short code;

	/**
	 * \public 
	 * The MIME type of the data to send.
	 */
	std::string mimeType;

	/** 
	 * \public
	 * The name of the file to send.
	 */
	std::string filename;

	/** 
	 * \public
	 * The callback function.
	 * \note The callback function is called \b before anything is done with other members.
	 * \param UrlChar* Allow to use this parameter as if it was \c this.
	 */
	std::function<void(UrlChar*)> callback;

	/** 
	 * \public
	 * The list of all variables (prefixed by a \c $) coming from GET and POST methods.
	 * \par Example: var["$name"] to access/modify the name variable.
	 * \note \c $$Location contains the url from which the request is coming.
	 */
	std::map<std::string, std::string>* var;
};

/** 
 * This class allows to run a http server.
 */
class HttpServer
{
public:
	/**
	 * \public
	 * \param port The port where the server will listen.
	 * \note By default, \c port is \c 0 which means that the server will run on any port available.
	 */
	HttpServer(int port = sf::Socket::AnyPort);

	/** 
	 * \public
	 * Launches the server. 
	 */
	void launch();

	/**
	 * \public
	 * Manages the response for a given url.
	 * \param url The url part after the hostname.
	 * \param code The response code to the request.
	 * \param mimeType The MIME type of the data to send.
	 * \param filename The name of the file to send.
	 * \return A reference on the current HttpServer to allow chaining req().
	 * \see UrlChar
	 */
	HttpServer& req(std::string url, unsigned short code, std::string mimeType, std::string filename);
	
	/**
	 * \public
	 * Manages the response for a given url.
	 * \param url The url part after the hostname.
	 * \param code The response code to the request.
	 * \param mimeType The MIME type of the data to send.
	 * \param filename The name of the file to send.
	 * \param callback The callback function.
	 * \note The callback function is called \b before anything is done with other members.
	 * \return A reference on the current HttpServer to allow chaining req().
	 * \see UrlChar, UrlChar.callback
	 */
	HttpServer& req(std::string url, unsigned short code, std::string mimeType, std::string filename, std::function<void(UrlChar*)> callback);
	
	/**
	 * \public
	 * \param url The url part after the hostname.
	 * \param code The response code to the request.
	 * \param redirectUrl The url to redirect to.
	 * \note For redirection, the code must be 3XX.
	 * \return A reference on the current HttpServer to allow chaining req().
	 * \see UrlChar
	 */
	HttpServer& req(std::string url, unsigned short code, std::string redirectUrl);

	/**
	 * \public
	 * \param url The url part after the hostname.
	 * \param code The response code to the request.
	 * \param redirectUrl The url to redirect to.
	 * \param callback The callback function.
	 * \note For redirection, the code must be 3XX.
	 * \note The callback function is called \b before anything is done with other members.
	 * \return A reference on the current HttpServer to allow chaining req().
	 * \see UrlChar, UrlChar.callback
	 */
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