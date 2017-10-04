#include "Texture2.h"

using namespace std;
using namespace rapidxml;

// init static members
GLuint Atlas::m_current_bind[8] = {0, 0, 0, 0, 0, 0, 0, 0};
std::list<Atlas*> Atlas::m_atlaslist;

Atlas::Atlas()
{
	m_width = 0;
	m_height = 0;
	m_texname = 0;
	m_name = "";
}

Atlas::~Atlas()
{
	Debug::log("Atlas destructor " + m_name);

	if (glIsTexture(m_texname))
		glDeleteTextures(1, &m_texname);
}

void Atlas::init()
{
	for (int i=0; i < 8; i++)
	{
		m_current_bind[i] = 0;
	}

	m_atlaslist.clear();
}

void Atlas::pngReadData(png_structp png_ptr, png_bytep data, png_size_t length)
{
	png_voidp input = png_get_io_ptr(png_ptr);
    ((BinaryFile*)input)->read((char*)data, length);
}


bool Atlas::loadPNG(std::string filename, Image* image)
{
	BinaryFile input;
	char* pixels = NULL;
	png_bytep *row_pointers = NULL;

	png_uint_32 image_width;
	png_uint_32 image_height;
	png_uint_32 bit_depth;
	png_uint_32 num_channels;
	png_uint_32 color_type;

	try
	{
		bool result = input.open(filename);
		if (!result)
			throw ios_base::failure("Texture::loadPNG(): couldn't open file " + filename);

		png_byte png_sig[8];
		input.read((char*)png_sig, 8);

		if (png_sig_cmp(png_sig, 0, 8) != 0)
			throw ios_base::failure("Texture::loadPNG(): png_check_sig() failed.");

		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png_ptr)
			throw ios_base::failure("Texture::loadPNG(): png_create_read_struct() failed.");

		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
			throw ios_base::failure("Texture::loadPNG(): png_create_info_struct() [info_ptr] failed.");

		png_infop end_info = png_create_info_struct(png_ptr);
		if (!end_info)
			throw ios_base::failure("Texture::loadPNG(): png_create_info_struct() [end_info] failed.");

		png_set_read_fn(png_ptr, (png_voidp)&input, pngReadData);

		// read the png file..
		png_set_sig_bytes(png_ptr, 8);
		png_read_info(png_ptr, info_ptr);

		image_width		= png_get_image_width(png_ptr, info_ptr);
		image_height	= png_get_image_height(png_ptr, info_ptr);

		bit_depth		= png_get_bit_depth(png_ptr, info_ptr);
		num_channels	= png_get_channels(png_ptr, info_ptr);
		color_type		= png_get_color_type(png_ptr, info_ptr);

		switch (color_type)
		{
			case PNG_COLOR_TYPE_PALETTE:
				png_set_palette_to_rgb(png_ptr);
				num_channels = 3;
				break;

			case PNG_COLOR_TYPE_GRAY:
				if (bit_depth < 8)
					png_set_expand_gray_1_2_4_to_8(png_ptr);
				bit_depth = 8;
				break;
		}

		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		{
			png_set_tRNS_to_alpha(png_ptr);
			num_channels++;
		}

		if (bit_depth == 16)
			png_set_strip_16(png_ptr);

		png_bytep *row_pointers = new png_bytep[image_height];

		pixels = new char[image_width * image_height * bit_depth * num_channels / 8];
		const int stride = image_width * bit_depth * num_channels / 8;
		
		for (int i = 0; i < (int)image_height; i++)
		{
			png_uint_32 offset = (image_height - i - 1) * stride;
			row_pointers[i] = (png_bytep)pixels + (i * stride);
		}
		
		png_read_image(png_ptr, row_pointers);

		png_read_end(png_ptr, end_info);
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		
		input.close();

		if (row_pointers != nullptr)
		{
			delete [] row_pointers;
			row_pointers = nullptr;
		}
	}
	catch (ios_base::failure failure)
	{
		Debug::log(failure.what());

		input.close();

		if (row_pointers != nullptr)
		{
			delete [] row_pointers;
			row_pointers = nullptr;
		}
		if (pixels != nullptr)
		{
			delete [] pixels;
			pixels = nullptr;
		}
		return false;
	}

	image->pixels = pixels;
	image->width = image_width;
	image->height = image_height;

	if (num_channels == 3)
		image->format = TEXTURE_FORMAT_RGB;
	else if (num_channels == 4)
		image->format = TEXTURE_FORMAT_RGBA;

	return true;
}



// static Atlas loader
Atlas* Atlas::loadAtlas(std::string filename)
{
	Atlas* atlas = new Atlas();

	if (atlas->load(filename))
	{
		Debug::log("Loaded atlas " + filename);
		m_atlaslist.push_back(atlas);
		return m_atlaslist.back();
	}

	delete atlas;	
	return nullptr;
}

