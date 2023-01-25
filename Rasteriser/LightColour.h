#pragma once
#include <cmath>
#include "Polygon3D.h"
#include "Vertex.h"
#include <future>

class LightColour
{
public:
	//Constructors-destructor
	LightColour();
	LightColour(int r, int g, int b);
	LightColour(const LightColour& l);
	~LightColour();

	//Accessors and mutators
	int GetR() const;
	void SetR(const int r);
	int GetG() const;
	void SetG(const int g);
	int GetB() const;
	void SetB(const int b);

	//Overloaded operators
	LightColour& operator+ (const LightColour& l);
	LightColour& operator+ (const Vector3D& v);
	LightColour& operator/ (const int d);
	LightColour& operator= (const LightColour& l);
	LightColour& operator= (const Vector3D& v);
	LightColour& operator* (const int m);

	//Methods
	void FlatShading();
	void Ambient(bool _Gouraud);
	void DiffuseDirectional(const Polygon3D& poly, bool _Gouraud);
	void DiffusePoint(const Vertex& point, Vector3D& normal, bool _Gouraud);
	void Gouraud(const Vertex& point, Vector3D& normal);
	void SpecularPointDiffuse(const Vertex& point, Vector3D& normal, const Vertex& cameraPos);

	double Attenuation(const Vertex& centroid);

	void Clamp();

private:
	int _r, _g, _b;

	//Ambient and diffuse reflection coefficients: in [0, 1]
	const float _ambientReflectionCoefficient = 0.1f;
	const float _diffuseReflectionCoefficient = 0.5f;
	const float _specularReflectionCoefficient = 0.5f;
	const float _specularRoughnessConstant = 0.9f;

	//The base rgb values of the object
	const int _rBase = 136;
	const int _gBase = 8;
	const int _bBase = 8;

	//Light position
	const Vertex _lightPosition = { 0, 0, -1, 1 };
	//Attenuation coefficients
	double a = 0.8, b = 0.4, c = 0.1;

	//Intnsities for different lighting modes
	float _ambientIntensity, _diffuseIntensity, _specularIntensity, _GouraudIntensity;
};
