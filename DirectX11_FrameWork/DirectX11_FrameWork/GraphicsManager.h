#pragma once
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
	class D3DClass * m_Direct3D = nullptr;
	class CameraClass* m_Camera = nullptr;
	class ModelClass* m_Model = nullptr;
	class ColorShaderClass* m_ColorShader = nullptr;
};

