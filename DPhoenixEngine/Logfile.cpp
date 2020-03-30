#include "D3DUtil.h"

//create / open output.txt file
//for stream output to go
std::ofstream out("output.txt");

namespace DPhoenix
{
	//constructor
	LogFile::LogFile() {}
	//destructor
	LogFile::~LogFile()
	{
		//ensure the output isn't corrupt
		//(is good) and then close the
		//output file
		if (out.good()) out.close();
	}

	void LogFile::Print(std::string s)
	{
		//stream the string to the output file
		out << s << std::endl;
	}

	void LogFile::Print(char c[])
	{
		//stream the char to the output file
		out << c << std::endl;
	}

	//operator overloads so chevrons can be used
	//wrapper for th ofstream
	std::ofstream& LogFile::operator << (char c[])
	{
		//stream the char to the output file
		out << c;
		return out;
	}

	std::ofstream& LogFile::operator << (std::string s)
	{
		//stream the string to the output file
		out << s;
		return out;
	}

	std::ofstream& LogFile::operator << (double d)
	{
		//stream the double to the output file
		out << d;
		return out;
	}

	std::ofstream& LogFile::operator << (float f)
	{
		//stream the float to the output file
		out << f;
		return out;
	}

	std::ofstream& LogFile::operator << (int i)
	{
		//stream the int to the output file
		out << i;
		return out;
	}

	std::ofstream& LogFile::operator << (bool b)
	{
		//stream the boolean with text values
		//to output files
		if (b) out << "True";
		else out << "False";
		return out;
	}

};

