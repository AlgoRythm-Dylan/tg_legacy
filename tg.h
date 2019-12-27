// TODO: Allocate extra large buffers to avoid allocation every resize

#ifndef TG_H
#define TG_H

#define TG_BLACK 0
#define TG_RED 1
#define TG_GREEN 2
#define TG_YELLOW 3
#define TG_BLUE 4
#define TG_MAGENTA 5
#define TG_CYAN 6
#define TG_WHITE 7

#include <stdbool.h>

static int TGCurrentColorID = 1; // 0 is reserved in ncurses for default pair. This won't even be use in Windows

typedef struct {
    int id;
    short foreground, background;
    bool foregroundBright, backgroundBright;
} TGColor;

typedef struct {
    bool underlined;
    bool bold;
    unsigned short Attributes;
    int color;
} TGAttributes;

#ifdef _WIN32
#include <Windows.h>
#define TG_WINDOWS_MODE true

#else
#include <uchar.h>
#define TG_WINDOWS_MODE false

// I will keep the COORD and CHAR_INFO structures from Windows so that I don't abstract away EVERYTHING
typedef struct {
    int X, Y;
} COORD;

typedef struct {
    union {
        char16_t UnicodeChar;
        char AsciiChar;
    } Char;
    TGAttributes Attrs;
    unsigned short Attributes;
} CHAR_INFO;

#include <ncursesw/curses.h>
typedef WINDOW* HANDLE; // Let's make this easy

#endif // Not Win32

// A drawing buffer, for general purposes
typedef struct {
	COORD size;
	int length;
	CHAR_INFO *buffer;
} TGBuffer;

typedef struct {
    int key;
    bool ctrlDown;
} TGKeyEvent;

#define TG_MOUSE_LEFT 1
#define TG_MMB 3
#define TG_MOUSE_RIGHT 2

typedef struct {
    COORD position;
    short button;
    short state;
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
	TGInput inputBuffer;
} TGContext;

TGBuffer TGBufCreate(int, int); // Function to allocate a drawing buffer
TGBuffer TGBufCopy(TGBuffer*); // Deep copy one buffer to another
void TGBufSize(TGBuffer*, int, int); // Resize a draw buffer
void TGBufClear(TGBuffer*); // Fill a buffer with blank cells
void TGBufCell(TGBuffer*, int, int, CHAR_INFO); // Draw to a single cell on the buffer
void TGBufAttr(TGBuffer*, int, int, TGAttributes);
void TGBufFree(TGBuffer*); // Function to de-allocate a drawing buffer
void TGCalculateAttrs(TGAttributes*);

TGContext* TG(); // Initialization function, which returns a drawing context to the screen
void TGUpdate(); // Displays what has been drawn
void TGSetCursorVisible(bool);
TGInput TGGetInput();
void TGHandleResizeEvent(TGInput); // Resize is not handled automatically
int TGTitle(const char*);
void TGEnd();

int TGColorID();
TGColor TGColorCreate(int, int);

static TGContext TGMainContext = {0};
static long TGPreviousInputMode;

#endif // TG_H