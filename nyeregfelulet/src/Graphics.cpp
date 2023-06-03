#include "Graphics.h"

VertexBuffer::VertexBuffer(uint32_t size, GLuint usage)
	: m_AllocatedSize(size), m_Usage(usage)
{
	glCreateBuffers(1, &m_Id);
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, m_Usage);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_Id);
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_Id);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::StoreData(const void* data, uint32_t size)
{
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

IndexBuffer::IndexBuffer(uint32_t size, GLuint usage)
	: m_AllocatedSize(size), m_Usage(usage)
{
	glCreateBuffers(1, &m_Id);
	Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, m_Usage);
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &m_Id);
}

void IndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
}

void IndexBuffer::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::StoreData(const void* data, uint32_t size)
{
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data);
}

VertexArray::VertexArray()
{
	glCreateVertexArrays(1, &m_Id);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_Id);
}

void VertexArray::Bind() const
{
	glBindVertexArray(m_Id);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);
}