#pragma once

#include <vector>
#include <algorithm>
#include <future>
#include <ppl.h>
#include "Polygon3D.h"
#include "Vertex.h"
#include "Matrix.h"
#include "Camera.h"
#include <algorithm>
#include "stdafx.h"
#include <math.h>



class Model
{
public:
	Model();
	Model(const Model& m);
	~Model();

	//Accessors and mutators
	const std::vector<Polygon3D>& GetPolygons() const;
	const std::vector<Vertex>& GetVertices()const;
	const std::vector<Vertex>& GetTransform()const;
	const std::vector<Vertex>& GetWorldTransform() const;
	const void SetWorldTransform();

	
	size_t GetPolygonCount() const;
	size_t GetVertexCount() const;

	void AddVertex(float x, float y, float z);
	void AddPolygon(int i0, int i1, int i2);

	void ApplyTransformToLocalVertices(const Matrix &transform);
	void ApplyTransformToTransformedVertices(const Matrix &transform);

	void Dehomogenize();

	void CalculateBackfaces(Camera camera);

	void Sort();

	std::vector<Polygon3D> Merge(std::vector<Polygon3D> left, std::vector<Polygon3D> right);

	void CalculatePolygonAverageZ();

	std::vector<Vertex> GetPolygonVerticesSorted(Polygon3D poly);

	std::vector<Vertex> GetPolygonWorldVerticesSorted(Polygon3D poly);

	std::vector<Polygon3D> Mergesort(std::vector<Polygon3D> arr);

	void CalculatePolygonNormals();

	void CalculatePolygonCentroid();

	void CalculateVertexNormals();

	std::vector<Vector3D> CalculateVerticesNormalInPolygon(const Polygon3D& poly);

	std::vector<Vector3D> CalculateWorldVerticesNormalInPolygon(const Polygon3D& poly);

private:
	std::vector<Polygon3D> _polygons;
	std::vector<Vertex> _vertices;
	std::vector<Vertex> _transform;
	std::vector<Vertex> _worldTransform;
};