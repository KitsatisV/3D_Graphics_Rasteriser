#include "Model.h"

Model::Model()
{
	_polygons.clear();
	_vertices.clear();
	_transform.clear();
	_worldTransform.clear();
}


//Copy Constructor
Model::Model(const Model& m)
{
	_polygons = m.GetPolygons();
	_vertices = m.GetVertices();
	_transform = m.GetTransform();
	_worldTransform = m.GetWorldTransform();
}

Model::~Model()
{

}

//Accessors and mutators
const std::vector<Polygon3D>& Model::GetPolygons() const
{
	return _polygons;
}

const std::vector<Vertex>& Model::GetVertices() const
{
	return _vertices;
}

const std::vector<Vertex>& Model::GetTransform() const
{
	return _transform;
}

const std::vector<Vertex>& Model::GetWorldTransform() const
{
	return _worldTransform;
}

const void Model::SetWorldTransform()
{
	_worldTransform = _transform;
}


size_t Model::GetPolygonCount() const
{
	return _polygons.size();
}

size_t Model::GetVertexCount() const
{
	return _vertices.size();
}

void Model::AddVertex(float x, float y, float z)
{
	Vertex temp(x, y, z, 1);
	_vertices.push_back(temp);
}

void Model::AddPolygon(int i0, int i1, int i2)
{
	Polygon3D temp(i0, i1, i2);
	_polygons.push_back(temp);
}

void Model::ApplyTransformToLocalVertices(const Matrix &transform)
{
	_transform.clear();
	int verticesSize = int(_vertices.size());
	for (int i = 0; i < verticesSize; i++)
	{
		_transform.push_back(transform * _vertices[i]);
	}
}

void Model::ApplyTransformToTransformedVertices(const Matrix &transform)
{
	int transformSize = int(_transform.size());
	for (int i = 0; i < transformSize; i++)
	{
		_transform[i] = transform * _transform[i];
	}
}

void Model::Dehomogenize()
{
	int transformSize = int(_transform.size());
	for (int i = 0; i < transformSize; i++)
	{
		_transform[i].Dehomogenize();
	}
}

void Model::CalculateBackfaces(Camera camera)
{
	for (int i = 0; i < GetPolygonCount(); i++)
	{
		//Getting the polygon's vertices
		Vertex tempVertex0 = GetTransform()[GetPolygons()[i].GetIndex(0)];
		Vertex tempVertex1 = GetTransform()[GetPolygons()[i].GetIndex(1)];
		Vertex tempVertex2 = GetTransform()[GetPolygons()[i].GetIndex(2)];

		//Two vectors, from each polygon, are enough, in order to calculate the polygon's normal vector,
		//by using the cross product of the two
		Vector3D tempVector1 = Vector3D(tempVertex0.GetX() - tempVertex1.GetX(), tempVertex0.GetY() - tempVertex1.GetY(), tempVertex0.GetZ() - tempVertex1.GetZ());
		Vector3D tempVector2 = Vector3D(tempVertex1.GetX() - tempVertex2.GetX(), tempVertex1.GetY() - tempVertex2.GetY(), tempVertex1.GetZ() - tempVertex2.GetZ());
		Vector3D normalVector = tempVector1.Cross(tempVector2);

		//Creating an eye-vector, by subtracting the camera position from vertex0
		Vector3D eyeVector = Vector3D(tempVertex0.GetX() - camera.GetPos().GetX(), tempVertex0.GetY() - camera.GetPos().GetY(), tempVertex0.GetZ() - camera.GetPos().GetZ());

		// Calculate the dot product and check it
		// If <0 the mark the polygon as culled
		float dotProduct = normalVector.Dot(eyeVector);
		if (dotProduct <= 0)
		{
			//Flag this polygon for culling
			_polygons[i].SetCull(true);
		}
		else
		{
			//Flag the polygon to not be culled any more, so it can be displayed when it rotates round
			_polygons[i].SetCull(false);
		}
	}
}

void Model::Sort()
{
	std::sort(_polygons.begin(), _polygons.end(), [](Polygon3D a, Polygon3D b) {	return (a.GetAverageZ() < b.GetAverageZ());	});
}

