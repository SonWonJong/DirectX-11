#include "stdafx.h"
#include "cameraclass.h"


CameraClass::CameraClass()
{
	m_Position = XMFLOAT3(0.0f, 0.0f, 0.0f);;
	m_Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
}


CameraClass::CameraClass(const CameraClass& other)
{
}


CameraClass::~CameraClass()
{
}


void CameraClass::SetPosition(float x, float y, float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}


void CameraClass::SetRotation(float x, float y, float z)
{
	m_Rotation.x = x;
	m_Rotation.y = y;
	m_Rotation.z = z;
}


XMFLOAT3 CameraClass::GetPosition()
{
	return m_Position;
}


XMFLOAT3 CameraClass::GetRotation()
{
	return m_Rotation;
}


void CameraClass::Render()
{
	float Yaw, Pitch, Roll;
	
	XMFLOAT3 Up(0.0f, 1.0f, 0.0f);
	XMFLOAT3 Position(m_Position);
	XMFLOAT3 LookAt(0.0f, 0.0f, 1.0f);

	XMVECTOR UpVector, PositionVector, LookAtVector;
	XMMATRIX RotationMatrix;

	UpVector = XMLoadFloat3(&Up);
	PositionVector = XMLoadFloat3(&Position);
	LookAtVector = XMLoadFloat3(&LookAt);

	// yaw (Y 축), pitch (X 축) 및 roll (Z 축)의 회전값을 라디안 단위로 설정합니다.
	Pitch = DirectX::XMConvertToDegrees(m_Rotation.x);
	Yaw = DirectX::XMConvertToDegrees(m_Rotation.y);
	Roll = DirectX::XMConvertToDegrees(m_Rotation.z);

	// yaw, pitch, roll을 통해 회전 행렬을 만듬
	RotationMatrix = XMMatrixRotationRollPitchYaw(Pitch, Yaw, Roll);

	// lookat 및 up 벡터를 회전행렬로 변형하여 뷰가 원점에서 올바르게 회전하도록 함
	LookAtVector = XMVector3TransformCoord(LookAtVector, RotationMatrix);
	UpVector = XMVector3TransformCoord(UpVector, RotationMatrix);

	// 회전 된 카메라 위치를 뷰어 위치로 변환합니다.
	LookAtVector = XMVectorAdd(PositionVector, LookAtVector);

	// 마지막으로 세 개의 업데이트 된 벡터에서 뷰 행렬을 만듭니다.
	m_ViewMatrix = XMMatrixLookAtLH(PositionVector, LookAtVector, UpVector);

}


void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_ViewMatrix;
}
