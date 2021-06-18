#pragma once

class KH_CORE_DLL NonCopyable
{
protected:
	NonCopyable() = default;
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
};