//std::vector<Polygon3D> Model::Merge(std::vector<Polygon3D> left, std::vector<Polygon3D> right)
//{
//	// Initialize a result vector
//	std::vector<Polygon3D> result;
//
//	// Loop until one of the lists is empty
//	while (!left.empty() && !right.empty())
//	{
//		// Compare the first elements of each list
//		// and append the smaller one to the result
//		if (left.front().GetAverageZ() < right.front().GetAverageZ())
//		{
//			result.push_back(left.front());
//			left.erase(left.begin());
//		}
//		else
//		{
//			result.push_back(right.front());
//			right.erase(right.begin());
//		}
//	}
//
//	// Append the remaining elements of the non-empty list
//	while (!left.empty())
//	{
//		result.push_back(left.front());
//		left.erase(left.begin());
//	}
//	while (!right.empty())
//	{
//		result.push_back(right.front());
//		right.erase(right.begin());
//	}
//
//	return result;
//}
//
//// Recursive function for merge sort
//std::vector<Polygon3D> Model::Mergesort(std::vector<Polygon3D> arr)
//{
//	// If the input list has fewer than 2 elements, it is already sorted
//	if (arr.size() < 2)
//		return arr;
//
//	// Find the middle index of the list
//	int mid = arr.size() / 2;
//
//	// Split the list into two smaller lists
//	std::vector<Polygon3D> left(arr.begin(), arr.begin() + mid);
//	std::vector<Polygon3D> right(arr.begin() + mid, arr.end());
//
//	// Sort the left and right sides, using threads
//	left = Mergesort(left);
//	right = Mergesort(right);
//
//	// Merge the sorted left and right sides
//	return Merge(left, right);
//}

void Model::CalculatePolygonAverageZ()
{
	std::vector<Vertex> vertices;
	if (GetPolygonCount() > 0)
	{
		for (int i = 0; i < _polygons.size(); i++)
		{
			vertices.push_back(_transform[_polygons[i].GetIndex(0)]);
			vertices.push_back(_transform[_polygons[i].GetIndex(1)]);
			vertices.push_back(_transform[_polygons[i].GetIndex(2)]);
			
			_polygons[i].SetAverageZ((vertices[0].GetZ() + vertices[1].GetZ() + vertices[2].GetZ()) / 3.0f);

			vertices.clear();
		}
	}
}

std::vector<Vertex> Model::GetPolygonVerticesSorted(Polygon3D poly)
{
	std::vector<Vertex> vertices;
	vertices.push_back(_transform[poly.GetIndex(0)]);
	vertices.push_back(_transform[poly.GetIndex(1)]);
	vertices.push_back(_transform[poly.GetIndex(2)]);


	std::sort(vertices.begin(), vertices.end(), [](Vertex a, Vertex b) {	if (a.GetY() == b.GetY()) { return (a.GetX() < b.GetX()); }	return (a.GetY() > b.GetY());	});

	return vertices;
}

std::vector<Vertex> Model::GetPolygonWorldVerticesSorted(Polygon3D poly)
{
	std::vector<Vertex> vertices;
	vertices.push_back(_worldTransform[poly.GetIndex(0)]);
	vertices.push_back(_worldTransform[poly.GetIndex(1)]);
	vertices.push_back(_worldTransform[poly.GetIndex(2)]);


	std::sort(vertices.begin(), vertices.end(), [](Vertex a, Vertex b) {	if (a.GetY() == b.GetY()) { return (a.GetX() < b.GetX()); }	return (a.GetY() > b.GetY());	});

	return vertices;
}

void Model::CalculatePolygonNormals()
{
	//Calculating normals for the fully transformed polygons
	for (int i = 0; i < GetPolygonCount(); i++)
	{
		//Getting the polygon's vertices' normals
		std::vector<Vector3D> normals = CalculateVerticesNormalInPolygon(_polygons[i]);

		//The normal of this polygon is the average normal of its vertices
		_polygons[i].SetNormal(Vector3D((normals[0].GetX() + normals[1].GetX() + normals[2].GetX()) / 3, (normals[0].GetY() + normals[1].GetY() + normals[2].GetY()) / 3, (normals[0].GetZ() + normals[1].GetZ() + normals[2].GetZ()) / 3));
	}

	//Calculating normals for the world transformed polygons
	for (int i = 0; i < GetPolygonCount(); i++)
	{
		//Getting the polygon's vertices' normals
		std::vector<Vector3D> normals = CalculateWorldVerticesNormalInPolygon(_polygons[i]);

		//The normal of this polygon is the average normal of its vertices
		_polygons[i].SetWorldNormal(Vector3D((normals[0].GetX() + normals[1].GetX() + normals[2].GetX()) / 3, (normals[0].GetY() + normals[1].GetY() + normals[2].GetY()) / 3, (normals[0].GetZ() + normals[1].GetZ() + normals[2].GetZ()) / 3));
	}
}

void Model::CalculatePolygonCentroid()
{
	for (int i = 0; i < _polygons.size(); i++)
	{
		std::vector<Vertex> vertices = GetPolygonVerticesSorted(_polygons[i]);
		float x = (vertices[0].GetX() + vertices[1].GetX() + vertices[2].GetX()) / 3;
		float y = (vertices[0].GetY() + vertices[1].GetY() + vertices[2].GetY()) / 3;
		float z = (vertices[0].GetZ() + vertices[1].GetZ() + vertices[2].GetZ()) / 3;
		float w = (vertices[0].GetW() + vertices[1].GetW() + vertices[2].GetW()) / 3;

		_polygons[i].SetCentroid(Vertex(x, y, z, w));
	}
}

