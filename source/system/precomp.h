#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <condition_variable>
#include <fstream>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

// target Windows 10 or later
#define _WIN32_WINNT 0x0A00
#include <sdkddkver.h>
#include <Windows.h>
#include <shellapi.h>