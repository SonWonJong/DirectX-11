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
#include "TextClass.h"
#include "InputManager.h"
#include "FrustumClass.h"
#include "ModelListClass.h"

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

	// 카메라 포지션 설정
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// m_Model 객체 생성
	m_Model = new ModelClass;
	if (!m_Model)
		return false;

	// m_Model 객체 초기화
	if (!m_Model->Initialize(m_Direct3D->GetDevice(), L"../DirectX11_FrameWork/Model/sphere.txt", L"../DirectX11_FrameWork/Image/seafloor.dds"))
	{
		MessageBox(InHwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// TextureShader 객체 생성
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

	// m_ColorShader 객체 생성
	m_ColorShader = new ColorShader;
	if (!m_ColorShader)
	{
		return false;
	}

	// TextureShader 객체 생성
	m_TextureShader = new TextureShader;
	if (!m_TextureShader)
	{
		return false;
	}

	if (!m_TextureShader->Initialize(m_Direct3D->GetDevice(), InHwnd))
	{
		return false;
	}

	// m_ColorShader 객체 초기화
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

	// 비트맵 객체 초기화
	if (!m_Bitmap->Initialize(m_Direct3D->GetDevice(), InScreenWidth, InScreenHeight, L"../DirectX11_FrameWork/Image/seafloor.dds", 255, 255))
	{
		MessageBox(InHwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}
	

	// 카메라 포지션 설정
	XMMATRIX baseViewMatrix;
	//m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	// m_Text 객체 생성
	m_Text = new TextClass;
	if (!m_Text)
	{
		return false;
	}

	// m_Text 객체 초기화
	if (!m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), InHwnd, InScreenWidth, InScreenHeight, baseViewMatrix))
	{
		MessageBox(InHwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}


	m_fps = 0;
	m_cpu = 0;


	m_ModelList = new ModelListClass;
	if (!m_ModelList)
	{
		return false;
	}

	if (!m_ModelList->Initialize(50))
	{
		return false;
	}

	m_Frustum = new FrustumClass;
	if (!m_Frustum)
	{
		return false;
	}
	
	return true;
}

void GraphicsManager::Shutdown()
{
	// m_Text 객체 반환
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

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

	// m_ColorShader 객체 반환
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

	// m_ColorShader 객체 반환
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = nullptr;
	}

	// m_Model 객체 반환
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = nullptr;
	}

	// m_Camera 객체 반환
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	// Direct3D 객체 반환
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = nullptr;
	}

	if (m_ModelList)
	{
		m_ModelList->Shutdown();
		delete m_ModelList;
		m_ModelList = nullptr;
	}


	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = nullptr;
	}
}

bool GraphicsManager::Frame()
{
	// Mouse Pos
	//int MouseX = 0, MouseY = 0;

	//InputManager::GetInstance().GetMouseLocation(MouseX, MouseY);

	//if (!m_Text->SetMousePosition(MouseX, MouseY, m_Direct3D->GetDeviceContext()))
	//{
	//	return false;
	//}
	// Mouse Pos



	if (!m_Text->SetFps(m_fps, m_Direct3D->GetDeviceContext()))
	{
		return false;
	}

	if (!m_Text->SetCpu(m_cpu, m_Direct3D->GetDeviceContext()))
	{
		return false;
	}

	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	return Render();
}

