#pragma once

#include "Graphics.h"

#include <glm/glm.hpp>

#include <vector>

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
};

class NyeregGenerator
{
public:
	NyeregGenerator(uint32_t density);
	~NyeregGenerator() = default;

	const VertexArray& GetVertexArray() const { return m_VertexArray; }
	const IndexBuffer& GetIndexBuffer() const { return m_IndexBuffer; }
	uint32_t GetVertexCount() const { return (uint32_t)m_Indices.size(); }
private:
	void Generate(uint32_t density);
	float Nyereg(const glm::vec2& point);
private:
	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;

	VertexArray m_VertexArray;
	VertexBuffer m_VertexBuffer;
	IndexBuffer m_IndexBuffer;
};