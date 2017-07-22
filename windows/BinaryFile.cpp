// Windows specific implementation of BinaryFile class

#include "BinaryFile.h"

using namespace std;

void BinaryFile::init(void* pobject)
{
}

BinaryFile::BinaryFile()
{

}

BinaryFile::~BinaryFile(void)
{
	if (m_stream.is_open())
		m_stream.close();
}

bool BinaryFile::open(std::string filename)
{
	m_stream.open(filename.c_str(), ios_base::in | ios_base::binary);

	if (!m_stream.is_open())
		return false;

	m_stream.seekg(0, ios::beg);

	return true;
}

void BinaryFile::close()
{
	m_stream.close();
}

void BinaryFile::read_string(char* buffer, int max_size)
{
	for (int i = 0; i < max_size-1; i++)
	{
		unsigned char cc = m_stream.get();
		buffer[i] = cc;
		if (cc == 0)
			return;
	}

	buffer[max_size - 1] = 0;
}

void BinaryFile::read(char* buffer, int num_bytes)
{
	m_stream.read(buffer, num_bytes);
}

unsigned char BinaryFile::read_byte()
{
	return m_stream.get();
}

unsigned short BinaryFile::read_word()
{
	char t[2];
	m_stream.read(t, 2);

	return ((unsigned int)(t[0] & 0xff) << 8) |
		   (unsigned int)(t[1] & 0xff);
}

unsigned int BinaryFile::read_dword()
{
	char t[4];
	m_stream.read(t, 4);

	return ((unsigned int)(t[0] & 0xff) << 24) |
		   ((unsigned int)(t[1] & 0xff) << 16) |
		   ((unsigned int)(t[2] & 0xff) << 8) |
		   (unsigned int)(t[3] & 0xff);
}

float BinaryFile::read_float()
{
	char t[4];
	m_stream.read(t, 4);

	IntFloat f;

	f.i = ((unsigned int)(t[0] & 0xff) << 24) |
		  ((unsigned int)(t[1] & 0xff) << 16) |
		  ((unsigned int)(t[2] & 0xff) << 8) |
		  (unsigned int)(t[3] & 0xff);
	return f.f;
}

int BinaryFile::get_length()
{
	m_stream.seekg(0, ios::beg);
	std::streamoff begin = m_stream.tellg();
	m_stream.seekg(0, ios::end);
	std::streamoff end = m_stream.tellg();
	m_stream.seekg(0, ios::beg);

	return (int)(end - begin);
}
