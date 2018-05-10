#include "stdafx.h"
#include "InputManager.h"

InputManager::InputManager()
{
}


InputManager::~InputManager()
{
}

bool InputManager::Initialize(HINSTANCE InHinstance, HWND InHwnd, int InScreenWidth, int InScreenHeight)
{
	m_ScreenWidth = InScreenWidth;
	m_ScreenHeight = InScreenHeight;
	m_MouseX = m_ScreenWidth / 2;
	m_MouseY = m_ScreenHeight / 2;

	// Direct Input 인터페이스를 초기화 합니다.
	HRESULT result = DirectInput8Create(InHinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_DirectInput, NULL);
	if (FAILED(result))
	{
		return false;
	}

	result = m_DirectInput->CreateDevice(GUID_SysKeyboard, &m_Keyboard, NULL);
	if (FAILED(result))
	{
		return false;
	}
	result = m_Keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}
	// 다른 프로그램과 공유하지 않도록 키보드의 협조 수준을 설정
	result = m_Keyboard->SetCooperativeLevel(InHwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// 키보드를 할당
	result = m_Keyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	// 마우스 Direct Input 인터페이스를 생성
	result = m_DirectInput->CreateDevice(GUID_SysMouse, &m_Mouse, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 마우스의 데이터 형식을 설정
	result = m_Mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}

	// 마우스의 협력 수준을 설정
	result = m_Mouse->SetCooperativeLevel(InHwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// 마우스를 할당
	result = m_Mouse->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void InputManager::Shutdown()
{
	// 마우스를 반환합니다.
	if (m_Mouse)
	{
		m_Mouse->Unacquire();
		m_Mouse->Release();
		m_Mouse = nullptr;
	}

	// 키보드를 반환합니다.
	if (m_Keyboard)
	{
		m_Keyboard->Unacquire();
		m_Keyboard->Release();
		m_Keyboard = nullptr;
	}

	// m_directInput 객체를 반환합니다.
	if (m_DirectInput)
	{
		m_DirectInput->Release();
		m_DirectInput = nullptr;
	}
}

bool InputManager::Frame()
{
	if (!ReadKeyboard())
	{
		return false;
	}

	if (!ReadMouse())
	{
		return false;
	}

	ProcessInput();

	return true;
}

bool InputManager::ReadKeyboard()
{
	// 키보드 디바이스를 얻는다.
	HRESULT result = m_Keyboard->GetDeviceState(sizeof(m_KeyboardState), (LPVOID)&m_KeyboardState);
	if (FAILED(result))
	{
		// 키보드가 포커스를 잃었거나 획득되지 않은 경우 컨트롤을 다시 가져 온다
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_Keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool InputManager::ReadMouse()
{
	// 마우스 디바이스를 얻는다.
	HRESULT result = m_Mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_MouseState);
	if (FAILED(result))
	{
		// 마우스가 포커스를 잃었거나 획득되지 않은 경우 컨트롤을 다시 가져온다
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_Mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void InputManager::ProcessInput()
{
	// 프레임 동안 마우스 위치의 변경을 기반으로 마우스 커서의 위치를 업데이트 합니다.
	m_MouseX += m_MouseState.lX;
	m_MouseY += m_MouseState.lY;

	if (m_MouseState.lX > 1)
	{
		int j = 0;
		j = 11;
	}

	// 마우스 위치가 화면 너비 또는 높이를 초과하지 않는지 확인한다.
	if (m_MouseX < 0) 
		m_MouseX = 0;
	if (m_MouseY < 0) 
		m_MouseY = 0; 
	if (m_MouseX > m_ScreenWidth) 
		m_MouseX = m_ScreenWidth;
	if (m_MouseY > m_ScreenHeight) 
		m_MouseY = m_ScreenHeight;
}

void InputManager::GetMouseLocation(int& InMouseX, int& InMouseY)
{
	InMouseX = m_MouseX;
	InMouseY = m_MouseY;
}

bool InputManager::IsKeyPressed(int InCheckKey)
{
	if (InCheckKey < MaxKeyCount && m_KeyboardState[InCheckKey] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputManager::IsEscapePressed()
{
	if (m_KeyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputManager::IsLeftPressed()
{
	if (m_KeyboardState[DIK_LEFT] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsRightPressed()
{
	if (m_KeyboardState[DIK_RIGHT] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsUpPressed()
{
	if (m_KeyboardState[DIK_UP] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsDownPressed()
{
	if (m_KeyboardState[DIK_DOWN] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsAPressed()
{
	if (m_KeyboardState[DIK_A] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsZPressed()
{
	if (m_KeyboardState[DIK_Z] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsPgUpPressed()
{
	if (m_KeyboardState[DIK_PGUP] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsPgDownPressed()
{
	if (m_KeyboardState[DIK_PGDN] & 0x80)
	{
		return true;
	}

	return false;
}
