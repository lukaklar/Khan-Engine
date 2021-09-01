#pragma once
#include "core/singleton.h"
#include "system/input/inputenums.h"
#include <thirdparty/glm/glm.hpp>

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
	int32_t m_VirtualKeyCode;
	TriggerType m_TriggerType;
	double m_TimeFrame;
};

struct InputActionState
{
	bool m_Active;
	bool m_IsPressed;
	std::chrono::high_resolution_clock::time_point m_TimePressed;
};

class InputManager : public Singleton<InputManager>
{
	friend class Singleton<InputManager>;
public:
	void PushContext(InputContext context);
	void PopContext();
	InputContext PeekContext() const;

	bool IsActionActive(InputAction action) const;
	inline const glm::ivec2& GetCursorPosition() const { return m_CursorPosition; }
	inline const glm::ivec2& GetCursorDelta() const { return m_CursorDelta; }
	inline void ResetCursorDelta() { m_CursorDelta = glm::ivec2(0, 0); }
	
	void OnKeyPressed(int32_t virtualKeycode, uint32_t pressCount, InputType inputType);
	void OnMouseButtonPressed(MouseButton button, ClickType clickType, int32_t x, int32_t y);
	void OnCursorMoved(int32_t x, int32_t y);
	void OnRawCursorMove(int32_t deltaX, int32_t deltaY);

private:
	void AddAction(InputContext context, InputAction action, int32_t virtualKeyCode, TriggerType triggerType, double timeFrame);

private:
	InputManager();

	std::array<InputActionDescription, static_cast<size_t>(InputAction::Count)> m_aInputActionDescriptor;
	std::vector<InputContext> m_ContextStack;
	std::array<std::unordered_map<int32_t, std::unordered_map<InputAction, InputActionState>>, static_cast<size_t>(InputContext::Count)> m_InputMap;

	glm::ivec2 m_CursorPosition;
	glm::ivec2 m_CursorDelta;
};