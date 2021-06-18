#pragma once
#include "core/singleton.h"
#include "system/input/inputenums.h"

enum class MouseButton
{
	Left,
	Right,
	Middle,
	Count
};

enum class ClickType
{
	Single,
	Double,
	Release,
	Count
};

enum class InputType
{
	Press,
	Repeat,
	Release,
	Count
};

enum class TriggerType
{
	PressOnce,
	PressTwice,
	Hold,
	Release,
	Count
};

struct InputActionDescription
{
	InputAction m_Action;
	int m_VirtualKeyCode;
	TriggerType m_TriggerType;
	double m_TimeFrame;
};

struct InputActionState
{
	bool m_Active;
	bool m_IsPressed;
	std::chrono::high_resolution_clock::time_point m_TimePressed;
};

class InputManager final : public Singleton<InputManager>
{
	friend class Singleton<InputManager>;
public:
	void PushContext(InputContext context);
	void PopContext();
	InputContext PeekContext() const;

	bool IsActionActive(InputAction action) const;
	std::pair<int, int> GetCursorPosition() const { return m_CursorPosition; }
	
	void OnKeyPressed(int virtualKeycode, unsigned int pressCount, InputType inputType);
	void OnMouseButtonPressed(MouseButton button, ClickType clickType, int x, int y);
	void OnCursorMoved(int x, int y);


private:
	void AddAction(InputContext context, InputAction action, int virtualKeyCode, TriggerType triggerType, double timeFrame);

private:
	InputManager();

	std::array<InputActionDescription, static_cast<size_t>(InputAction::Count)> m_aInputActionDescriptor;
	std::vector<InputContext> m_ContextStack;
	std::array<std::unordered_map<int, std::unordered_map<InputAction, InputActionState>>, static_cast<size_t>(InputContext::Count)> m_InputMap;

	std::pair<int, int> m_CursorPosition;
};