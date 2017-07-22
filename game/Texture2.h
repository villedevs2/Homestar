#pragma once

#include <Platform.h>
#include "libpng/png.h"
#include "format/format.h"
#include "rapidxml.hpp"

#include "Debug.h"
#include "BinaryFile.h"

#include <string>
#include <vector>
#include <assert.h>
#include <list>

class Texture;

class Atlas
{
public:
	class TextureReference
	{
	};

	Atlas();
	~Atlas();

	static Atlas* loadAtlas(std::string filename);
	static Atlas* findAtlas(std::string name);
	static void init();
	void apply(int unit);
	int getWidth();
	int getHeight();
	void addReference(TextureReference* texture);
	void deleteReference(TextureReference* texture);

private:
	typedef enum
	{
		TEXTURE_FORMAT_RGB,
		TEXTURE_FORMAT_RGBA,
	} Format;

	typedef struct
	{
		char* pixels;
		int width;
		int height;
		Format format;
	} Image;

	static void pngReadData(png_structp png_ptr, png_bytep data, png_size_t length);
	static bool loadPNG(std::string filename, Image* image);

	bool load(std::string filename);

	GLuint m_texname;
	int m_width;
	int m_height;
	std::string m_name;
	std::list<TextureReference*> m_references;

	static GLuint m_current_bind[8];
	static std::list<Atlas*> m_atlaslist;
};


// Texture class
class Texture : public Atlas::TextureReference
{
public:
	Texture();
	~Texture();

	bool load(std::string filename, float x, float y, float width, float height);
	bool load(std::string filename);
	Atlas* getAtlas();
	void setAtlas(Atlas* atlas);
	std::string getName();
	void setName(std::string name);
	void apply(int unit);

	// intentionally public
	float m_left;
	float m_top;
	float m_right;
	float m_bottom;

private:
	Atlas* m_atlas;
	std::string m_name;
};


// Texture set class
class TextureSet : public Atlas::TextureReference
{
public:
	TextureSet();
	~TextureSet();

	bool load(std::string filename);
	void apply(int unit);
	int getNumTextures();
	Atlas* getAtlas();
	Texture* getTexture(int index);
	Texture* findTexture(std::string name);

private:
	struct TextureEntry
	{
		std::string name;
		int x;
		int y;
		int width;
		int height;
	};

	Atlas* m_atlas;
	std::vector<Texture*> m_textures;
};
