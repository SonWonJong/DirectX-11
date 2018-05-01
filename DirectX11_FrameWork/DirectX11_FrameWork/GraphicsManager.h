#pragma once


const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


class GraphicsManager
{
public:
	GraphicsManager();
	~GraphicsManager();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	class DirectXManager * m_Direct3D = nullptr;
	class CameraClass* m_Camera = nullptr;
	class ModelClass* m_Model = nullptr;
	class ModelClass* m_Model2 = nullptr;
	class ModelClass* m_Model3 = nullptr;
	class ColorShader* m_ColorShader = nullptr;
	class TextureShader* m_TextureShader = nullptr;
};