// static Atlas finder
Atlas* Atlas::findAtlas(std::string name)
{
	for (std::list<Atlas*>::iterator it=m_atlaslist.begin(); it != m_atlaslist.end(); ++it)
	{
		Atlas* atlas = *it;
		if (atlas->m_name.compare(name) == 0)
		{
			return atlas;
		}
	}

	return nullptr;
}

int Atlas::getWidth()
{
	return m_width;
}

int Atlas::getHeight()
{
	return m_height;
}

void Atlas::apply(int unit)
{
	if (m_current_bind[unit] != m_texname)
	{
		m_current_bind[unit] = m_texname;

		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, m_texname);
	}
}




void Atlas::addReference(TextureReference* texture)
{
	m_references.push_back(texture);
}

void Atlas::deleteReference(TextureReference* texture)
{
//	Debug::log("delete reference to " + ((Texture*)texture)->getName());

	std::list<TextureReference*>::iterator it;
	for (it = m_references.begin(); it != m_references.end(); ++it)
	{
		if (*it == texture)
		{
			m_references.remove(*it);
			if (m_references.size() <= 0)
			{
				m_atlaslist.remove(this);
				delete this;
			}
			return;
		}
	}
}

bool Atlas::load(std::string filename)
{
	Image image;

	if (!loadPNG(filename, &image))
		return false;

	if (glIsTexture(m_texname))
		glDeleteTextures(1, &m_texname);

	glGenTextures(1, &m_texname);
	m_name = filename;
	m_width = image.width;
	m_height = image.height;

	// upload it
	glBindTexture(GL_TEXTURE_2D, m_texname);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	GLenum type = 0;
	switch (image.format)
	{
		case TEXTURE_FORMAT_RGB:	type = GL_RGB; break;
		case TEXTURE_FORMAT_RGBA:	type = GL_RGBA; break;
	}
			
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 type,
				 image.width,
				 image.height,
				 0,
				 type,
				 GL_UNSIGNED_BYTE,
				 image.pixels);

	if (image.pixels != NULL)
	{
		delete [] image.pixels;
	}

	return true;
}





// Texture class
// ----------------------------------------------------------------------------

Texture::Texture()
{
	m_atlas = nullptr;
	m_name = "";
}

Texture::~Texture()
{
	if (m_atlas != NULL)
	{
		m_atlas->deleteReference(this);
	}
}

Atlas* Texture::getAtlas()
{
	return m_atlas;
}

void Texture::setAtlas(Atlas* atlas)
{
	if (m_atlas != NULL)
	{
		m_atlas->deleteReference(this);
	}

	m_atlas = atlas;
	m_atlas->addReference(this);
}

std::string Texture::getName()
{
	return m_name;
}

void Texture::setName(std::string name)
{
	m_name = name;
}

void Texture::apply(int unit)
{
	m_atlas->apply(unit);
}

bool Texture::load(std::string filename, float x, float y, float width, float height)
{
	Atlas* atlas = nullptr;

	// find atlas
	atlas = Atlas::findAtlas(filename);

	if (atlas == nullptr)
	{
		Debug::log("Texture::load(): Atlas " + filename + " not loaded yet, loading");
		atlas = Atlas::loadAtlas(filename);
	}

	if (atlas == nullptr)
	{
		Debug::error("Texture::load(): atlas null " + filename);
		return false;
	}
	

	m_atlas = atlas;

	float atlas_width = (float)(atlas->getWidth());
	float atlas_height = (float)(atlas->getHeight());

	m_left = ((float)(x + 0.5f) / atlas_width);
	m_top = ((float)(y + 0.5f) / atlas_height);
	m_right = m_left + ((float)(width-1) / atlas_width);
	m_bottom = m_top + ((float)(height-1) / atlas_height);

	m_name = filename;

	atlas->addReference(this);

	return true;
}

bool Texture::load(std::string filename)
{
	Atlas* atlas = nullptr;

	Debug::log("Texture::load(): " + filename);

	// find atlas
	atlas = Atlas::findAtlas(filename);

	if (atlas == nullptr)
	{
		atlas = Atlas::loadAtlas(filename);
	}

	if (atlas == nullptr)
	{
		return false;
	}

	m_atlas = atlas;

	m_left = 0.0f;
	m_top = 0.0f;
	m_right = 1.0f;
	m_bottom = 1.0f;

	m_name = filename;

	atlas->addReference(this);

	return true;
}




// TextureSet class
// ----------------------------------------------------------------------------

TextureSet::TextureSet()
{
	m_atlas = nullptr;
}

