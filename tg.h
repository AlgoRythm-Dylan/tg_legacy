// TODO: Allocate extra large buffers to avoid allocation every resize

#ifndef TG_H
#define TG_H

#define TG_VERSION "2.1.1"
// 3600 is default # of cells for cmd.exe
#define TG_DEFAULT_BUFFER_LENGTH 3600

#define TG_BLACK 0
#define TG_RED 1
#define TG_GREEN 2
#define TG_YELLOW 3
#define TG_BLUE 4
#define TG_MAGENTA 5
#define TG_CYAN 6
#define TG_WHITE 7

#include <stdbool.h>
#include <wchar.h>

static int TGCurrentColorID = 1; // 0 is reserved in ncurses for default pair. This won't even be use in Windows

typedef struct {
    bool underlined;
    bool bold;
    unsigned short attributes;
    unsigned int color;
} TGAttributes;

typedef struct {
    wchar_t character;
    TGAttributes attributes;
} TGCharInfo;

typedef struct {
    unsigned int id;
    unsigned short foreground, background;
    bool foregroundBright, backgroundBright;
} TGColor;

#ifdef _WIN32
#include <Windows.h>
#define TG_WINDOWS_MODE true
typedef CHAR_INFO* TGSystemBuffer;
#else
#define TG_WINDOWS_MODE false

// I will keep the COORD structure from Windows
typedef struct {
    int X, Y;
} COORD;

#include <ncursesw/curses.h>
#include <locale.h>
typedef WINDOW* HANDLE; // Let's make this easy
typedef WINDOW* TGSystemBuffer;
#endif // Not Win32

// A drawing buffer, for general purposes
typedef struct {
	COORD size;
	unsigned int length;
    TGSystemBuffer systemDrawBuffer;
    TGCharInfo *buffer;
    TGAttributes currentAttributes;
    COORD virtualCursorPosition;
} TGBuffer;

#define TG_KEY_UP 0
#define TG_KEY_RIGHT 1
#define TG_KEY_DOWN 2
#define TG_KEY_LEFT 3
#define TG_KEY_BACKSPACE 4
#define TG_KEY_TAB 5
#define TG_KEY_ESCAPE 6
#define TG_KEY_PAGE_UP 7
#define TG_KEY_PAGE_DOWN 8
#define TG_KEY_END 9
#define TG_KEY_HOME 10
#define TG_KEY_INSERT 11
#define TG_KEY_DELETE 12

int TGIsSpecialKey(int);

typedef struct {
    unsigned int key;
    bool ctrlDown, special;
} TGKeyEvent;

#define TG_MOUSE_LEFT 1
#define TG_MMB 3
#define TG_MOUSE_RIGHT 2

#define TG_MOUSE_CLICK 1
#define TG_MOUSE_MOVE 2

typedef struct {
    COORD position;
    unsigned short button;
    unsigned short action;
} TGMouseEvent;

typedef struct {
    COORD oldSize;
    COORD newSize;
} TGResizeEvent;

#define TG_EVENT_KEY 1
#define TG_EVENT_MOUSE 2
#define TG_EVENT_RESIZE 4

typedef struct {
    short eventType;
    bool empty;
    union {
        TGKeyEvent keyEvent;
        TGMouseEvent mouseEvent;
        TGResizeEvent resizeEvent;
    } event;
} TGInput;

// Essentially a drawing context to the screen
typedef struct {
	HANDLE screenBufferHandle, inputHandle;
	TGBuffer drawBuffer;        //  Needs to be TGBufFree'd
} TGContext;

TGBuffer TGBufCreate(int, int); // Function to allocate a drawing buffer
//TGBuffer TGBufDuplicate(TGBuffer*); // Deep copy one buffer to another
void TGBufSize(TGBuffer*, int, int); // Resize a draw buffer
void TGBufClear(TGBuffer*); // Fill a buffer with blank cells
void TGBufCell(TGBuffer*, int, int, TGCharInfo); // Draw to a single cell on the buffer
void TGBufAttr(TGBuffer*, int, int, TGAttributes);
void TGBufFree(TGBuffer*); // Function to de-allocate a drawing buffer
void TGCalculateAttrs(TGAttributes*);
void TGBufCursorPosition(TGBuffer*, int, int);
void TGBufCursorMove(TGBuffer*, int);
void TGBufAddLString(TGBuffer*, char*); // Add legacy string
void TGBufAddLStringAttr(TGBuffer*, char*, TGAttributes); // Add legacy string without using buffer current attrs
void TGBufAddString(TGBuffer*, wchar_t*);
void TGBufAddStringAttr(TGBuffer*, wchar_t*, TGAttributes);

TGContext* TG(); // Initialization function, which returns a drawing context to the screen
void TGUpdate(); // Displays what has been drawn
void TGSetCursorVisible(bool);
TGInput TGGetInput();
void TGHandleResizeEvent(TGInput); // Resize is not handled automatically
void TGTitle(const char*);
void TGSetCursorPosition(int, int);
COORD TGGetCursorPosition();
void TGEnd();

int TGColorID();
TGColor TGColorCreate(int, int);

static TGContext TGMainContext = {0};
static long TGPreviousInputMode;

#endif // TG_H