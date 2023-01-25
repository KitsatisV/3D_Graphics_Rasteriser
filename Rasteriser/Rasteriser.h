#pragma once
#define _USE_MATH_DEFINES
#include <vector>
#include <time.h>
#include <math.h>
//#include <ppl.h>
#include "Framework.h"
#include "Vertex.h"
#include "Matrix.h"
#include "Model.h"
#include "Polygon3d.h"
#include "MD2Loader.h"
#include "Camera.h"
#include "Vector3D.h"
#include "LightColour.h"

enum class RenderPhase
{
	Start,
	WireframeScaleX,
	WireframeScaleY,
	WireframeScaleZ,
	WireframeScaleXYZ,
	WireframeTranslateX,
	WireframeTranslateY,
	WireframeTranslateZ,
	WireframeRotateX,
	WireframeRotateY,
	WireframeRotateZ,
	WireframeRotateXYZ,
	BackfaceCullingScaleX,
	BackfaceCullingScaleY,
	BackfaceCullingScaleZ,
	BackfaceCullingScaleXYZ,
	BackfaceCullingTranslateX,
	BackfaceCullingTranslateY,
	BackfaceCullingTranslateZ,
	BackfaceCullingRotateX,
	BackfaceCullingRotateY,
	BackfaceCullingRotateZ,
	BackfaceCullingRotateXYZ,
	DrawSolid,
	FlatShadingRotateXYZ,
	AmbientRotateXYZ,
	DiffuseRotateXYZ
};

enum class LightMode
{
	Flat,
	Ambient,
	DiffuseDirectional,
	DiffusePoint,
	Gouraud,
	Specular
};

class Rasteriser : public Framework
{
public:
	string ModelPath();
	bool Initialise();
	void Update(Bitmap &bitmap);
	void Render(Bitmap &bitmap);

	void DrawWireFrame(Bitmap &bitmap);
	void DrawSolid(Bitmap &bitmap);
	void Ambient(Bitmap& bitmap);
	void Diffuse(Bitmap& bitmap);
	void GenerateProjectionMatrix(float d, float aspectRatio);
	void GenerateScreenMatrix(float d, int width, int height);
	void Rasteriser::BackfaceCulling(Bitmap &bitmap);
	void DrawString(const Bitmap& bitmap, LPCTSTR text);
	void Pipeline(float trans_x, float trans_y, float trans_z, float scale_x, float scale_y, float scale_z, float rot_x, float rot_y, float rot_z);
	float Interpolation(float x, float x1, float y1, float x2, float y2);

private:
	Model _model;
	Camera _camera;
	Matrix _modelTransform;
	Matrix _perspectiveTransform;
	Matrix _screenTransform;
	string _modelpath;
	float _angle;
	const float _ambientReflectionCoefficient = 0.2f;
	Vector3D _ambientColor;
	float r, g, b;
	Vector3D _diffuseColor;
	const float _diffuseReflectionCoefficient = 0.5f;
	Vector3D _lightDirection = {0, 0, -50};
	LightColour _light;

	// For displaying purposes
	int _frameCount{ 0 };
	std::wstring _displayText = L"";
	RenderPhase _phase = RenderPhase::Start;
	LightMode _lightMode = LightMode::Flat;
	float scaleX = 0.0f, scaleY = 0.0f, ScaleZ = 0.0f, ScaleXYZ = 0.0f, TransX = 0.0f, TransY = 0.0f, TransZ = 0.0f, RotX = 0.0f, RotY = 0.0f, RotZ = 0.0f, RotXYZ = 0.0f;
	bool _resetCull = true;
	bool _resetSolid = true;
	bool _resetAmbient = true;
	bool _resetDiffuse = true;
	bool _Gouraud = false;
	bool GDI = false;
};

