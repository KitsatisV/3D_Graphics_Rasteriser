#include "Polygon3D.h"

Polygon3D::Polygon3D()
{
	_cull = false;
	_averageZ = 0;
}

Polygon3D::Polygon3D(int index0, int index1, int index2)
{
	_indices[0] = index0;
	_indices[1] = index1;
	_indices[2] = index2;
	_cull = false;
}

Polygon3D::Polygon3D(int index0, int index1, int index2, Vector3D normal, float averageZ, Vector3D worldNormal, Vertex centroid)
{
	_indices[0] = index0;
	_indices[1] = index1;
	_indices[2] = index2;
	_cull = false;
	_normal = normal;
	_averageZ = averageZ;
	_worldNormal = worldNormal;
	_centroid = centroid;
}

Polygon3D::~Polygon3D()
{

}

//Copy Constructor
Polygon3D::Polygon3D(const Polygon3D& p)
{
	_indices[0] = p.GetIndex(0);
	_indices[1] = p.GetIndex(1);
	_indices[2] = p.GetIndex(2);
	_cull = p.GetCull();
	_normal = p.GetNormal();
	_averageZ = p.GetAverageZ();
	_colour = p.GetColour();
	_worldNormal = p.GetWorldNormal();
	_centroid = p.GetCentroid();
}

//Accessor to return index of specified vertex
int Polygon3D::GetIndex(int i) const
{
	return _indices[i];
}

//Accessors and mutators
bool Polygon3D::GetCull() const
{
	return _cull;
}

void Polygon3D::SetCull(const bool cull)
{
	_cull = cull;
}

Vector3D Polygon3D::GetNormal() const
{
	return _normal;
}

void Polygon3D::SetNormal(const Vector3D &other)
{
	_normal = other;
}

float Polygon3D::GetAverageZ() const
{
	return _averageZ;
}

void Polygon3D::SetAverageZ(const float averageZ)
{
	_averageZ = averageZ;
}

COLORREF Polygon3D::GetColour() const
{
	return _colour;
}

void Polygon3D::SetColour(const COLORREF colour)
{
	_colour = colour;
}

Vector3D Polygon3D::GetWorldNormal() const
{
	return _worldNormal;
}

void Polygon3D::SetWorldNormal(const Vector3D& other)
{
	_worldNormal = other;
}

Vertex Polygon3D::GetCentroid() const
{
	return _centroid;
}

void Polygon3D::SetCentroid(const Vertex& vrt)
{
	_centroid = vrt;
}

Polygon3D& Polygon3D::operator= (const Polygon3D &rhs)
{
	//Only do the assignment if we are not assigning
	//to ourselves
	if (this != &rhs)
	{
		_indices[0] = rhs.GetIndex(0);
		_indices[1] = rhs.GetIndex(1);
		_indices[2] = rhs.GetIndex(2);
		_cull = rhs.GetCull();
		_averageZ = rhs.GetAverageZ();
		_normal = rhs.GetNormal();
		_colour = rhs.GetColour();
		_worldNormal = rhs.GetWorldNormal();
		_centroid = rhs.GetCentroid();
	}
	return *this;
}

void Polygon3D::PixelFill(const HDC& hdc, const float& x, const float& y, const int& r, const int& g, const int& b)
{
	//
	_colour = RGB(r, g, b);
	SetPixel(hdc, x, y, _colour);
}
