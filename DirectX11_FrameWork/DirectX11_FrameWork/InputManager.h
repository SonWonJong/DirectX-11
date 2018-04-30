#pragma once

class InputManager
{
public:
	InputManager();
	~InputManager();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	void GetMouseLocation(int&, int&);

	bool IsKeyPressed(int);
	bool IsEscapePressed();
	bool IsLeftPressed();
	bool IsRightPressed();
	bool IsUpPressed();
	bool IsDownPressed();
	bool IsAPressed();
	bool IsZPressed();
	bool IsPgUpPressed();
	bool IsPgDownPressed();

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8 * m_DirectInput = nullptr;
	IDirectInputDevice8* m_Keyboard = nullptr;
	IDirectInputDevice8* m_Mouse = nullptr;
	DIMOUSESTATE m_MouseState;

	int m_ScreenWidth = 0;
	int m_ScreenHeight = 0;
	int m_MouseX = 0;
	int m_MouseY = 0;
	static const int MaxKeyCount = 256;

	unsigned char m_KeyboardState[MaxKeyCount] = { 0, };
};