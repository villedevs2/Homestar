#pragma once

#include <Platform.h>
#include <Debug.h>
#include "format/format.h"
#include "BinaryFile.h"
#include <ios>

namespace Shaders
{
	typedef struct
	{
		GLuint shader;

		GLuint position;
		GLuint tex_coord;
		GLuint color_sampler;
		GLuint location;
		GLuint scale;
		GLuint color;
		GLuint vp_matrix;
		GLuint rot_matrix;
	} GameShaderContext;

	typedef struct
	{
		GLuint shader;

		GLuint position;
		GLuint tex_coord;
		GLuint color_sampler;
		GLuint location;
		GLuint scale;
		GLuint vp_matrix;
		GLuint rot_matrix;
		GLuint transform;
		GLuint color;
	} UIShaderContext;

	typedef struct
	{
		GLuint shader;

		GLuint position;
		GLuint direction;
		GLuint texture;
		GLuint location;
		GLuint scale;
		GLuint color_sampler;
		GLuint vp_matrix;
		GLuint part_size;
	} ParticleShaderContext;

	typedef struct
	{
		GLuint shader;

		GLuint position;
		GLuint tex_coord;
		GLuint amb_coord;
		GLuint color;
		GLuint normal;
		GLuint vp_matrix;
		GLuint v_matrix;
		GLuint light;
		GLuint cam_pos;		// for env mapping
		GLuint color_sampler;
		GLuint amb_sampler;
	} LevelShaderContext;

	std::string loadShader(std::string filename);
	GLuint loadShaderProgram(std::string vs_filename, std::string fs_filename);
};
