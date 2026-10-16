#pragma once

#include <chrono>
#include <cstdint>
#include <condition_variable>
#include <fstream>
#include <functional>
#include <mutex>
#include <thread>

// target Windows 10 or later
#define _WIN32_WINNT 0x0A00
#include <sdkddkver.h>
#include <Windows.h>
#include <shellapi.h>