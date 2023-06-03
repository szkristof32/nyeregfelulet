#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>
#include <vector>

class Shader
{
public:
	Shader(const std::string& filepath);
	virtual ~Shader();

	void Start() const;
	void Stop() const;

	void LoadInt(const std::string& name, int value);
	void LoadFloat(const std::string& name, float value);
	void LoadBool(const std::string& name, bool value);
	void LoadTextureUnit(const std::string& name, int unit);
	void LoadVec2(const std::string& name, const glm::vec2& vector);
	void LoadVec2(const std::string& name, float x, float y);
	void LoadVec3(const std::string& name, const glm::vec3& vector);
	void LoadVec3(const std::string& name, float x, float y, float z);
	void LoadVec4(const std::string& name, const glm::vec4& vector);
	void LoadVec4(const std::string& name, float x, float y, float z, float w);
	void LoadMat4(const std::string& name, const glm::mat4& matrix);
private:
	std::string ReadFile(const std::string& filepath);
	std::unordered_map<uint32_t, std::string> PreProcess(const std::string& source);

	void CompileOrGetOpenGLBinaries(const std::unordered_map<uint32_t, std::string>& shaderSources);
	void CreateProgram();

	int GetUniformLocation(const std::string& name);
private:
	uint32_t m_InternalId;
	std::string m_FilePath;
	std::string m_Name;

	std::unordered_map<std::string, uint32_t> m_UniformLocations;
	std::unordered_map<uint32_t, std::vector<uint32_t>> m_OpenGLSPIRV;
};