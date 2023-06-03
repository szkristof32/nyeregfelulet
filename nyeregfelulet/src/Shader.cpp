#include "Shader.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <cassert>
#include <filesystem>
#include <iostream>
#include <fstream>

namespace Utils {

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		assert(false && "Unknown shader type!");
		return 0;
	}

	static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
	{
		switch (stage)
		{
			case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
			case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
		}
		assert(false);
		return (shaderc_shader_kind)0;
	}

	static const char* GLShaderStageToString(GLenum stage)
	{
		switch (stage)
		{
			case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
			case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
		}
		assert(false);
		return nullptr;
	}

	static const char* GetCacheDirectory()
	{
		// TODO: make sure the assets directory is valid
		return "assets/cache/shader/opengl";
	}

	static void CreateCacheDirectoryIfNeeded()
	{
		std::string cacheDirectory = GetCacheDirectory();
		if (!std::filesystem::exists(cacheDirectory))
			std::filesystem::create_directories(cacheDirectory);
	}

	static const char* GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
	{
		switch (stage)
		{
			case GL_VERTEX_SHADER:    return ".cached_opengl.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_opengl.frag";
		}
		assert(false);
		return "";
	}

	static const char* GLShaderStageCachedVulkanFileExtension(uint32_t stage)
	{
		switch (stage)
		{
			case GL_VERTEX_SHADER:    return ".cached_vulkan.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_vulkan.frag";
		}
		assert(false);
		return "";
	}

}

Shader::Shader(const std::string& filepath)
	: m_FilePath(filepath)
{
	Utils::CreateCacheDirectoryIfNeeded();

	std::string source = ReadFile(filepath);
	auto shaderSources = PreProcess(source);

	{
		CompileOrGetOpenGLBinaries(shaderSources);
		CreateProgram();
	}

	// Extract name from filepath
	auto lastSlash = filepath.find_last_of("/\\");
	lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
	auto lastDot = filepath.rfind('.');
	auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
	m_Name = filepath.substr(lastSlash, count);
}

Shader::~Shader()
{
	glDeleteProgram(m_InternalId);
}

std::string Shader::ReadFile(const std::string& filepath)
{
	std::string result;
	std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
	if (in)
	{
		in.seekg(0, std::ios::end);
		size_t size = in.tellg();
		if (size != -1)
		{
			result.resize(size);
			in.seekg(0, std::ios::beg);
			in.read(&result[0], size);
		}
		else
		{
			std::cerr << "Could not read from file '" << filepath << "'\n";
		}
	}
	else
	{
		std::cerr << "Could not open file '" << filepath << "'\n";
	}

	return result;
}

std::unordered_map<GLenum, std::string> Shader::PreProcess(const std::string& source)
{
	std::unordered_map<GLenum, std::string> shaderSources;

	const char* typeToken = "#type";
	size_t typeTokenLength = strlen(typeToken);
	size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
	while (pos != std::string::npos)
	{
		size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
		assert(eol != std::string::npos && "Syntax error");
		size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
		std::string type = source.substr(begin, eol - begin);
		assert(Utils::ShaderTypeFromString(type) && "Invalid shader type specified");

		size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
		assert(nextLinePos != std::string::npos && "Syntax error");
		pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

		shaderSources[Utils::ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
	}

	return shaderSources;
}

void Shader::CompileOrGetOpenGLBinaries(const std::unordered_map<GLenum, std::string>& shaderSources)
{
	shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
	const bool optimize = false;
	if (optimize)
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

	std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

	auto& shaderData = m_OpenGLSPIRV;
	shaderData.clear();
	for (auto&& [stage, source] : shaderSources)
	{
		std::filesystem::path shaderFilePath = m_FilePath;
		std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedOpenGLFileExtension(stage));

		std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
		if (in.is_open())
		{
			in.seekg(0, std::ios::end);
			auto size = in.tellg();
			in.seekg(0, std::ios::beg);

			auto& data = shaderData[stage];
			data.resize(size / sizeof(uint32_t));
			in.read((char*)data.data(), size);
		}
		else
		{
			shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);
			if (module.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				std::cerr << module.GetErrorMessage();
				assert(false);
			}

			shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

			std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
			if (out.is_open())
			{
				auto& data = shaderData[stage];
				out.write((char*)data.data(), data.size() * sizeof(uint32_t));
				out.flush();
				out.close();
			}
		}
	}
}

void Shader::CreateProgram()
{
	GLuint program = glCreateProgram();

	std::vector<GLuint> shaderIDs;
	for (auto&& [stage, spirv] : m_OpenGLSPIRV)
	{
		GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
		glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
		glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
		glAttachShader(program, shaderID);
	}

	glLinkProgram(program);

	GLint isLinked;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
		std::cerr << "Shader linking failed (" << m_FilePath << "):\n" << infoLog.data() << "\n";

		glDeleteProgram(program);

		for (auto id : shaderIDs)
			glDeleteShader(id);
	}

	for (auto id : shaderIDs)
	{
		glDetachShader(program, id);
		glDeleteShader(id);
	}

	m_InternalId = program;
}

void Shader::Start() const
{
	glUseProgram(m_InternalId);
}

void Shader::Stop() const
{
	glUseProgram(0);
}

GLint Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocations.find(name) != m_UniformLocations.end())
	{
		return m_UniformLocations.at(name);
	}

	GLint location = glGetUniformLocation(m_InternalId, name.c_str());
	m_UniformLocations[name] = location;
	return location;
}

void Shader::LoadInt(const std::string& name, int value)
{
	GLint location = GetUniformLocation(name);
	glUniform1i(location, value);
}

void Shader::LoadFloat(const std::string& name, float value)
{
	GLint location = GetUniformLocation(name);
	glUniform1f(location, value);
}

void Shader::LoadBool(const std::string& name, bool value)
{
	LoadFloat(name, value);
}

void Shader::LoadTextureUnit(const std::string& name, int unit)
{
	LoadInt(name, unit);
}

void Shader::LoadVec2(const std::string& name, const glm::vec2& vector)
{
	LoadVec2(name, vector.x, vector.y);
}

void Shader::LoadVec2(const std::string& name, float x, float y)
{
	GLint location = GetUniformLocation(name);
	glUniform2f(location, x, y);
}

void Shader::LoadVec3(const std::string& name, const glm::vec3& vector)
{
	LoadVec3(name, vector.x, vector.y, vector.z);
}

void Shader::LoadVec3(const std::string& name, float x, float y, float z)
{
	GLint location = GetUniformLocation(name);
	glUniform3f(location, x, y, z);
}

void Shader::LoadVec4(const std::string& name, const glm::vec4& vector)
{
	LoadVec4(name, vector.x, vector.y, vector.z, vector.w);
}

void Shader::LoadVec4(const std::string& name, float x, float y, float z, float w)
{
	GLint location = GetUniformLocation(name);
	glUniform4f(location, x, y, z, w);
}

void Shader::LoadMat4(const std::string& name, const glm::mat4& matrix)
{
	GLint location = GetUniformLocation(name);
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(matrix));
}