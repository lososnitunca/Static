// stdafx.h: включаемый файл для стандартных системных включаемых файлов
// или включаемых файлов для конкретного проекта, которые часто используются, но
// не часто изменяются
//

#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _USE_32BIT_TIME_T

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>

#include <windows.h>
#include <time.h>
#include <MT4ManagerAPI.h>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <fstream>

#pragma comment(lib, "Ws2_32.lib")

// TODO: Установите здесь ссылки на дополнительные заголовки, требующиеся для программы
