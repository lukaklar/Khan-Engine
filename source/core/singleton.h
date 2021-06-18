#pragma once
#include "core/core_dll.h"
#include "core/noncopyable.h"

template<typename T>
class KH_CORE_DLL Singleton : public NonCopyable
{
public:
	static void CreateSingleton() { s_pInstance = new T(); }
	static T* Get() { return s_pInstance; }
	static T& GetInstance() { return *s_pInstance; }
	static void DestroySingleton() { delete s_pInstance; }

protected:
	Singleton() = default;
	~Singleton() = default;

	Singleton(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton& operator=(Singleton&&) = delete;

	inline static T* s_pInstance = nullptr;
};