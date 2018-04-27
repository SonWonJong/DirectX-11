#pragma once
class InputManager
{
public:
	InputManager();
	~InputManager();

	void Initialize();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);

private:
	bool m_Keys[256];
};

