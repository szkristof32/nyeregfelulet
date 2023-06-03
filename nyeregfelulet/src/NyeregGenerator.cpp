#include "NyeregGenerator.h"

#define VERTEX_COUNT(density) density * density * sizeof(Vertex)
#define INDEX_COUNT(density) (density - 1) * (density - 1) * 6 * sizeof(uint32_t)

NyeregGenerator::NyeregGenerator(uint32_t density)
	: m_VertexArray(), m_VertexBuffer(VERTEX_COUNT(density), GL_STREAM_DRAW), m_IndexBuffer(INDEX_COUNT(density), GL_STREAM_DRAW)
{
	Generate(density);

	m_VertexArray.Bind();

	m_VertexBuffer.StoreData(m_Vertices.data(), (uint32_t)m_Vertices.size() * sizeof(Vertex));
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (const void*)offsetof(Vertex, Position));
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (const void*)offsetof(Vertex, Normal));
	m_VertexBuffer.Unbind();

	m_IndexBuffer.Bind();
	m_IndexBuffer.StoreData(m_Indices.data(), (uint32_t)m_Indices.size() * sizeof(uint32_t));
	m_IndexBuffer.Unbind();
}

void NyeregGenerator::Generate(uint32_t density)
{
	for (uint32_t i = 0; i < density; i++)
	{
		float x = (float)i / (float)density * 2.0f - 1.0f;
		for (uint32_t j = 0; j < density; j++)
		{
			float y = (float)j / (float)density * 2.0f - 1.0f;
			float z = Nyereg({ x, y });

			Vertex vertex{};
			vertex.Position = { x, y, z };
			vertex.Normal = { 0, 1, 0 };
			m_Vertices.emplace_back(vertex);
		}
	}

	for (uint32_t i = 0; i < density - 1; i++)
	{
		for (uint32_t j = 0; j < density - 1; j++)
		{
			m_Indices.emplace_back(j * density + i);
			m_Indices.emplace_back(j * density + i + 1);
			m_Indices.emplace_back((j + 1) * density + i);
			m_Indices.emplace_back(j * density + i + 1);
			m_Indices.emplace_back((j + 1) * density + i);
			m_Indices.emplace_back((j + 1) * density + i + 1);
		}
	}
}

float NyeregGenerator::Nyereg(const glm::vec2& point)
{
	return point.x * point.x - point.y * point.y;
}