#ifndef	ERROR_H
#define ERROR_H

#include <string>

#include "Console.h"

namespace Error
{
	static void Report(const char* _report)
	{
		Con::SetColor(Con::Color::RED, Con::Intensity::HIGH, Con::Color::BLACK, Con::Intensity::LOW);
		std::cout << _report << "\n";
	}
	static void Report(std::string _report)
	{
		Con::SetColor(Con::Color::RED, Con::Intensity::HIGH, Con::Color::BLACK, Con::Intensity::LOW);
		std::cout << _report.c_str() << "\n";
	}

	static void Report(const char* _file, const char* _line, const char* _report)
	{
		Con::SetColor(Con::Color::RED, Con::Intensity::HIGH, Con::Color::BLACK, Con::Intensity::LOW);
		std::cout << _file << " - " << _line << " - " << _report << "\n";
	}
	static void Report(const char* _file, const char* _line, std::string _report)
	{
		Con::SetColor(Con::Color::RED, Con::Intensity::HIGH, Con::Color::BLACK, Con::Intensity::LOW);
		std::cout << _file << " - " << _line << " - " << _report.c_str() << "\n";
	}
}



#endif