void Model::CalculateVertexNormals()
{
	CalculatePolygonNormals();
	for (int i = 0; i < _transform.size(); i++)
	{
		std::vector<Vector3D> normals;

		for each (Polygon3D poly in _polygons)
		{
			if (_transform[i] == _transform[poly.GetIndex(0)] || _transform[i] == _transform[poly.GetIndex(1)] || _transform[i] == _transform[poly.GetIndex(2)])
				normals.push_back(poly.GetWorldNormal());
		}

		float x = 0;
		float y = 0;
		float z = 0;

		for each (Vector3D vector in normals)
		{
			x = x + vector.GetX();
			y = y + vector.GetY();
			z = z + vector.GetZ();
		}
		
		x = x / (int)normals.size();
		y = y / (int)normals.size();
		z = z / (int)normals.size();

		_transform[i].SetNormal(Vector3D(x, y, z));
	}
}

std::vector<Vector3D> Model::CalculateVerticesNormalInPolygon(const Polygon3D& poly)
{
		std::vector<Vector3D> normals;

		//Getting the polygon's vertices
		std::vector<Vertex> vertices;
		vertices.push_back(_transform[poly.GetIndex(0)]);
		vertices.push_back(_transform[poly.GetIndex(1)]);
		vertices.push_back(_transform[poly.GetIndex(2)]);

		//Getting the two vectors from each vertex to the other two vertices
		Vector3D Vector01 = Vector3D(vertices[1].GetX() - vertices[0].GetX(), vertices[1].GetY() - vertices[0].GetY(), vertices[1].GetZ() - vertices[0].GetZ());
		Vector3D Vector02 = Vector3D(vertices[2].GetX() - vertices[0].GetX(), vertices[2].GetY() - vertices[0].GetY(), vertices[2].GetZ() - vertices[0].GetZ());

		Vector3D Vector10 = Vector3D(vertices[0].GetX() - vertices[1].GetX(), vertices[0].GetY() - vertices[1].GetY(), vertices[0].GetZ() - vertices[1].GetZ());
		Vector3D Vector12 = Vector3D(vertices[2].GetX() - vertices[1].GetX(), vertices[2].GetY() - vertices[1].GetY(), vertices[2].GetZ() - vertices[1].GetZ());

		Vector3D Vector20 = Vector3D(vertices[0].GetX() - vertices[2].GetX(), vertices[0].GetY() - vertices[2].GetY(), vertices[0].GetZ() - vertices[2].GetZ());
		Vector3D Vector21 = Vector3D(vertices[1].GetX() - vertices[2].GetX(), vertices[1].GetY() - vertices[2].GetY(), vertices[1].GetZ() - vertices[2].GetZ());

		//Setting each vertex's normal
		normals.push_back(Vector01.Cross(Vector02));
		normals.push_back(Vector10.Cross(Vector12));
		normals.push_back(Vector20.Cross(Vector21));

		return normals;
}

std::vector<Vector3D> Model::CalculateWorldVerticesNormalInPolygon(const Polygon3D& poly)
{
	std::vector<Vector3D> normals;

	//Getting the polygon's vertices
	std::vector<Vertex> vertices;
	vertices.push_back(_worldTransform[poly.GetIndex(0)]);
	vertices.push_back(_worldTransform[poly.GetIndex(1)]);
	vertices.push_back(_worldTransform[poly.GetIndex(2)]);

	//Getting the two vectors from each vertex to the other two vertices
	Vector3D Vector01 = Vector3D(vertices[1].GetX() - vertices[0].GetX(), vertices[1].GetY() - vertices[0].GetY(), vertices[1].GetZ() - vertices[0].GetZ());
	Vector3D Vector02 = Vector3D(vertices[2].GetX() - vertices[0].GetX(), vertices[2].GetY() - vertices[0].GetY(), vertices[2].GetZ() - vertices[0].GetZ());

	Vector3D Vector10 = Vector3D(vertices[0].GetX() - vertices[1].GetX(), vertices[0].GetY() - vertices[1].GetY(), vertices[0].GetZ() - vertices[1].GetZ());
	Vector3D Vector12 = Vector3D(vertices[2].GetX() - vertices[1].GetX(), vertices[2].GetY() - vertices[1].GetY(), vertices[2].GetZ() - vertices[1].GetZ());

	Vector3D Vector20 = Vector3D(vertices[0].GetX() - vertices[2].GetX(), vertices[0].GetY() - vertices[2].GetY(), vertices[0].GetZ() - vertices[2].GetZ());
	Vector3D Vector21 = Vector3D(vertices[1].GetX() - vertices[2].GetX(), vertices[1].GetY() - vertices[2].GetY(), vertices[1].GetZ() - vertices[2].GetZ());

	//Setting each vertex's normal
	normals.push_back(Vector01.Cross(Vector02));
	normals.push_back(Vector10.Cross(Vector12));
	normals.push_back(Vector20.Cross(Vector21));

	return normals;
}