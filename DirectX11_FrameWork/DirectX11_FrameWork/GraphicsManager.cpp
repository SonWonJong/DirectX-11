#include "stdafx.h"
#include "GraphicsManager.h"
#include "DirectXManager.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "ColorShader.h"
#include "TextureShader.h"
#include "LightClass.h"
#include "LightShader.h"
#include "BitmapClass.h"

GraphicsManager::GraphicsManager()
{
}


GraphicsManager::~GraphicsManager()
{
}

bool GraphicsManager::Initialize(int InScreenWidth, int InScreenHeight, HWND InHwnd)
{
	m_Direct3D = new DirectXManager;
	if (!m_Direct3D)
		return false;

	if (!m_Direct3D->Initialize(InScreenWidth, InScreenHeight, VSYNC_ENABLED, InHwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
		return false;

	m_Camera = new CameraClass;
	if (!m_Camera)
		return false;

	// ī�޶� ������ ����
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// m_Model ��ü ����
	m_Model = new ModelClass;
	if (!m_Model)
		return false;

	// m_Model ��ü �ʱ�ȭ
	if (!m_Model->Initialize(m_Direct3D->GetDevice(), L"../DirectX11_FrameWork/Model/cube.txt", L"../DirectX11_FrameWork/Image/seafloor.dds"))
	{
		MessageBox(InHwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// TextureShader ��ü ����
	m_LightShader = new LightShader;
	if (!m_LightShader)
	{
		return false;
	}

	if (!m_LightShader->Initialize(m_Direct3D->GetDevice(), InHwnd))
	{
		return false;
	}

	m_LightClass = new LightClass;
	if (m_LightClass == nullptr)
	{
		return false;
	}

	m_LightClass->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_LightClass->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_LightClass->SetDirection(0.0f, 0.0f, 1.0f);
	m_LightClass->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_LightClass->SetSpecularPower(32.0f);

	// m_ColorShader ��ü ����
	m_ColorShader = new ColorShader;
	if (!m_ColorShader)
	{
		return false;
	}

	// TextureShader ��ü ����
	m_TextureShader = new TextureShader;
	if (!m_TextureShader)
	{
		return false;
	}

	if (!m_TextureShader->Initialize(m_Direct3D->GetDevice(), InHwnd))
	{
		return false;
	}

	// m_ColorShader ��ü �ʱ�ȭ
	if (!m_ColorShader->Initialize(m_Direct3D->GetDevice(), InHwnd))
	{
		MessageBox(InHwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	m_Bitmap = new BitmapClass;
	if (!m_Bitmap)
	{
		return false;
	}

	// ��Ʈ�� ��ü �ʱ�ȭ
	if (!m_Bitmap->Initialize(m_Direct3D->GetDevice(), InScreenWidth, InScreenHeight, L"../DirectX11_FrameWork/Image/seafloor.dds", 255, 255))
	{
		MessageBox(InHwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}
	

	return true;
}

void GraphicsManager::Shutdown()
{
	if (m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = nullptr;
	}

	if (m_LightClass)
	{
		delete m_LightClass;
		m_LightClass = nullptr;
	}

	// m_ColorShader ��ü ��ȯ
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = nullptr;
	}

	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = nullptr;
	}

	// m_ColorShader ��ü ��ȯ
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = nullptr;
	}

	// m_Model ��ü ��ȯ
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = nullptr;
	}

	// m_Camera ��ü ��ȯ
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	// Direct3D ��ü ��ȯ
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = nullptr;
	}
}

bool GraphicsManager::Frame()
{
	return Render();
}

bool GraphicsManager::Render()
{
	m_Direct3D->BeginScene(0.0f, 1.0f, 1.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�
	XMMATRIX WorldMatrix, ViewMatrix, ProjectionMatrix, orthoMatrix, UIWorld;
	m_Direct3D->GetWorldMatrix(WorldMatrix);
	m_Camera->GetViewMatrix(ViewMatrix);
	m_Direct3D->GetProjectionMatrix(ProjectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	static float rotation = 0.0f;

	rotation += 0.01f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}
	UIWorld = WorldMatrix;
	WorldMatrix = XMMatrixRotationY(rotation);

	// �� ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� ������� �غ��մϴ�.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	////// ���� ���̴��� ����Ͽ� ���� ������
	//if (!m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), WorldMatrix, ViewMatrix, ProjectionMatrix))
	//{
	//	return false;
	//}

	//if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), WorldMatrix, ViewMatrix, ProjectionMatrix, m_Model->GetTexture()))
	//{
	//	return false;
	//}

	if (!m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount()
		, WorldMatrix, ViewMatrix, ProjectionMatrix, m_Model->GetTexture()
		, m_LightClass->GetDirection(), m_LightClass->GetDiffuseColor(), m_LightClass->GetAmbientColor()
		, m_Camera->GetPosition(), m_LightClass->GetSpecularColor(), m_LightClass->GetSpecularPower()))
	{
		return false;
	}

	// UI Manager Render�� �̵�//
	m_Direct3D->TurnZBufferOff();
	if (!m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 0, 350))
	{
		return false;
	}

	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), UIWorld, ViewMatrix, orthoMatrix, m_Bitmap->GetTexture()))
	{
		return false;
	}
	m_Direct3D->TurnZBufferOn();


	// ������ ������ ȭ�鿡 ���
	m_Direct3D->EndScene();

	return true;
}
