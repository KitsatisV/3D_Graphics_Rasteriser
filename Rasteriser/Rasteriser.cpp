#include "Rasteriser.h"

Rasteriser app;

string Rasteriser::ModelPath()
{
	char buf[256];
	GetCurrentDirectoryA(256, buf);
	return std::string(buf) + '\\';
}

bool Rasteriser::Initialise()
{
	_modelpath="";
	_modelpath = ModelPath() + "\\cube.md2";

	if (!MD2Loader::LoadModel(_modelpath.c_str(), _model,
		&Model::AddPolygon,
		&Model::AddVertex))
	{
		return false;
	}
	_model;
	
	Camera temp(0, 0, 0, Vertex(0, 0, -50, 1)); 
	_camera = temp;
	return true;
}

void Rasteriser::DrawWireFrame(Bitmap &bitmap)
{
	//Getting the polygons from _model
	//and storing them to local lists
	std::vector<Polygon3D> polys = _model.GetPolygons();

	//Creating a Pen while storing the current pen
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	HGDIOBJ oldPen = SelectObject(bitmap.GetDC(), hPen);

	//Making a loop for 0 till it reaches the polys size
	//	a.	for each polygon getting the index of it and then the
	//		Vertex that corresponds to this index (3 Vertex for each Polygon).
	//	b.	Drawing the polygon (triangle) using the LineTo function
	for (int i = 0; i < polys.size(); i++)
	{
		std::vector<Vertex> vertices = _model.GetPolygonVerticesSorted(polys[i]);

		//Drawing each triangle line, using the triangle's vertices
		MoveToEx(bitmap.GetDC(), vertices[0].GetIntX(), vertices[0].GetIntY(), NULL);
		LineTo(bitmap.GetDC(), vertices[1].GetIntX(), vertices[1].GetIntY());
		LineTo(bitmap.GetDC(), vertices[2].GetIntX(), vertices[2].GetIntY());
		LineTo(bitmap.GetDC(), vertices[0].GetIntX(), vertices[0].GetIntY());
	}
}

void Rasteriser::BackfaceCulling(Bitmap& bitmap)
{
	//Getting the model's visible polygons
	std::vector<Polygon3D> visiblePolys;
	for each (Polygon3D poly in _model.GetPolygons())
	{
		//This loop goes through every polygon in model.
		//If it is not flagged to be culled, it gets pushed back to the visble polys vector
		if (!poly.GetCull())
			visiblePolys.push_back(poly);
	}

	//Creating a Pen while storing the current pen
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	HGDIOBJ oldPen = SelectObject(bitmap.GetDC(), hPen);

	//Making a loop for 0 till it reaches the visiblePolys size
	//	a.	for each visible polygon, getting the index of it and then the
	//		Vertex that corresponds to this index (3 Vertex for each Polygon).
	//	b.	Drawing the visible polygon (triangle) using the LineTo function
	for (int i = 0; i < visiblePolys.size(); i++)
	{
		std::vector<Vertex> vertices = _model.GetPolygonVerticesSorted(visiblePolys[i]);

		//Drawing each triangle line, using the triangle's vertices
		MoveToEx(bitmap.GetDC(), vertices[0].GetIntX(), vertices[0].GetIntY(), NULL);
		LineTo(bitmap.GetDC(), vertices[1].GetIntX(), vertices[1].GetIntY());
		LineTo(bitmap.GetDC(), vertices[2].GetIntX(), vertices[2].GetIntY());
		LineTo(bitmap.GetDC(), vertices[0].GetIntX(), vertices[0].GetIntY());
	}
}

