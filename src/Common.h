#pragma once

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <thread>
#include <chrono>
#include <memory>
#include <string>
#include <mutex>
#include <vector>
#include <map>
#include <exception>

#include "resource.h"
#include "SIDefs.h"
#include "SIErrors.h"
#include "SIException.h"
#include "PE/SharedResource.h"

#include <asmjit/asmjit.h>
