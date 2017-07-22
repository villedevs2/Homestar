#include "Shaders.h"

using namespace std;

std::string Shaders::loadShader(std::string filename)
{
	BinaryFile input;
	char* bytes = NULL;
	std::string shader = "";

	try
	{
		if (!input.open(filename))
			throw ios_base::failure("Engine::loadShader(): couldn't open shader file.");

		int length = input.get_length();
		bytes = new char[length+1];
		memset(bytes, 0, length+1);
		input.read(bytes, length);

		shader = bytes;
		delete [] bytes;
	}
	catch (ios_base::failure& failure)
	{
		input.close();

		if (bytes != NULL)
		{
			delete [] bytes;
			bytes = NULL;
		}
		return shader;
	}

	input.close();
	return shader;
}

GLuint Shaders::loadShaderProgram(std::string vs_filename, std::string fs_filename)
{
	GLenum error;
	GLuint program = 0;

	try
	{
		GLsizei length;
 		GLchar name[1000];

		std::string default_vs_code = Shaders::loadShader(vs_filename);
		std::string default_fs_code = Shaders::loadShader(fs_filename);

		if (default_vs_code.length() <= 0)
			throw std::string("Shaders::loadShaderProgram(): load vertex shader failed.");
		if (default_fs_code.length() <= 0)
			throw std::string("Shaders::loadShaderProgram(): load fragment shader failed.");

		const char *vert_code, *frag_code;

		GLuint default_vert_shader = glCreateShader(GL_VERTEX_SHADER);
		vert_code = default_vs_code.c_str();
		glShaderSource(default_vert_shader, 1, &vert_code, NULL);
		glCompileShader(default_vert_shader);

		glGetShaderInfoLog(default_vert_shader, 1000, &length, name);
		if (length > 0)
		{
			Debug::error(str(fmt::Format("{:s} compilation errors") << vs_filename));
			Debug::error(str(fmt::Format("{:s}") << name));
		}

		error = glGetError();
		if (error != GL_NO_ERROR)
			throw std::string("Shaders::loadShaderProgram(): compile vertex shader failed.");

		GLuint default_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
		frag_code = default_fs_code.c_str();
		glShaderSource(default_frag_shader, 1, &frag_code, NULL);
		glCompileShader(default_frag_shader);

		glGetShaderInfoLog(default_frag_shader, 1000, &length, name);
		if (length > 0)
		{
			Debug::error(str(fmt::Format("{:s} compilation errors") << fs_filename));
			Debug::error(str(fmt::Format("{:s}") << name));
		}

		error = glGetError();
		if (error != GL_NO_ERROR)
			throw std::string("Shaders::loadShaderProgram(): compile fragment shader failed.");
	
		program = glCreateProgram();
		error = glGetError();
		if (error != GL_NO_ERROR)
			throw std::string("Shaders::loadShaderProgram(): create program failed.");

		glAttachShader(program, default_vert_shader);
		glAttachShader(program, default_frag_shader);
		glLinkProgram(program);

		error = glGetError();
		if (error != GL_NO_ERROR)
			throw std::string("Shaders::loadShaderProgram(): link program failed.");

		glGetProgramInfoLog(program, 1000, &length, name);
		if (length > 0)
		{
			Debug::error(str(fmt::Format("{:s}/{:s} link errors") << vs_filename << fs_filename));
			Debug::error(str(fmt::Format("{:s}") << name));
		}
	}
	catch (std::string& error_string)
	{
		Debug::error(error_string);
	}

	return program;
}