bool GraphicsManager::CubeAndTextureRender()
{
	m_Direct3D->BeginScene(0.0f, 1.0f, 1.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다
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

	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	////// 색상 쉐이더를 사용하여 모델을 렌더링
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

	// UI Manager Render로 이동//
	m_Direct3D->TurnZBufferOff();


	if (!m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 0, 350))
	{
		return false;
	}

	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), UIWorld, ViewMatrix, orthoMatrix, m_Bitmap->GetTexture()))
	{
		return false;
	}

	/////////////////Alpha
	m_Direct3D->TurnOnAlphaBlending();
	if (!m_Text->Render(m_Direct3D->GetDeviceContext(), UIWorld, orthoMatrix))
	{
		return false;
	}
	m_Direct3D->TurnOffAlphaBlending();
	/////////////////Alpha


	m_Direct3D->TurnZBufferOn();


	// 버퍼의 내용을 화면에 출력
	m_Direct3D->EndScene();

	return true;
}
bool GraphicsManager::FrustumRender()
{
	m_Direct3D->BeginScene(0.0f, 1.0f, 1.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다
	XMMATRIX WorldMatrix, ViewMatrix, ProjectionMatrix, orthoMatrix, UIWorld;
	m_Direct3D->GetWorldMatrix(WorldMatrix);
	m_Camera->GetViewMatrix(ViewMatrix);
	m_Direct3D->GetProjectionMatrix(ProjectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	m_Frustum->ConstructFrustum(SCREEN_DEPTH, ProjectionMatrix, ViewMatrix);


	// 렌더될 모델의 수를 얻음
	int modelCount = m_ModelList->GetModelCount();

	// 렌더될 모델의 개수를 초기화한다.
	int renderCount = 0;

	// 모든 모델을 살피고 카메라 뷰에서 볼수 있는 경우에만 렌더링한다.
	float positionX, positionY, positionZ;
	float radius = 1.0f; // 구의 반지름을 1.0f로 설정
	XMFLOAT4 color;
	for (int index = 0; index < modelCount; index++)
	{
		// 인덱스에서 구형 모델의 위치와 색상을 가져온다.
		m_ModelList->GetData(index, positionX, positionY, positionZ, color);

		// 구형 모델이 뷰 Frustum에 있는지 확인한다.
		if (m_Frustum->CheckSphere(positionX, positionY, positionZ, radius))
		{
			// 모델을 렌더링할 위치로 이동
			WorldMatrix = XMMatrixTranslation(positionX, positionY, positionZ);

			//모델 버텍스와 인덱스 버퍼를 그래픽 파이프라인에 배치하여 드로우잉 준비
			m_Model->Render(m_Direct3D->GetDeviceContext());

			// 라이트 쉐이더를 사용하여 모델을 렌더링합니다.
			m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount()
				, WorldMatrix, ViewMatrix, ProjectionMatrix, m_Model->GetTexture()
				, m_LightClass->GetDirection(), m_LightClass->GetDiffuseColor(), m_LightClass->GetAmbientColor()
				, m_Camera->GetPosition(), m_LightClass->GetSpecularColor(), m_LightClass->GetSpecularPower());

			m_Direct3D->GetWorldMatrix(WorldMatrix);

			// 이 모델이 렌더링 되었기 때문에 프레임의 수를 늘림
			renderCount++;
		}
	}



	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	////// 색상 쉐이더를 사용하여 모델을 렌더링
	//if (!m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), WorldMatrix, ViewMatrix, ProjectionMatrix))
	//{
	//	return false;
	//}

	//if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), WorldMatrix, ViewMatrix, ProjectionMatrix, m_Model->GetTexture()))
	//{
	//	return false;
	//}

	//if (!m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount()
	//	, WorldMatrix, ViewMatrix, ProjectionMatrix, m_Model->GetTexture()
	//	, m_LightClass->GetDirection(), m_LightClass->GetDiffuseColor(), m_LightClass->GetAmbientColor()
	//	, m_Camera->GetPosition(), m_LightClass->GetSpecularColor(), m_LightClass->GetSpecularPower()))
	//{
	//	return false;
	//}

	// UI Manager Render로 이동//
	m_Direct3D->TurnZBufferOff();


	if (!m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 0, 350))
	{
		return false;
	}

	//if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), UIWorld, ViewMatrix, orthoMatrix, m_Bitmap->GetTexture()))
	//{
	//	return false;
	//}

	/////////////////Alpha
	m_Direct3D->TurnOnAlphaBlending();
	//if (!m_Text->Render(m_Direct3D->GetDeviceContext(), UIWorld, orthoMatrix))
	//{
	//	return false;
	//}
	m_Direct3D->TurnOffAlphaBlending();
	/////////////////Alpha


	m_Direct3D->TurnZBufferOn();


	// 버퍼의 내용을 화면에 출력
	m_Direct3D->EndScene();

	return true;
}

bool GraphicsManager::Render()
{
	return FrustumRender();
}
