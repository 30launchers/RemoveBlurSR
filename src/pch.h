#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdio>
#include <thread>
#include <chrono>

#include "MinHook/include/MinHook.h"

#define LOG(fmt, ...) printf("[RemoveBlurSR] " fmt "\n", ##__VA_ARGS__)
#define ERR(fmt, ...) printf("[RemoveBlurSR::Error] " fmt "\n", ##__VA_ARGS__)