TextureSet::~TextureSet()
{
	int num_textures = m_textures.size();
	for (int i=0; i < num_textures; i++)
	{
		delete m_textures[i];
	}

	if (m_atlas != nullptr)
		m_atlas->deleteReference(this);
}

void TextureSet::apply(int unit)
{
	assert(m_textures.size() > 0);

	m_textures[0]->apply(unit);
}

Atlas* TextureSet::getAtlas()
{
	return m_atlas;
}

Texture* TextureSet::findTexture(std::string name)
{
	int num_textures = m_textures.size();
	for (int i=0; i < num_textures; i++)
	{
		if (name == m_textures[i]->getName())
			return m_textures[i];
	}

	return nullptr;
}

Texture* TextureSet::getTexture(int index)
{
	assert(index >= 0 && index < m_textures.size());

	return m_textures[index];
}

int TextureSet::getNumTextures()
{
	return (int)(m_textures.size());
}

bool TextureSet::load(std::string filename)
{
	BinaryFile input;
	char* text = nullptr;

	std::string atlas_name = "";
	std::list<TextureEntry> texture;

	xml_document<> doc;

	// load XML file
	try
	{
		bool result = input.open(filename);
		if (!result)
			throw ios_base::failure("TextureSet::load(): couldn't open file " + filename);

		int length = input.get_length();
		text = new char[length+1];
		
		input.read(text, length);
		text[length] = 0;		// null-termination in case the file is missing it

		input.close();
	}
	catch (ios_base::failure failure)
	{
		input.close();

		if (text != nullptr)
			delete [] text;

		return nullptr;
	}

	// parse it
	try
	{
		doc.parse<0>(text);
	}
	catch (rapidxml::parse_error failure)
	{
		const char* error_what = failure.what();
		char* error_where = failure.where<char>();

		if (text != nullptr)
			delete [] text;

		return nullptr;
	}

	xml_node<>* root = doc.first_node("atlas");

	// loop through subnodes
	xml_node<>* node = root->first_node();
	while (node != 0)
	{
		std::string nodename = std::string(node->name());
		std::transform(nodename.begin(), nodename.end(), nodename.begin(), ::tolower);

		if (nodename == "sheet")
		{
			xml_attribute<>* attr = node->first_attribute();
			while (attr != 0)
			{
				std::string attrname = std::string(attr->name());
				std::transform(attrname.begin(), attrname.end(), attrname.begin(), ::tolower);
				if (attrname == "src")
				{
					atlas_name = std::string(attr->value());
				}
				else if (attrname == "type")
				{
					// ignore this for now (only diffuse would be handled)
				}

				attr = attr->next_attribute();
			}
		}
		else if (nodename == "texture")
		{
			xml_attribute<>* attr = node->first_attribute();

			TextureEntry txr;

			while (attr != 0)
			{
				std::string attrname = std::string(attr->name());
				std::transform(attrname.begin(), attrname.end(), attrname.begin(), ::tolower);
				if (attrname == "name")
				{
					txr.name = std::string(attr->value());
				}
				else if (attrname == "x")
				{
					txr.x = strtol(attr->value(), NULL, 10);
				}
				else if (attrname == "y")
				{
					txr.y = strtol(attr->value(), NULL, 10);
				}
				else if (attrname == "width")
				{
					txr.width = strtol(attr->value(), NULL, 10);
				}
				else if (attrname == "height")
				{
					txr.height = strtol(attr->value(), NULL, 10);
				}

				attr = attr->next_attribute();
			}

			texture.push_back(txr);
		}

		node = node->next_sibling();
	}

	if (text != nullptr)
		delete [] text;

	// load atlas if we have filename
	if (atlas_name.length() > 0)
	{
		m_textures.clear();

		m_atlas = Atlas::findAtlas(atlas_name);		
		if (m_atlas == nullptr)
		{
			m_atlas = Atlas::loadAtlas(atlas_name);
		}
		if (m_atlas == nullptr)
			return false;

		float atlas_width = m_atlas->getWidth();
		float atlas_height = m_atlas->getHeight();

		// add textures
		std::list<TextureEntry>::iterator it;
		for (it = texture.begin(); it != texture.end(); ++it)
		{
			TextureEntry ent = *it;

			Texture* tex = new Texture();

			tex->m_left = ((float)(ent.x + 0.5f) / (float)(atlas_width));
			tex->m_top = ((float)(ent.y + 0.5f) / (float)(atlas_height));
			tex->m_right = tex->m_left + ((float)(ent.width-1) / (float)(atlas_width));
			tex->m_bottom = tex->m_top + ((float)(ent.height-1) / (float)(atlas_height));
			tex->setName(ent.name);
			tex->setAtlas(m_atlas);

			m_textures.push_back(tex);
		}

		m_atlas->addReference(this);

		return true;
	}

	return false;
}
