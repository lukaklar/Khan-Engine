#pragma once
//#include "system/timer.h"
//
//enum class EMouseButton
//{
//	Left,
//	Right,
//	Middle
//};
//
//enum class EClickType
//{
//	Single,
//	Double,
//	Release,
//	WheelUp,
//	WheelDown
//};
//
//enum class EInputType
//{
//	Press,
//	Repeat,
//	Release
//};
//
//enum class ETriggerType
//{
//	PressOnce,
//	PressTwice,
//	Hold,
//	Release
//};
//
//enum class EInputAction
//{
//	None,
//	MoveForward,
//	MoveBackward,
//	StrafeLeft,
//	StrafeRight,
//	Count
//};
//
//class InputAction
//{
//public:
//	bool IsActive() const { return m_Active; }
//
//private:
//	bool m_Active;
//	bool m_IsPressed;
//	Timer m_Timer;
//
//	// description
//	EInputAction m_ActionID;
//	int32_t m_VirtualKeyCode; // you can have multiple
//	ETriggerType m_TriggerType;
//	float m_TimeFrame; // seconds
//};