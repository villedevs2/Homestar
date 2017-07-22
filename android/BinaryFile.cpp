// Android specific implementation of BinaryFile class

#include "BinaryFile.h"

AAssetManager* BinaryFile::m_manager = NULL;

void BinaryFile::init(void* pobject)
{
	m_manager = (AAssetManager*)pobject;
}

BinaryFile::BinaryFile()
{
	m_asset = NULL;
}

BinaryFile::~BinaryFile(void)
{
	if (m_asset != NULL)
		AAsset_close(m_asset);
}

bool BinaryFile::open(std::string filename)
{
	assert(m_manager != NULL);

	m_asset = AAssetManager_open(m_manager, filename.c_str(), AASSET_MODE_RANDOM);

	if (m_asset == NULL)
		return false;

	AAsset_seek(m_asset, 0, SEEK_SET);

	return true;
}

void BinaryFile::close()
{
	if (m_asset != NULL)
		AAsset_close(m_asset);

	m_asset = NULL;
}

void BinaryFile::read(char* buffer, int num_bytes)
{
	if (m_asset != NULL)
	{
		AAsset_read(m_asset, buffer, num_bytes);
	}
}

unsigned char BinaryFile::read_byte()
{
	char t = 0;

	if (m_asset != NULL)
		AAsset_read(m_asset, &t, 1);

	return t;
}

unsigned short BinaryFile::read_word()
{
	char t[2];

	if (m_asset != NULL)
		AAsset_read(m_asset, t, 2);

	return ((unsigned int)(t[0] & 0xff) << 8) |
		   (unsigned int)(t[1] & 0xff);
}

unsigned int BinaryFile::read_dword()
{
	char t[4];

	if (m_asset != NULL)
		AAsset_read(m_asset, t, 4);

	return ((unsigned int)(t[0] & 0xff) << 24) |
		   ((unsigned int)(t[1] & 0xff) << 16) |
		   ((unsigned int)(t[2] & 0xff) << 8) |
		   (unsigned int)(t[3] & 0xff);
}

float BinaryFile::read_float()
{
	char t[4];

	if (m_asset != NULL)
		AAsset_read(m_asset, t, 4);

	IntFloat f;

	f.i = ((unsigned int)(t[0] & 0xff) << 24) |
		  ((unsigned int)(t[1] & 0xff) << 16) |
		  ((unsigned int)(t[2] & 0xff) << 8) |
		  (unsigned int)(t[3] & 0xff);
	return f.f;
}

int BinaryFile::get_length()
{
	if (m_asset != NULL)
	{
		return AAsset_getLength(m_asset);
	}

	return 0;
}
