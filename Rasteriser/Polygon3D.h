#pragma once
#include "Vector3D.h"
#include "Vertex.h"
#include "stdafx.h"

class Polygon3D
{
public:
	Polygon3D();
	Polygon3D(int index0, int index1, int index2);
	Polygon3D(int index0, int index1, int index2, Vector3D normal, float averageZ, Vector3D worldNormal, Vertex centroid);
	~Polygon3D();
	Polygon3D(const Polygon3D& p);

	//Accessor to return index of specified vertex
	int GetIndex(int) const;

	//Accessors and mutators
	bool GetCull() const;
	void SetCull(const bool cull);
	Vector3D GetNormal() const;
	void SetNormal(const Vector3D &other);
	float GetAverageZ() const;
	void SetAverageZ(const float averageZ);
	COLORREF GetColour() const;
	void SetColour(const COLORREF colour);
	Vector3D GetWorldNormal() const;
	void SetWorldNormal(const Vector3D& other);
	Vertex GetCentroid() const;
	void SetCentroid(const Vertex& vrt);

	//Operator overloads
	Polygon3D& operator= (const Polygon3D &rhs);

	//Methods
	void PixelFill(const HDC& hdc, const float& x, const float& y, const int& r, const int& g, const int& b);

private:
	int _indices[3];
	bool _cull;
	Vector3D _normal;
	float _averageZ;
	COLORREF _colour;
	Vector3D _worldNormal;
	Vertex _centroid;
};
