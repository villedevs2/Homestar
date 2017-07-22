// Windows specific implementation of BinaryFile class

#pragma once

#include <fstream>
#include <string>
#include <assert.h>

class BinaryFile
{
private:
	std::string m_filename;
	std::fstream m_stream;

	typedef union
	{
		unsigned int i;
		float f;
	} IntFloat;

public:
	BinaryFile();
	~BinaryFile();

	// static initializer
	static void init(void* pobject);

	bool open(std::string filename);
	void close();
	unsigned char read_byte();
	unsigned short read_word();
	unsigned int read_dword();
	float read_float();
	void read(char* buffer, int num_bytes);
	int get_length();
	void read_string(char* buffer, int max_size);
};
