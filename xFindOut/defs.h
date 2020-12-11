#pragma once

#ifdef _WIN64
#define HEX_PRINT "%llX"
#else
#define HEX_PRINT "%X"
#endif //_WIN64