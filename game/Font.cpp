#include "Font.h"

using namespace std;
using namespace rapidxml;

Font::Font()
{
	m_lineheight = 10;
	m_hmargin = 0;

	m_texture = NULL;
}

Font::~Font()
{
	if (m_texture != NULL)
		delete m_texture;
}

bool Font::load(std::string filename, int size)
{
	m_size = size;

	BinaryFile input;
	char* text = NULL;

	// load XML file
	try
	{
		bool result = input.open(filename);
		if (!result)
			throw ios_base::failure("Font::load(): couldn't open file.");

		int length = input.get_length();
		text = new char[length+1];
		
		input.read(text, length);
		text[length] = 0;		// null-termination in case the file is missing it

		input.close();
	}
	catch (ios_base::failure& failure)
	{
		Debug::error(failure.what());

		input.close();

		if (text != NULL)
			delete [] text;

		return false;
	}

	// parse it
	xml_document<> doc;
	try
	{
		doc.parse<0>(text);
	}
	catch (rapidxml::parse_error& failure)
	{
		const char* error_what = failure.what();
		char* error_where = failure.where<char>();

		Debug::error(failure.what());

		if (text != NULL)
			delete [] text;

		return false;
	}


	for (int i=0; i < 255; i++)
	{
		memset(&m_glyph[i], 0, sizeof(Glyph));
	}


	bool lowercase = false;

	xml_node<>* root = doc.first_node("font");

	// read root tag attributes
	xml_attribute<>* attr = root->first_attribute();
	while (attr != 0)
	{
		std::string attrname = std::string(attr->name());
		if (attrname == "lowercase")
		{
			std::string v = std::string(attr->value());
			if (v == "true")
				lowercase = true;
		}
		else if (attrname == "lineheight")
		{
			m_lineheight = strtol(attr->value(), NULL, 10);
		}
		else if (attrname == "hmargin")
		{
			m_hmargin = strtol(attr->value(), NULL, 10);
		}
		attr = attr->next_attribute();
	}
	
	// loop through subnodes
	xml_node<>* node = root->first_node();
	while (node != NULL)
	{
		std::string nodename = std::string(node->name());

		if (nodename == "sheet")
		{
			xml_attribute<>* attr = node->first_attribute("src");
			if (attr != NULL)
			{
				if (m_texture != NULL)
				{
					delete m_texture;
				}
				m_texture = new Texture();
				m_texture->load(attr->value());
			}
		}
		else if (nodename == "glyph")
		{
			xml_attribute<>* attr = node->first_attribute();

			int ascii;
			int x, y, width, height;
			int useunderspace = 0;
			int underspace = 0;

			while (attr != NULL)
			{
				std::string attrname = std::string(attr->name());
				if (attrname == "ascii")
				{
					ascii = strtol(attr->value(), NULL, 16);
				}
				else if (attrname == "x")
				{
					x = strtol(attr->value(), NULL, 10);
				}
				else if (attrname == "y")
				{
					y = strtol(attr->value(), NULL, 10);
				}
				else if (attrname == "width")
				{
					width = strtol(attr->value(), NULL, 10);
				}
				else if (attrname == "height")
				{
					height = strtol(attr->value(), NULL, 10);
				}
				else if (attrname == "useunderspace")
				{
					useunderspace = strtol(attr->value(), NULL, 10);
				}
				else if (attrname == "underspace")
				{
					underspace = strtol(attr->value(), NULL, 10);
				}

				attr = attr->next_attribute();
			}

			assert(ascii >= 0 && ascii <= 255);

			Glyph* glyph = &m_glyph[ascii & 0xff];
			glyph->x = (float)x;
			glyph->y = (float)y;
			glyph->width = (float)width;
			glyph->height = (float)height;
			glyph->use_underspace = useunderspace ? true : false;
			glyph->underspace = underspace;
		}

		node = node->next_sibling();
	}

	// if no lowercase, copy uppercase chars to matching lower ones
	if (!lowercase)
	{
		for (int i=0; i < 26; i++)
		{
			int from = i + 0x41;
			int to = i + 0x61;
			memcpy(&m_glyph[to], &m_glyph[from], sizeof(Glyph));
		}
	}

	if (m_texture != NULL)
	{
		Atlas* atlas = m_texture->getAtlas();
		int atlas_width = atlas->getWidth();
		int atlas_height = atlas->getHeight();

		// process UV coords for each glyph
		for (int i=0; i < 255; i++)
		{
			Glyph* glyph = &m_glyph[i];

			glyph->u1 = (float)(glyph->x) / (float)(atlas_width);
			glyph->u2 = glyph->u1 + ((float)(glyph->width) / (float)(atlas_width));
			glyph->v1 = (float)(glyph->y) / (float)(atlas_height);
			glyph->v2 = glyph->v1 + ((float)(glyph->height) / (float)(atlas_height));
		}
	}

	if (text != NULL)
		delete [] text;

	// scale to desired size
	float scale = (float)(size) / (float)(m_lineheight);
	for (int i=0; i < 255; i++)
	{
		Glyph* glyph = &m_glyph[i];
		glyph->width *= scale;
		glyph->height *= scale;
		glyph->underspace *= scale;
	}

	m_lineheight = size;

	return true;
}

const Font::Glyph* Font::getGlyph(int code)
{
	assert(code >= 0 && code <= 255);

	return &m_glyph[code & 0xff];
}

int Font::getLineHeight()
{
	return m_lineheight;
}

int Font::getHMargin()
{
	return m_hmargin;
}

void Font::apply(int unit)
{
	m_texture->apply(unit);
}