void Rasteriser::DrawSolid(Bitmap& bitmap)
{
	//Getting the model's visible polygons and vertices
	std::vector<Polygon3D> visiblePolys;
	for each (Polygon3D poly in _model.GetPolygons())
	{
		//This loop goes through every polygon in model.
		//If it is not flagged to be culled, it gets pushed back to the visble polys vector
		if (!poly.GetCull())
			visiblePolys.push_back(poly);
	}
	
	//PUT EVERYTHING HERE///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	switch (_lightMode)
	{
	case LightMode::Flat:
	{
		if (_lightMode == LightMode::Flat)
		{
			//Getting the base colour of the object
			_light.FlatShading();

			if (GDI)
			{
				//GDI CALLS
				//Creating a Pen while storing the current pen
				HPEN hPen = CreatePen(PS_SOLID, 1, RGB(_light.GetR(), _light.GetG(), _light.GetB()));
				HGDIOBJ oldPen = SelectObject(bitmap.GetDC(), hPen);

				//Creating a Brush while storing the current brush
				HBRUSH hBrush = CreateSolidBrush(RGB(_light.GetR(), _light.GetG(), _light.GetB()));
				HGDIOBJ oldBrush = SelectObject(bitmap.GetDC(), hBrush);

				//Painting the object, using GDI calls
				for (int i = 0; i < visiblePolys.size(); i++)
				{
					//Drawing-filling the polygon
					std::vector<Vertex> vertices = _model.GetPolygonVerticesSorted(visiblePolys[i]);
					POINT points[3];
					points[0].x = vertices[0].GetX();
					points[0].y = vertices[0].GetY();
					points[1].x = vertices[1].GetX();
					points[1].y = vertices[1].GetY();
					points[2].x = vertices[2].GetX();
					points[2].y = vertices[2].GetY();
					Polygon(bitmap.GetDC(), points, 3);
				}
				//GDI CALLS
			}
			else if (!GDI)
			{
				for (int i = 0; i < visiblePolys.size(); i++)
				{
					std::vector<Vertex> vertices = _model.GetPolygonVerticesSorted(visiblePolys[i]);

					//my solution
					for (int j = vertices[0].GetY(); j > vertices[2].GetY(); j--)
					{
						if ((vertices[1].GetY() - vertices[0].GetY()) != 0 && (vertices[2].GetY() - vertices[0].GetY()) != 0 && (vertices[2].GetY() - vertices[1].GetY()))
						{
							float a = Interpolation(j, vertices[0].GetY(), vertices[0].GetX(), vertices[1].GetY(), vertices[1].GetX());
							float b = Interpolation(j, vertices[0].GetY(), vertices[0].GetX(), vertices[2].GetY(), vertices[2].GetX());
							float c = Interpolation(j, vertices[1].GetY(), vertices[1].GetX(), vertices[2].GetY(), vertices[2].GetX());

							int minX;
							int maxX;

							if (j > vertices[1].GetY())
							{
								minX = min(a, b);
								maxX = max(a, b);
							}
							else
							{
								minX = min(b, c);
								maxX = max(b, c);
							}

							for (int k = minX; k < maxX; k++)
							{
								visiblePolys[i].PixelFill(bitmap.GetDC(), k, j, _light.GetR(), _light.GetG(), _light.GetB());
							}
						}
					}
				}
			}
		}
	}
	case LightMode::Ambient:
	{
		if (_lightMode == LightMode::Ambient)
		{
			//Getting the base colour of the object
			_light.Ambient(_Gouraud);

			for (int i = 0; i < visiblePolys.size(); i++)
			{
				std::vector<Vertex> vertices = _model.GetPolygonVerticesSorted(visiblePolys[i]);

				//my solution
				for (int j = vertices[0].GetY(); j > vertices[2].GetY(); j--)
				{
					if ((vertices[1].GetY() - vertices[0].GetY()) != 0 && (vertices[2].GetY() - vertices[0].GetY()) != 0 && (vertices[2].GetY() - vertices[1].GetY()))
					{
						float a = Interpolation(j, vertices[0].GetY(), vertices[0].GetX(), vertices[1].GetY(), vertices[1].GetX());
						float b = Interpolation(j, vertices[0].GetY(), vertices[0].GetX(), vertices[2].GetY(), vertices[2].GetX());
						float c = Interpolation(j, vertices[1].GetY(), vertices[1].GetX(), vertices[2].GetY(), vertices[2].GetX());

						int minX;
						int maxX;

						if (j > vertices[1].GetY())
						{
							minX = min(a, b);
							maxX = max(a, b);
						}
						else
						{
							minX = min(b, c);
							maxX = max(b, c);
						}

						for (int k = minX; k < maxX; k++)
						{
							visiblePolys[i].PixelFill(bitmap.GetDC(), k, j, _light.GetR(), _light.GetG(), _light.GetB());
						}
					}

				}
			}

			////GDI calls
			////Creating a Pen while storing the current pen
			//HPEN hPen = CreatePen(PS_SOLID, 1, RGB(_light.GetR(), _light.GetG(), _light.GetB()));
			//HGDIOBJ oldPen = SelectObject(bitmap.GetDC(), hPen);

			////Creating a Brush while storing the current brush
			//HBRUSH hBrush = CreateSolidBrush(RGB(_light.GetR(), _light.GetG(), _light.GetB()));
			//HGDIOBJ oldBrush = SelectObject(bitmap.GetDC(), hBrush);

			////Painting the object, using GDI calls
			//for (int i = 0; i < visiblePolys.size(); i++)
			//{
			//	//Drawing-filling the polygon
			//	std::vector<Vertex> vertices = _model.GetPolygonVerticesSorted(visiblePolys[i]);
			//	POINT points[3];
			//	points[0].x = vertices[0].GetX();
			//	points[0].y = vertices[0].GetY();
			//	points[1].x = vertices[1].GetX();
			//	points[1].y = vertices[1].GetY();
			//	points[2].x = vertices[2].GetX();
			//	points[2].y = vertices[2].GetY();
			//	Polygon(bitmap.GetDC(), points, 3);
			//}
		}
	}
	case LightMode::DiffuseDirectional:
	{
		if (_lightMode == LightMode::DiffuseDirectional)
		{
			//Painting the object, using GDI calls
			for (int i = 0; i < visiblePolys.size(); i++)
			{
				//Getting the colour of each polygon
				_light.DiffuseDirectional(visiblePolys[i], _Gouraud);

				std::vector<Vertex> vertices = _model.GetPolygonVerticesSorted(visiblePolys[i]);

				//my solution
				for (int j = vertices[0].GetY(); j > vertices[2].GetY(); j--)
				{
					if ((vertices[1].GetY() - vertices[0].GetY()) != 0 && (vertices[2].GetY() - vertices[0].GetY()) != 0 && (vertices[2].GetY() - vertices[1].GetY()))
					{
						float a = Interpolation(j, vertices[0].GetY(), vertices[0].GetX(), vertices[1].GetY(), vertices[1].GetX());
						float b = Interpolation(j, vertices[0].GetY(), vertices[0].GetX(), vertices[2].GetY(), vertices[2].GetX());
						float c = Interpolation(j, vertices[1].GetY(), vertices[1].GetX(), vertices[2].GetY(), vertices[2].GetX());

						int minX;
						int maxX;

						if (j > vertices[1].GetY())
						{
							minX = min(a, b);
							maxX = max(a, b);
						}
						else
						{
							minX = min(b, c);
							maxX = max(b, c);
						}

						for (int k = minX; k < maxX; k++)
						{
							visiblePolys[i].PixelFill(bitmap.GetDC(), k, j, _light.GetR(), _light.GetG(), _light.GetB());
						}
					}

				}

				////GDI calls
				////Creating a Pen while storing the current pen
				//HPEN hPen = CreatePen(PS_SOLID, 1, RGB(_light.GetR(), _light.GetG(), _light.GetB()));
				//HGDIOBJ oldPen = SelectObject(bitmap.GetDC(), hPen);

				////Creating a Brush while storing the current brush
				//HBRUSH hBrush = CreateSolidBrush(RGB(_light.GetR(), _light.GetG(), _light.GetB()));
				//HGDIOBJ oldBrush = SelectObject(bitmap.GetDC(), hBrush);

				////Drawing-filling the polygon
				//std::vector<Vertex> vertices = _model.GetPolygonVerticesSorted(visiblePolys[i]);
				//POINT points[3];
				//points[0].x = vertices[0].GetX();
				//points[0].y = vertices[0].GetY();
				//points[1].x = vertices[1].GetX();
				//points[1].y = vertices[1].GetY();
				//points[2].x = vertices[2].GetX();
				//points[2].y = vertices[2].GetY();
				//Polygon(bitmap.GetDC(), points, 3);
			}
		}
	}
	case LightMode::DiffusePoint:
	{
		if (_lightMode == LightMode::DiffusePoint)
		{
			for (int i = 0; i < visiblePolys.size(); i++)
			{
				//Getting the colour of each polygon
				_light.DiffusePoint(visiblePolys[i].GetCentroid(), visiblePolys[i].GetWorldNormal(), _Gouraud);

				std::vector<Vertex> vertices = _model.GetPolygonVerticesSorted(visiblePolys[i]);

				//my solution
				for (int j = vertices[0].GetY(); j > vertices[2].GetY(); j--)
				{
					if ((vertices[1].GetY() - vertices[0].GetY()) != 0 && (vertices[2].GetY() - vertices[0].GetY()) != 0 && (vertices[2].GetY() - vertices[1].GetY()))
					{
						float a = Interpolation(j, vertices[0].GetY(), vertices[0].GetX(), vertices[1].GetY(), vertices[1].GetX());
						float b = Interpolation(j, vertices[0].GetY(), vertices[0].GetX(), vertices[2].GetY(), vertices[2].GetX());
						float c = Interpolation(j, vertices[1].GetY(), vertices[1].GetX(), vertices[2].GetY(), vertices[2].GetX());

						int minX;
						int maxX;

						if (j > vertices[1].GetY())
						{
							minX = min(a, b);
							maxX = max(a, b);
						}
						else
						{
							minX = min(b, c);
							maxX = max(b, c);
						}

						for (int k = minX; k < maxX; k++)
						{
							visiblePolys[i].PixelFill(bitmap.GetDC(), k, j, _light.GetR(), _light.GetG(), _light.GetB());
						}
					}

				}

				////GDI calls
				////Creating a Pen while storing the current pen
				//HPEN hPen = CreatePen(PS_SOLID, 1, RGB(_light.GetR(), _light.GetG(), _light.GetB()));
				//HGDIOBJ oldPen = SelectObject(bitmap.GetDC(), hPen);

				////Creating a Brush while storing the current brush
				//HBRUSH hBrush = CreateSolidBrush(RGB(_light.GetR(), _light.GetG(), _light.GetB()));
				//HGDIOBJ oldBrush = SelectObject(bitmap.GetDC(), hBrush);

				////Drawing-filling the polygon
				//std::vector<Vertex> vertices = _model.GetPolygonVerticesSorted(visiblePolys[i]);
				//POINT points[3];
				//points[0].x = vertices[0].GetX();
				//points[0].y = vertices[0].GetY();
				//points[1].x = vertices[1].GetX();
				//points[1].y = vertices[1].GetY();
				//points[2].x = vertices[2].GetX();
				//points[2].y = vertices[2].GetY();
				//Polygon(bitmap.GetDC(), points, 3);
			}
		}
	}
	case LightMode::Gouraud:
	{
		if (_lightMode == LightMode::Gouraud)
		{
			_Gouraud = true;
			_model.CalculateVertexNormals();
			//Painting the object, using GDI calls
			for (int i = 0; i < visiblePolys.size(); i++)
			{
				std::vector<Vertex> vertices = _model.GetPolygonVerticesSorted(visiblePolys[i]);

				//Getting the colour of each Vertex of the polygon
				_light.Gouraud(vertices[0], vertices[0].GetNormal());
				LightColour tempVrtx0;
				tempVrtx0.SetR(_light.GetR());
				tempVrtx0.SetG(_light.GetG());
				tempVrtx0.SetB(_light.GetB());
				_light.Gouraud(vertices[1], vertices[1].GetNormal());
				LightColour tempVrtx1;
				tempVrtx1.SetR(_light.GetR());
				tempVrtx1.SetG(_light.GetG());
				tempVrtx1.SetB(_light.GetB());
				_light.Gouraud(vertices[2], vertices[2].GetNormal());
				LightColour tempVrtx2;
				tempVrtx2.SetR(_light.GetR());
				tempVrtx2.SetG(_light.GetG());
				tempVrtx2.SetB(_light.GetB());

				LightColour temp0 = tempVrtx0;
				LightColour temp1 = tempVrtx1;
				LightColour temp2 = tempVrtx2;



				//my solution
				for (int j = vertices[0].GetY(); j > vertices[2].GetY(); j--)
				{
					if ((vertices[1].GetY() - vertices[0].GetY()) != 0 && (vertices[2].GetY() - vertices[0].GetY()) != 0 && (vertices[2].GetY() - vertices[1].GetY()))
					{
						float a = Interpolation(j, vertices[0].GetY(), vertices[0].GetX(), vertices[1].GetY(), vertices[1].GetX());
						float b = Interpolation(j, vertices[0].GetY(), vertices[0].GetX(), vertices[2].GetY(), vertices[2].GetX());
						float c = Interpolation(j, vertices[1].GetY(), vertices[1].GetX(), vertices[2].GetY(), vertices[2].GetX());

						int minX;
						int maxX;
						LightColour tempColourMinX;
						LightColour tempColourMaxX;
						LightColour temptempMinX;
						LightColour temptempMaxX;
						LightColour pointColour;

						if (j > vertices[1].GetY())
						{
							minX = min(a, b);
							maxX = max(a, b);

							if (minX == a)
							{
								tempColourMinX = temp0 * ((j - vertices[1].GetY()) / (vertices[0].GetY() - vertices[1].GetY())) + temp1 * ((vertices[0].GetY() - j) / (vertices[1].GetY() - vertices[0].GetY()));

								temp0 = tempVrtx0;

								tempColourMaxX = temp0 * ((j - vertices[2].GetY()) / (vertices[0].GetY() - vertices[2].GetY())) + temp2 * ((vertices[0].GetY() - j) / (vertices[2].GetY() - vertices[0].GetY()));

								temp0 = tempVrtx0;
								temp1 = tempVrtx1;
								temp2 = tempVrtx2;
							}
							else
							{
								tempColourMaxX = temp0 * ((j - vertices[1].GetY()) / (vertices[0].GetY() - vertices[1].GetY())) + temp1 * ((vertices[0].GetY() - j) / (vertices[1].GetY() - vertices[0].GetY()));

								temp0 = tempVrtx0;

								tempColourMinX = temp0 * ((j - vertices[2].GetY()) / (vertices[0].GetY() - vertices[2].GetY())) + temp2 * ((vertices[0].GetY() - j) / (vertices[2].GetY() - vertices[0].GetY()));

								temp0 = tempVrtx0;
								temp1 = tempVrtx1;
								temp2 = tempVrtx2;
							}
						}
						else
						{
							minX = min(b, c);
							maxX = max(b, c);

							if (minX == c)
							{
								tempColourMinX = temp1 * ((j - vertices[2].GetY()) / (vertices[1].GetY() - vertices[2].GetY())) + temp2 * ((vertices[1].GetY() - j) / (vertices[2].GetY() - vertices[1].GetY()));

								temp2 = tempVrtx2;

								tempColourMaxX = temp0 * ((j - vertices[2].GetY()) / (vertices[0].GetY() - vertices[2].GetY())) + temp2 * ((vertices[0].GetY() - j) / (vertices[2].GetY() - vertices[0].GetY()));

								temp0 = tempVrtx0;
								temp1 = tempVrtx1;
								temp2 = tempVrtx2;
							}
							else
							{
								tempColourMaxX = temp1 * ((j - vertices[2].GetY()) / (vertices[1].GetY() - vertices[2].GetY())) + temp2 * ((vertices[1].GetY() - j) / (vertices[2].GetY() - vertices[1].GetY()));

								temp2 = tempVrtx2;

								tempColourMinX = temp0 * ((j - vertices[2].GetY()) / (vertices[0].GetY() - vertices[2].GetY())) + temp2 * ((vertices[0].GetY() - j) / (vertices[2].GetY() - vertices[0].GetY()));

								temp0 = tempVrtx0;
								temp1 = tempVrtx1;
								temp2 = tempVrtx2;
							}
						}

						for (int k = minX; k < maxX; k++)
						{
							temptempMinX = tempColourMinX;
							temptempMaxX = tempColourMaxX;

							pointColour = temptempMinX * ((maxX - k) / (maxX - minX)) + temptempMaxX * ((k - minX) / (maxX - minX));
							visiblePolys[i].PixelFill(bitmap.GetDC(), k, j, pointColour.GetR(), pointColour.GetG(), pointColour.GetB());

							temptempMinX = tempColourMinX;
							temptempMaxX = tempColourMaxX;
						}
					}

				}
			}
		}
	}
	case LightMode::Specular:
	{
		if (_lightMode == LightMode::Specular)
		{
			for (int i = 0; i < visiblePolys.size(); i++)
			{
				std::vector<Vertex> vertices = _model.GetPolygonVerticesSorted(visiblePolys[i]);

				//my solution
				for (int j = vertices[0].GetY(); j > vertices[2].GetY(); j--)
				{
					if ((vertices[1].GetY() - vertices[0].GetY()) != 0 && (vertices[2].GetY() - vertices[0].GetY()) != 0 && (vertices[2].GetY() - vertices[1].GetY()))
					{
						float a = Interpolation(j, vertices[0].GetY(), vertices[0].GetX(), vertices[1].GetY(), vertices[1].GetX());
						float b = Interpolation(j, vertices[0].GetY(), vertices[0].GetX(), vertices[2].GetY(), vertices[2].GetX());
						float c = Interpolation(j, vertices[1].GetY(), vertices[1].GetX(), vertices[2].GetY(), vertices[2].GetX());

						int minX;
						int maxX;

						if (j > vertices[1].GetY())
						{
							minX = min(a, b);
							maxX = max(a, b);
						}
						else
						{
							minX = min(b, c);
							maxX = max(b, c);
						}

						for (int k = minX; k < maxX; k++)
						{
							Vertex point = { (float)k, (float)j, 1.0f, 1.0f };
							//Getting the colour of each point
							_light.SpecularPointDiffuse(point, visiblePolys[i].GetWorldNormal(), _camera.GetPos());
							_light.Clamp();

							visiblePolys[i].PixelFill(bitmap.GetDC(), k, j, _light.GetR(), _light.GetG(), _light.GetB());
						}
					}
				}
			}
		}
	}
	default:
		break;
	}


	//PUT EVERYTHING HERE///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

void Rasteriser::GenerateProjectionMatrix(float d, float aspectRatio)
{
	// Perspective projection
	Matrix perspective{ d / aspectRatio, 0, 0, 0,
						0, d, 0, 0,
						0, 0, d, 0,
						0, 0, 1, 0 };
	_perspectiveTransform = perspective;
}

void Rasteriser::GenerateScreenMatrix(float d, int width, int height) // ScrenTransformation
{
	Matrix view{ float(width) / 2, 0, 0, float(width) / 2,
				 0, float(-height) / 2, 0, float(height) / 2,
				 0, 0, d / 2, d / 2,
				 0, 0, 0, 1 };
	_screenTransform = view;
}

void Rasteriser::Update(Bitmap &bitmap)  
{
	
	float aspectRatio = float(float(bitmap.GetWidth()) / float(bitmap.GetHeight()));
	GenerateProjectionMatrix(1, aspectRatio);
	GenerateScreenMatrix(1, bitmap.GetWidth(), bitmap.GetHeight());
	// Displaying the code
	if (_frameCount < 50)
	{
		_phase = RenderPhase::Start;
		_displayText = L"Wireframe";
	}
	else if (_frameCount < 140)
	{
		_phase = RenderPhase::WireframeScaleX;
		_displayText = L"Wireframe: X Axis Scale";
	}
	else if (_frameCount < 230)
	{
		_phase = RenderPhase::WireframeScaleY;
		_displayText = L"Wireframe: Y Axis Scale";
	}
	else if (_frameCount < 320)
	{
		_phase = RenderPhase::WireframeScaleZ;
		_displayText = L"Wireframe: Z Axis Scale";
	}
	else if (_frameCount < 410)
	{
		_phase = RenderPhase::WireframeScaleXYZ;
		_displayText = L"Wireframe: X-Y-Z Axis Scale";
	}
	else if (_frameCount < 500)
	{
		_phase = RenderPhase::WireframeTranslateX;
		_displayText = L"Wireframe: X Axis Translate";
	}
	else if (_frameCount < 590)
	{
		_phase = RenderPhase::WireframeTranslateY;
		_displayText = L"Wireframe: Y Axis Translate";
	}
	else if (_frameCount < 680)
	{
		_phase = RenderPhase::WireframeTranslateZ;
		_displayText = L"Wireframe: Z Axis Translate";
	}
	else if (_frameCount < 770)
	{
		_phase = RenderPhase::WireframeRotateX;
		_displayText = L"Wireframe: X Axis Rotate";
	}
	else if (_frameCount < 860)
	{
		_phase = RenderPhase::WireframeRotateY;
		_displayText = L"Wireframe: Y Axis Rotate";
	}
	else if (_frameCount < 950)
	{
		_phase = RenderPhase::WireframeRotateZ;
		_displayText = L"Wireframe: Z Axis Rotate";
	}
	else if (_frameCount < 1040)
	{
		_phase = RenderPhase::WireframeRotateXYZ;
		_displayText = L"Wireframe: X-Y-Z Axis Rotate";
	}
	else if (_frameCount < 1130)
	{
		_phase = RenderPhase::BackfaceCullingScaleX;
		_displayText = L"BackfaceCulling: X Axis Scale";
	}
	else if (_frameCount < 1220)
	{
		_phase = RenderPhase::BackfaceCullingScaleY;
		_displayText = L"BackfaceCulling: Y Axis Scale";
	}
	else if (_frameCount < 1310)
	{
		_phase = RenderPhase::BackfaceCullingScaleZ;
		_displayText = L"BackfaceCulling: Z Axis Scale";
	}
	else if (_frameCount < 1400)
	{
		_phase = RenderPhase::BackfaceCullingScaleXYZ;
		_displayText = L"BackfaceCulling: X-Y-Z Axis Scale";
	}
	else if (_frameCount < 1490)
	{
		_phase = RenderPhase::BackfaceCullingTranslateX;
		_displayText = L"BackfaceCulling: X Axis Translate";
	}
	else if (_frameCount < 1580)
	{
		_phase = RenderPhase::BackfaceCullingTranslateY;
		_displayText = L"BackfaceCulling: Y Axis Translate";
	}
	else if (_frameCount < 1670)
	{
		_phase = RenderPhase::BackfaceCullingTranslateZ;
		_displayText = L"BackfaceCulling: Z Axis Translate";
	}
	else if (_frameCount < 1760)
	{
		_phase = RenderPhase::BackfaceCullingRotateX;
		_displayText = L"BackfaceCulling: X Axis Rotate";
	}
	else if (_frameCount < 1850)
	{
		_phase = RenderPhase::BackfaceCullingRotateY;
		_displayText = L"BackfaceCulling: Y Axis Rotate";
	}
	else if (_frameCount < 1940)
	{
		_phase = RenderPhase::BackfaceCullingRotateZ;
		_displayText = L"BackfaceCulling: Z Axis Rotate";
	}
	else if (_frameCount < 2030)
	{
	_phase = RenderPhase::BackfaceCullingRotateXYZ;
	_displayText = L"BackfaceCulling: X-Y-Z Axis Rotate";
	}
	else if (_frameCount < 2120)
	{
	_phase = RenderPhase::DrawSolid;
	_lightMode = LightMode::Flat;
	GDI = true;
	_displayText = L"Flat Shading, whithout GDI calls: X-Y-Z Axis Rotate";
	}
	else if (_frameCount < 2210)
	{
	_phase = RenderPhase::DrawSolid;
	_lightMode = LightMode::Ambient;
	_displayText = L"Ambient Lighting: X-Y-Z Axis Rotate";
	}
	else if (_frameCount < 2300)
	{
	_phase = RenderPhase::DrawSolid;
	_lightMode = LightMode::DiffuseDirectional;
	_displayText = L"Diffuse (Directional) Lighting: X-Y-Z Axis Rotate";
	}
	else if (_frameCount < 2390)
	{
	_phase = RenderPhase::DrawSolid;
	_lightMode = LightMode::DiffusePoint;
	_displayText = L"Diffuse (Point) Lighting: X-Y-Z Axis Rotate";
	}
	else if (_frameCount < 2480)
	{
	_phase = RenderPhase::DrawSolid;
	_lightMode = LightMode::Gouraud;
	_displayText = L"Gouraud Shading: X-Y-Z Axis Rotate";
	}
	else if (_frameCount < 2570)
	{
	_phase = RenderPhase::DrawSolid;
	_lightMode = LightMode::Specular;
	_displayText = L"Specular (Point) Lighting: X-Y-Z Axis Rotate";
	}
	else if (_frameCount < 2660)
	{
	_phase = RenderPhase::DrawSolid;
	_lightMode = LightMode::Flat;
	GDI = true;
	_displayText = L"Flat Shading, using GDI calls: X-Y-Z Axis Rotate";
	}
	_frameCount++;


}

void Rasteriser::Render(Bitmap &bitmap)   
{
	bitmap.Clear(RGB(0, 0, 0));
	if (_phase == RenderPhase::Start)
	{
		//Setting the initial values for each transformations
		scaleX = 0.5f;
		scaleY = 0.5f;
		ScaleZ = 0.5f;
		ScaleXYZ = 0.5f;
		TransX = -40.0f;
		TransY = -40.0f;
		TransZ = -40.0f;
		RotX = 0;
		RotY = 0;
		RotZ = 0;
		RotXYZ = 0;
	}

	//Selecting the correct transformation to display and demonstrate,
	// depending on the phase, set by the Update method
	switch (_phase)
	{
	case RenderPhase::Start:
	{
		Pipeline(0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f);
		DrawWireFrame(bitmap);
		//DrawSolid(bitmap);
		break;
	}
	case RenderPhase::WireframeScaleX:
	{
		Pipeline(0.0f, 0.0f, 0.0f, scaleX, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f);
		DrawWireFrame(bitmap);
		scaleX = scaleX + 0.007f;
		break;
	}
	case RenderPhase::WireframeScaleY:
	{
		Pipeline(0.0f, 0.0f, 0.0f, 0.5f, scaleY, 0.5f, 0.0f, 0.0f, 0.0f);
		DrawWireFrame(bitmap);
		scaleY = scaleY + 0.007f;
		break;
	}
	case RenderPhase::WireframeScaleZ:
	{
		Pipeline(0.0f, 0.0f, 0.0f, 0.5f, 0.5f, ScaleZ, 0.0f, 0.0f, 0.0f);
		DrawWireFrame(bitmap);
		ScaleZ = ScaleZ + 0.007f;
		break;
	}
	case RenderPhase::WireframeScaleXYZ:
	{
		Pipeline(0.0f, 0.0f, 0.0f, ScaleXYZ, ScaleXYZ, ScaleXYZ, 0.0f, 0.0f, 0.0f);
		DrawWireFrame(bitmap);
		ScaleXYZ = ScaleXYZ + 0.007f;
		break;
	}
	case RenderPhase::WireframeTranslateX:
	{
		Pipeline(TransX, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f);
		DrawWireFrame(bitmap);
		TransX = TransX + 1.0f;
		break;
	}
	case RenderPhase::WireframeTranslateY:
	{
		Pipeline(0.0f, TransY, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f);
		DrawWireFrame(bitmap);
		TransY = TransY + 1.0f;
		break;
	}
	case RenderPhase::WireframeTranslateZ:
	{
		Pipeline(0.0f, 0.0f, TransZ, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f);
		DrawWireFrame(bitmap);
		TransZ = TransZ + 1.0f;
		break;
	}
	case RenderPhase::WireframeRotateX:
	{
		Pipeline(0.0f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, RotX, 0.0f, 0.0f);
		DrawWireFrame(bitmap);
		RotX = RotX + 0.05f;
		break;
	}
	case RenderPhase::WireframeRotateY:
	{
		Pipeline(0.0f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 0.0f, RotY, 0.0f);
		DrawWireFrame(bitmap);
		RotY = RotY + 0.05f;
		break;
	}
	case RenderPhase::WireframeRotateZ:
	{
		Pipeline(0.0f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 0.0f, 0.0f, RotZ);
		DrawWireFrame(bitmap);
		RotZ = RotZ + 0.05f;
		break;
	}
	case RenderPhase::WireframeRotateXYZ:
	{
		Pipeline(0.0f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, RotXYZ, RotXYZ, RotXYZ);
		DrawWireFrame(bitmap);
		RotXYZ = RotXYZ + 0.05f;
		break;
	}
	case RenderPhase::BackfaceCullingScaleX:
	{
		//Resetting the initial values for the BackfaceCulling transformations
		if (_resetCull)
		{
			scaleX = 0.5f;
			scaleY = 0.5f;
			ScaleZ = 0.5f;
			ScaleXYZ = 0.5f;
			TransX = -40.0f;
			TransY = -40.0f;
			TransZ = -40.0f;
			RotX = 0.0f;
			RotY = 0.0f;
			RotZ = 0.0f;
			RotXYZ = 0.0f;
			_resetCull = false;
		}
		Pipeline(0.0f, 0.0f, 0.0f, scaleX, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f);
		BackfaceCulling(bitmap);
		scaleX = scaleX + 0.007f;
		break;
	}
	case RenderPhase::BackfaceCullingScaleY:
	{
		Pipeline(0.0f, 0.0f, 0.0f, 0.5f, scaleY, 0.5f, 0.0f, 0.0f, 0.0f);
		BackfaceCulling(bitmap);
		scaleY = scaleY + 0.007f;
		break;
	}
	case RenderPhase::BackfaceCullingScaleZ:
	{
		Pipeline(0.0f, 0.0f, 0.0f, 0.5f, 0.5f, ScaleZ, 0.0f, 0.0f, 0.0f);
		BackfaceCulling(bitmap);
		ScaleZ = ScaleZ + 0.007f;
		break;
	}
	case RenderPhase::BackfaceCullingScaleXYZ:
	{
		Pipeline(0.0f, 0.0f, 0.0f, ScaleXYZ, ScaleXYZ, ScaleXYZ, 0.0f, 0.0f, 0.0f);
		BackfaceCulling(bitmap);
		ScaleXYZ = ScaleXYZ + 0.007f;
		break;
	}
	case RenderPhase::BackfaceCullingTranslateX:
	{
		Pipeline(TransX, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f);
		BackfaceCulling(bitmap);
		TransX = TransX + 1.0f;
		break;
	}
	case RenderPhase::BackfaceCullingTranslateY:
	{
		Pipeline(0.0f, TransY, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f);
		BackfaceCulling(bitmap);
		TransY = TransY + 1.0f;
		break;
	}
	case RenderPhase::BackfaceCullingTranslateZ:
	{
		Pipeline(0.0f, 0.0f, TransZ, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f);
		BackfaceCulling(bitmap);
		TransZ = TransZ + 1.0f;
		break;
	}
	case RenderPhase::BackfaceCullingRotateX:
	{
		Pipeline(0.0f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, RotX, 0.0f, 0.0f);
		BackfaceCulling(bitmap);
		RotX = RotX + 0.05f;
		break;
	}
	case RenderPhase::BackfaceCullingRotateY:
	{
		Pipeline(0.0f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 0.0f, RotY, 0.0f);
		BackfaceCulling(bitmap);
		RotY = RotY + 0.05f;
		break;
	}
	case RenderPhase::BackfaceCullingRotateZ:
	{
		Pipeline(0.0f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 0.0f, 0.0f, RotZ);
		BackfaceCulling(bitmap);
		RotZ = RotZ + 0.05f;
		break;
	}
	case RenderPhase::BackfaceCullingRotateXYZ:
	{
		Pipeline(0.0f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, RotXYZ, RotXYZ, RotXYZ);
		BackfaceCulling(bitmap);
		RotXYZ = RotXYZ + 0.05f;
		break;
	}
	case RenderPhase::DrawSolid:
	{
		//Resetting the initial values for the FlatShading transformations
		if (_resetSolid)
		{
			scaleX = 0.5f;
			scaleY = 0.5f;
			ScaleZ = 0.5f;
			ScaleXYZ = 0.5f;
			TransX = -40.0f;
			TransY = -40.0f;
			TransZ = -40.0f;
			RotX = 0.0f;
			RotY = 0.0f;
			RotZ = 0.0f;
			RotXYZ = 0.0f;
			_resetSolid = false;
		}
		Pipeline(0.0f, 0.0f, 0.0f, 0.2f, 0.2f, 0.2f, RotXYZ, RotXYZ, RotXYZ);
		DrawSolid(bitmap);
		RotXYZ = RotXYZ + 0.05f;
		break;
	}
	
	default:
		break;
	}
	DrawString(bitmap, _displayText.c_str());
	
}

void Rasteriser::DrawString(const Bitmap& bitmap, LPCTSTR text)
{
	HDC hdc = bitmap.GetDC();
	HFONT hFont, hOldFont;

	// Retrieve a handle to the variable stock font.  
	hFont = hFont = CreateFont(48, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Myfont"));

	// Select the variable stock font into the specified device context. 
	if (hOldFont = (HFONT)SelectObject(hdc, hFont))
	{
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkColor(hdc, RGB(0, 0, 0));

		// Display the text string.  
		TextOut(hdc, 10, 10, text, lstrlen(text));

		// Restore the original font.        
		SelectObject(hdc, hOldFont);
	}
	DeleteObject(hFont);
}

void Rasteriser::Pipeline(float trans_x, float trans_y, float trans_z, float scale_x, float scale_y, float scale_z, float rot_x, float rot_y, float rot_z)
{
	//Implementing the pipeline

	{
		//Model/World Transformation
		
		//Translation
		_modelTransform = Matrix::TranslationMatrix(trans_x, trans_y, trans_z);
		_model.ApplyTransformToLocalVertices(_modelTransform);

		//Scaling
		_modelTransform = Matrix::ScalingMatrixXYZ(scale_x, scale_y, scale_z);
		_model.ApplyTransformToTransformedVertices(_modelTransform);

		//Rotation
		_modelTransform = Matrix::XRotationMatrix(rot_x);
		_model.ApplyTransformToTransformedVertices(_modelTransform);
		_modelTransform = Matrix::YRotationMatrix(rot_y);
		_model.ApplyTransformToTransformedVertices(_modelTransform);
		_modelTransform = Matrix::ZRotationMatrix(rot_z);
		_model.ApplyTransformToTransformedVertices(_modelTransform);
	}

	//Saving the model/world transformed vertices, for lighting purposes
	_model.SetWorldTransform();

	//Viewing Transformation
	_model.ApplyTransformToTransformedVertices(_camera.GetCameraMatrix());

	//Projection Transformation
	_model.ApplyTransformToTransformedVertices(_perspectiveTransform);
	_model.CalculatePolygonAverageZ();
	_model.Sort();
	_model.Dehomogenize();

	//Screen Transformation
	_model.ApplyTransformToTransformedVertices(_screenTransform);

	//Deciding the cull of each polygon
	_model.CalculateBackfaces(_camera);

	//Calculating the normals of the polygons
	_model.CalculatePolygonNormals();
}

float Rasteriser::Interpolation(float x, float x1, float y1, float x2, float y2)
{
	return (y1 + (x - x1) * (y2 - y1) / (x2 - x1));
}