#pragma once
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#include <string>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <Windows.h>

enum class ConsoleColor
{
	BLUE = 1,
	GREEN = 2,
	CYAN = 3,
	RED = 4,
	PINK = 5,
	YELLOW = 6,
	WHITE = 7,
	GREY = 8,
	BRIGHT_BLUE = 9,
	BRIGHT_GREEN = 10,
	BRIGHT_CYAN = 11,
	BRIGHT_RED = 12,
	BRIGHT_PINK = 13,
	BRIGHT_YELLOW = 14,
	BRIGHT_WHITE = 15
};

void setConsoleColor(int color = (int)ConsoleColor::WHITE);

class Log
{
public:
	Log(std::string first, std::string second, int firstColor = (int)ConsoleColor::BRIGHT_WHITE, int secondColor = (int)ConsoleColor::BRIGHT_WHITE)
		: first(first), second(second), firstColor(firstColor), secondColor(secondColor)
	{
		time_t t = time(0);
		localtime_s(&date, &t);
	}

	int firstColor;
	int secondColor;
	std::string first;
	std::string second;
	tm date;
	void print()
	{
		std::cout << "(" << (date.tm_mday < 10 ? "0" : "") << date.tm_mday << "/" << (date.tm_mon < 10 ? "0" : "") << date.tm_mon + 1 << "/" << 1900 + date.tm_year <<
			" - " << (date.tm_hour < 10 ? "0" : "") << date.tm_hour << ":" << (date.tm_min < 10 ? "0" : "") << date.tm_min <<
			":" << (date.tm_sec < 10 ? "0" : "") << date.tm_sec << ") | ";
		setConsoleColor(firstColor);
		std::cout << first << " : ";
		setConsoleColor(secondColor);
		std::cout << second << std::endl;
		setConsoleColor();
	}
};

