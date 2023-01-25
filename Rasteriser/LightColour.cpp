#include "LightColour.h"

LightColour::LightColour()
{
}

LightColour::LightColour(int r, int g, int b)
{
	_r = r;
	_g = g;
	_b = b;
}

LightColour::LightColour(const LightColour& l)
{
	_r = l.GetR();
	_g = l.GetG();
	_b = l.GetB();
}

LightColour::~LightColour()
{
}

int LightColour::GetR() const
{
	return _r;
}

void LightColour::SetR(const int r)
{
	_r = r;
}

int LightColour::GetG() const
{
	return _g;
}

void LightColour::SetG(const int g)
{
	_g = g;
}

int LightColour::GetB() const
{
	return _b;
}

void LightColour::SetB(const int b)
{
	_b = b;
}

LightColour& LightColour::operator+(const LightColour& l)
{
	_r = _r + l.GetR();
	_g = _g + l.GetG();
	_b = _b + l.GetB();
	return *this;
}

//Adding a Vector3D (float values) to a Light (int values), by rounding the Vector3D's
//values to the nearest integer: .x < .5 = rounded down || .x >= .5 = rounded up
LightColour& LightColour::operator+(const Vector3D& v)
{
	_r = _r + std::round(v.GetX());
	_g = _g + std::round(v.GetY());
	_b = _b + std::round(v.GetZ());
	return *this;
}

LightColour& LightColour::operator/(const int d)
{
	_r = _r / d;
	_g = _g / d;
	_b = _b / d;
	return *this;
}

LightColour& LightColour::operator=(const LightColour& l)
{
	_r = l.GetR();
	_g = l.GetG();
	_b = l.GetB();
	return *this;
}

//Setting a Light (int values) "equal" to a Vector3D (float values), by rounding the Vector3D's
//values to the nearest integer: .x < .5 = rounded down || .x >= .5 = rounded up
LightColour& LightColour::operator=(const Vector3D& v)
{
	_r = std::round(v.GetX());
	_g = std::round(v.GetY());
	_b = std::round(v.GetZ());
	return *this;
}

LightColour& LightColour::operator*(const int m)
{
	_r = _r * m;
	_g = _g * m;
	_b = _b * m;
	return *this;
}

void LightColour::FlatShading()
{
	_r = _rBase;
	_g = _gBase;
	_b = _bBase;
}

void LightColour::Ambient(bool _Gouraud)
{
	_ambientIntensity = _ambientReflectionCoefficient;

	if (!_Gouraud)
	{
		_r = _rBase * _ambientIntensity;
		_g = _gBase * _ambientIntensity;
		_b = _bBase * _ambientIntensity;
	}
}

void LightColour::DiffuseDirectional(const Polygon3D& poly, bool _Gouraud)
{
	//Getting the light's direction from the light Vertex
	//using its x, y, z values
	Vector3D _lightDirection = { _lightPosition.GetX(), _lightPosition.GetY(), _lightPosition.GetZ() };

	//Multiplying the diffuse reflection coefficient with
	//the dot product of the unit vector of the light direction
	//and the unit normal vector of the polygon
	_diffuseIntensity = _diffuseReflectionCoefficient * abs(_lightDirection.Normalise().Dot(poly.GetWorldNormal().Normalise()));

	if (!_Gouraud)
	{
		_r = _rBase * _diffuseIntensity;
		_g = _gBase * _diffuseIntensity;
		_b = _bBase * _diffuseIntensity;
	}
}

void LightColour::DiffusePoint(const Vertex& point, Vector3D& normal, bool _Gouraud)
{
	//Getting the light's direction from the light Vertex
	//using its x, y, z values
	Vector3D _lightDirection = { _lightPosition.GetX(), _lightPosition.GetY(), _lightPosition.GetZ() };

	//Getting the attenuation value
	double atten = Attenuation(point);

	//Multiplying the diffuse reflection coefficient with
	//the dot product of the unit vector of the light direction
	//and the unit normal vector of the polygon, and the attenuation value
	_diffuseIntensity = _diffuseReflectionCoefficient * abs(_lightDirection.Normalise().Dot(normal.Normalise())) * atten;

	if (!_Gouraud)
	{
		_r = _rBase * _diffuseIntensity;
		_g = _gBase * _diffuseIntensity;
		_b = _bBase * _diffuseIntensity;
	}
}

void LightColour::Gouraud(const Vertex& point, Vector3D& normal)
{
	//Calculating the light colour for Gouraud shading,
	//using Ambient and Diffuse (point lighting) lighting,
	//because specular lighting might cause enexpected and
	//unreal visual results.

	Ambient(true);
	DiffusePoint(point, normal, true);

	_r = _rBase * _ambientIntensity + _rBase * _diffuseIntensity;
	_g = _gBase * _ambientIntensity + _gBase * _diffuseIntensity;
	_b = _bBase * _ambientIntensity + _bBase * _diffuseIntensity;
}

void LightColour::SpecularPointDiffuse(const Vertex& point, Vector3D& normal, const Vertex& cameraPos)
{
	//Getting the light's direction from the light Vertex
	//using its x, y, z values
	Vector3D _lightDirection = { _lightPosition.GetX(), _lightPosition.GetY(), _lightPosition.GetZ() };

	Vector3D _camera = { cameraPos.GetX(), cameraPos.GetY(), cameraPos.GetZ() };

	Vector3D R = normal * 2 * normal.Normalise().Dot(_lightDirection.Normalise()) - _lightDirection.Normalise();

	_specularIntensity = _specularReflectionCoefficient * pow(R.Normalise().Dot(_camera.Normalise()), _specularRoughnessConstant);
	DiffusePoint(point, normal, true);

	_r = _rBase * _specularIntensity + _rBase * _diffuseIntensity;
	_g = _gBase * _specularIntensity + _gBase * _diffuseIntensity;
	_b = _bBase * _specularIntensity + _bBase * _diffuseIntensity;
}

double LightColour::Attenuation(const Vertex& point)
{
	double distance = sqrtf(pow(point.GetX() - _lightPosition.GetX(), 2) + pow(point.GetY() - _lightPosition.GetY(), 2) + pow(point.GetZ() - _lightPosition.GetZ(), 2));
	return 1.0 / (a + b * distance + c * pow(distance, 2));
}

void LightColour::Clamp()
{
	//Making sure that the rgb values are in the [0, 255] range

	if (_r < 0)		_r = 0;
	if (_g < 0)		_g = 0;
	if (_b < 0)		_b = 0;
	if (_r > 255)		_r = 255;
	if (_g > 255)		_g = 255;
	if (_b > 255)		_b = 255;
}
