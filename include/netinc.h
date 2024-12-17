// #pragma once

// #ifdef NB_NO_LOGGING
// #define NBN_LogInfo(...)
// #define NBN_LogError(...)
// #define NBN_LogDebug(...)
// #define NBN_LogTrace(...)
// #define NBN_LogWarning(...)
// #else
// #define NBN_LogInfo(...) TraceLog(LOG_INFO, __VA_ARGS__)
// #define NBN_LogError(...) TraceLog(LOG_ERROR, __VA_ARGS__)
// #define NBN_LogDebug(...) TraceLog(LOG_DEBUG, __VA_ARGS__)
// #define NBN_LogTrace(...) TraceLog(LOG_TRACE, __VA_ARGS__)
// #define NBN_LogWarning(...) TraceLog(LOG_WARNING, __VA_ARGS__)
// #endif

// #define NBN_Allocator malloc
// #define NBN_Reallocator realloc
// #define NBN_Deallocator free

// // If defined, the following flags inhibit definition of the indicated items
// #define NOGDICAPMASKS // CC_*, LC_*, PC_*, CP_*, TC_*, RC_
// #define NOVIRTUALKEYCODES // VK_*
// #define NOWINMESSAGES // WM_*, EM_*, LB_*, CB_*
// #define NOWINSTYLES // WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
// #define NOSYSMETRICS // SM_*
// #define NOMENUS // MF_*
// #define NOICONS // IDI_*
// #define NOKEYSTATES // MK_*
// #define NOSYSCOMMANDS // SC_*
// #define NORASTEROPS // Binary and Tertiary raster ops
// #define NOSHOWWINDOW // SW_*
// #define OEMRESOURCE // OEM Resource values
// #define NOATOM // Atom Manager routines
// #define NOCLIPBOARD // Clipboard routines
// #define NOCOLOR // Screen colors
// #define NOCTLMGR // Control and Dialog routines
// #define NODRAWTEXT // DrawText() and DT_*
// #define NOGDI // All GDI defines and routines
// #define NOKERNEL // All KERNEL defines and routines
// #define NOUSER // All USER defines and routines
// /*#define NONLS // All NLS defines and routines*/
// #define NOMB // MB_* and MessageBox()
// #define NOMEMMGR // GMEM_*, LMEM_*, GHND, LHND, associated routines
// #define NOMETAFILE // typedef METAFILEPICT
// #define NOMINMAX // Macros min(a,b) and max(a,b)
// #define NOMSG // typedef MSG and associated routines
// #define NOOPENFILE // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
// #define NOSCROLL // SB_* and scrolling routines
// #define NOSERVICE // All Service Controller routines, SERVICE_ equates, etc.
// #define NOSOUND // Sound driver routines
// #define NOTEXTMETRIC // typedef TEXTMETRIC and associated routines
// #define NOWH // SetWindowsHook and WH_*
// #define NOWINOFFSETS // GWL_*, GCL_*, associated routines
// #define NOCOMM // COMM driver routines
// #define NOKANJI // Kanji support stuff.
// #define NOHELP // Help engine interface.
// #define NOPROFILER // Profiler interface.
// #define NODEFERWINDOWPOS // DeferWindowPos routines
// #define NOMCX // Modem Configuration Extensions

// // Type required before windows.h inclusion
// typedef struct tagMSG *LPMSG;
// #include <winsock2.h> // Has to be included before windows.h
// #include <windows.h>

// // #pragma warning(push)
// // #pragma warning(disable : 4244)
// // #pragma warning(disable : 4068)
// // #pragma warning(disable : 4996)
// // #include "nbnet.h"
// // #include "udp.h"
// // #pragma warning(pop)

// Thanks to Raysan
#if defined(_WIN32)
// To avoid conflicting windows.h symbols with raylib, some flags are defined
// WARNING: Those flags avoid inclusion of some Win32 headers that could be required
// by user at some point and won't be included...
//-------------------------------------------------------------------------------------

// If defined, the following flags inhibit definition of the indicated items.
#define NOGDICAPMASKS     // CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define NOVIRTUALKEYCODES // VK_*
#define NOWINMESSAGES     // WM_*, EM_*, LB_*, CB_*
#define NOWINSTYLES       // WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#define NOSYSMETRICS      // SM_*
#define NOMENUS           // MF_*
#define NOICONS           // IDI_*
#define NOKEYSTATES       // MK_*
#define NOSYSCOMMANDS     // SC_*
#define NORASTEROPS       // Binary and Tertiary raster ops
#define NOSHOWWINDOW      // SW_*
#define OEMRESOURCE       // OEM Resource values
#define NOATOM            // Atom Manager routines
#define NOCLIPBOARD       // Clipboard routines
#define NOCOLOR           // Screen colors
#define NOCTLMGR          // Control and Dialog routines
#define NODRAWTEXT        // DrawText() and DT_*
#define NOGDI             // All GDI defines and routines
#define NOKERNEL          // All KERNEL defines and routines
#define NOUSER            // All USER defines and routines
//#define NONLS             // All NLS defines and routines
#define NOMB              // MB_* and MessageBox()
#define NOMEMMGR          // GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE        // typedef METAFILEPICT
#define NOMINMAX          // Macros min(a,b) and max(a,b)
#define NOMSG             // typedef MSG and associated routines
#define NOOPENFILE        // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL          // SB_* and scrolling routines
#define NOSERVICE         // All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND           // Sound driver routines
#define NOTEXTMETRIC      // typedef TEXTMETRIC and associated routines
#define NOWH              // SetWindowsHook and WH_*
#define NOWINOFFSETS      // GWL_*, GCL_*, associated routines
#define NOCOMM            // COMM driver routines
#define NOKANJI           // Kanji support stuff.
#define NOHELP            // Help engine interface.
#define NOPROFILER        // Profiler interface.
#define NODEFERWINDOWPOS  // DeferWindowPos routines
#define NOMCX             // Modem Configuration Extensions

// Type required before windows.h inclusion
typedef struct tagMSG *LPMSG;

#include <windows.h>

// Type required by some unused function...
typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

#include <objbase.h>
#include <mmreg.h>
#include <mmsystem.h>

// Some required types defined for MSVC/TinyC compiler
#if defined(_MSC_VER) || defined(__TINYC__)
    #include "propidl.h"
#endif
#endif

