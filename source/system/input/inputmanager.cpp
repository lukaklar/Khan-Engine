#include "system/precomp.h"
#include "system/input/inputmanager.hpp"

InputManager::InputManager()
{
	PushContext(InputContext::Default);
	AddAction(InputContext::Default, InputAction::MoveForward, 'W', TriggerType::PressOnce, 0.0);
	AddAction(InputContext::Default, InputAction::MoveBack, 'S', TriggerType::PressOnce, 0.0);
	AddAction(InputContext::Default, InputAction::MoveLeft, 'A', TriggerType::PressOnce, 0.0);
	AddAction(InputContext::Default, InputAction::MoveRight, 'D', TriggerType::PressOnce, 0.0);
}

void InputManager::AddAction(InputContext context, InputAction action, int32_t virtualKeyCode, TriggerType triggerType, double timeFrame)
{
	m_aInputActionDescriptor[static_cast<size_t>(action)].m_Action = action;
	m_aInputActionDescriptor[static_cast<size_t>(action)].m_VirtualKeyCode = virtualKeyCode;
	m_aInputActionDescriptor[static_cast<size_t>(action)].m_TriggerType = triggerType;
	m_aInputActionDescriptor[static_cast<size_t>(action)].m_TimeFrame = timeFrame;

	m_InputMap[static_cast<size_t>(context)][virtualKeyCode][action].m_Active = false;
	m_InputMap[static_cast<size_t>(context)][virtualKeyCode][action].m_IsPressed = false;
}

void InputManager::PushContext(InputContext context)
{
	m_ContextStack.emplace_back(context);
}

void InputManager::PopContext()
{
	m_ContextStack.pop_back();
}

InputContext InputManager::PeekContext() const
{
	return m_ContextStack.back();
}

bool InputManager::IsActionActive(InputAction action) const
{
	size_t contextValue = static_cast<size_t>(PeekContext());
	int32_t virtualKeyCode = m_aInputActionDescriptor[static_cast<size_t>(action)].m_VirtualKeyCode;
	const auto& iter = m_InputMap[contextValue].find(virtualKeyCode);

	if (iter == m_InputMap[contextValue].end())
	{
		return false;
	}

	const auto& it = iter->second.find(action);

	return it != iter->second.end() && it->second.m_Active;
}

void InputManager::OnKeyPressed(int32_t virtualKeycode, uint32_t pressCount, InputType inputType)
{
	size_t contextValue = static_cast<size_t>(PeekContext());
	const auto& iter = m_InputMap[contextValue].find(virtualKeycode);

	// If the current input context doesn't have any action bound to the virtual key code then there is nothing to set and we can leave
	if (iter == m_InputMap[contextValue].end())
	{
		return;
	}

	// Otherwise we get all of the input actions bound to the virtual key code
	std::unordered_map<InputAction, InputActionState>& actionControl = iter->second;

	// Iterate and check the conditions for every action and if it is fulfilled set it to active = true
	for (auto& it : actionControl)
	{
		InputActionState& state = it.second;
		const InputActionDescription& desc = m_aInputActionDescriptor[static_cast<size_t>(it.first)];

		if (desc.m_TriggerType == TriggerType::PressOnce)
		{
			// If it requires only the key to be down to be active
			state.m_Active = inputType == InputType::Press || inputType == InputType::Repeat;
		}
		else if (desc.m_TriggerType == TriggerType::PressTwice && inputType == InputType::Press)
		{
			// If it was previously pressed then set it to active and reset pressed if within allowed time frame
			if (state.m_IsPressed)
			{
				std::chrono::duration<double, std::milli> duration = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(std::chrono::high_resolution_clock::now() - state.m_TimePressed);
				state.m_Active = duration.count() <= desc.m_TimeFrame;
				state.m_IsPressed = false;
			}
			else
			{
				state.m_IsPressed = true;
				state.m_TimePressed = std::chrono::high_resolution_clock::now();
			}
		}
		else if (desc.m_TriggerType == TriggerType::Hold)
		{
			// if first press then prepare it for the upcoming repeats
			if (inputType == InputType::Press)
			{
				state.m_Active = false;
				state.m_IsPressed = true;
				state.m_TimePressed = std::chrono::high_resolution_clock::now();
			}
			else if (inputType == InputType::Repeat)
			{
				if (state.m_IsPressed)
				{
					std::chrono::duration<double, std::milli> duration = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(std::chrono::high_resolution_clock::now() - state.m_TimePressed);
					if (duration.count() >= desc.m_TimeFrame)
					{
						state.m_Active = true;
						state.m_IsPressed = false;
					}
				}
				else
				{
					// If the key is pressed but it was already active before, then reset it and start the count again
					state.m_Active = false;
					state.m_IsPressed = true;
					state.m_TimePressed = std::chrono::high_resolution_clock::now();
				}
			}
		}
		else if (desc.m_TriggerType == TriggerType::Release)
		{
			state.m_Active = inputType == InputType::Release;
		}
	}
}

void InputManager::OnMouseButtonPressed(MouseButton button, ClickType clickType, int32_t x, int32_t y)
{

}

void InputManager::OnCursorMoved(int32_t x, int32_t y)
{
	m_CursorPosition = { x, y };
}

void InputManager::OnRawCursorMove(int32_t deltaX, int32_t deltaY)
{
	m_CursorDelta += glm::ivec2(deltaX, deltaY);
}