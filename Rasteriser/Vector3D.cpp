#include "Vector3D.h"

Vector3D::Vector3D()
{

}

Vector3D::Vector3D(float x, float y, float z)
{
	_x = x;
	_y = y;
	_z = z;
}

//Copy Constructor
Vector3D::Vector3D(const Vector3D& v)
{
	_x = v.GetX();
	_y = v.GetY();
	_z = v.GetZ();
}

Vector3D::~Vector3D()
{

}

//Accessors and mutators
float Vector3D::GetX() const
{
	return _x;
}

void Vector3D::SetX(const float x)
{
	_x = x;
}

float Vector3D::GetY() const
{
	return _y;
}

void Vector3D::SetY(const float y)
{
	_y = y;
}

float Vector3D::GetZ() const
{
	return _z;
}

void Vector3D::SetZ(const float z)
{
	_z = z;
}


Vector3D& Vector3D::operator+ (const Vector3D &rhs)
{
	_x = _x + rhs.GetX();
	_y = _y + rhs.GetY();
	_z = _z + rhs.GetZ();
	return *this;
}

Vector3D& Vector3D::operator/ (const float d)
{
	_x = _x / d;
	_y = _x / d;
	_z = _z / d;
	return *this;
}

Vector3D& Vector3D::operator= (const Vector3D &rhs)
{
	_x = rhs.GetX();
	_y = rhs.GetY();
	_z = rhs.GetZ();
	return *this;
}

Vector3D& Vector3D::operator*(const float m)
{
	_x = _x * m;
	_y = _y * m;
	_z = _z * m;
	return *this;
}

Vector3D& Vector3D::operator- (const Vector3D& rhs)
{
	_x = _x - rhs.GetX();
	_y = _y - rhs.GetY();
	_z = _z - rhs.GetZ();
	return *this;
}


const float Vector3D::Dot(const Vector3D& b)
{
	return _x * b.GetX() + _y * b.GetY() + _z * b.GetZ();
}

const Vector3D Vector3D::Cross(const Vector3D& a)
{
	return Vector3D(_y * a.GetZ() - _z * a.GetY(), _z * a.GetX() - _x * a.GetZ(), _x * a.GetY() - _y * a.GetX());
}

float Vector3D::Magnitude()
{
	return (sqrt(pow(_x, 2) + pow(_y, 2) + pow(_z, 2)));
}

Vector3D Vector3D::Normalise()
{
	return Vector3D(_x, _y, _z) / Magnitude();
}