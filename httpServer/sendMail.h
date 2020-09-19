#pragma once
#include <tchar.h>
#include <string>
#include "easendmailobj.tlh"

using namespace std;

class Mail_
{
public:
	string sender;
	string recipient;
	string password;
	string subject;
	string body;
	void send();
};