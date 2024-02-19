#pragma once

// File manipulation
#include <stdio.h>
#include <filesystem>

// Write to the console
#include <iostream>

// String manipulation
#include <string>
#include <cstring>
#include <format>

// Types
#include <cstdint>

// Threading
#include <mutex>

// Windows specific
#if defined(NK_PLATFORM_WINDOWS)
	#include <windows.h>
#endif

// Interfaces
#include <concepts>
#include <type_traits>
#include <optional>

// Memory manipulation
#include <memory>
#include <functional>
#include <initializer_list>
#include <iterator>

// Global files
#include "core/defines.h"
#include "core/logging.h"
#include "core/assertion.h"
