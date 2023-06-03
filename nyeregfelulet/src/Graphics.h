#pragma once

#include <glad/glad.h>
#include <stdint.h>

class VertexBuffer
{
public:
	VertexBuffer(uint32_t size, GLuint usage);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;

	void StoreData(const void* data, uint32_t size);
private:
	GLuint m_Id;
	GLuint m_Usage;
	uint32_t m_AllocatedSize;
};

class IndexBuffer
{
public:
	IndexBuffer(uint32_t size, GLuint usage);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	void StoreData(const void* data, uint32_t size);
private:
	GLuint m_Id;
	GLuint m_Usage;
	uint32_t m_AllocatedSize;
};

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void Bind() const;
	void Unbind() const;
private:
	GLuint m_Id;
};