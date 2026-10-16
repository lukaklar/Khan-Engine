#pragma once
#include "core/containers.h"
#include "core/math.h"
#include "core/primitivetypes.h"
#include "core/singleton.h"
#include "system/input/inputenums.h"

namespace Khan
{
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
		int32 m_VirtualKeyCode;
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
		inline const Vector2I& GetCursorPosition() const { return m_CursorPosition; }
		inline const Vector2I& GetCursorDelta() const { return m_CursorDelta; }
		inline void ResetCursorDelta() { m_CursorDelta = Vector2I(0, 0); }

		void OnKeyPressed(int32 virtualKeycode, uint32 pressCount, InputType inputType);
		void OnMouseButtonPressed(MouseButton button, ClickType clickType, int32 x, int32 y);
		void OnCursorMoved(int32 x, int32 y);
		void OnRawCursorMove(int32 deltaX, int32 deltaY);

	private:
		void AddAction(InputContext context, InputAction action, int32 virtualKeyCode, TriggerType triggerType, double timeFrame);

	private:
		InputManager();

		TInlineArray<InputActionDescription, static_cast<size_t>( InputAction::Count )> m_aInputActionDescriptor;
		TArray<InputContext> m_ContextStack;
		TInlineArray<THashMap<int32, THashMap<InputAction, InputActionState>>, static_cast<size_t>( InputContext::Count )> m_InputMap;

		Vector2I m_CursorPosition;
		Vector2I m_CursorDelta;
	};
}