#pragma once

#include<vector>
#include "GL\glew.h"

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	unsigned int normalized;

	static const int GetSizeOfType(unsigned int type);
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;

public:

	VertexBufferLayout();
	~VertexBufferLayout();

	inline unsigned int VertexBufferLayout::GetStrinde() const
	{
		return m_Stride;
	}

	inline const std::vector<VertexBufferElement> VertexBufferLayout::GetElements() const &
	{
		return m_Elements;
	}

	template<typename T>
	void Push(unsigned int count);
};