#include "tg.h"
#include <string.h>
#include <stdlib.h>

TGBuffer TGBufCreate(int width, int height) {
	TGBuffer TGBuffer;
	TGBuffer.buffer = NULL; // Init the buffer to NULL
	#ifdef _WIN32
	TGBuffer.windowsDrawBuffer = NULL;
	#endif
	TGBufSize(&TGBuffer, width, height);
	return TGBuffer;
}

TGBuffer TGBufCopy(TGBuffer* original) {
	TGBuffer TGBuffer = TGBufCreate(original->size.X, original->size.Y);
	TGBuffer.length = original->length;
	memcpy(TGBuffer.buffer, original->buffer, sizeof(TGCharInfo) * TGBuffer.length);
	return TGBuffer;
}

void TGBufSize(TGBuffer* drawBuffer, int width, int height) {
	// Using free/ malloc here because we aren't interested in retaining data
	if (drawBuffer->buffer) {
		free(drawBuffer->buffer);
		#ifdef _WIN32
		free(drawBuffer->windowsDrawBuffer);
		#endif
	}
	drawBuffer->length = width * height;
	drawBuffer->buffer = malloc(sizeof(TGCharInfo) * drawBuffer->length);
	#ifdef _WIN32
	drawBuffer->windowsDrawBuffer = malloc(sizeof(CHAR_INFO) * drawBuffer->length);
	#endif
	// Copy the size to the buffer record
	drawBuffer->size.X = width;
	drawBuffer->size.Y = height;
	TGBufClear(drawBuffer);
}

void TGBufClear(TGBuffer *tgBuffer) {
	int i = 0, limit = tgBuffer->size.X * tgBuffer->size.Y;
	// Create a blank CHAR_INFO
	TGCharInfo clearChar;
	clearChar.AsciiChar = ' ';
	clearChar.UnicodeChar = ' ';
	TGAttributes attrs = { .color = 0 };
	TGCalculateAttrs(&attrs);
	clearChar.attributes = attrs;
	#ifdef _WIN32
	CHAR_INFO winClearChar = { 0 };
	#endif
	// Set everything to that buffer
	while (i < limit) {
		tgBuffer->buffer[i] = clearChar;
		#ifdef _WIN32
		tgBuffer->windowsDrawBuffer[i] = winClearChar;
		#endif
		i++;
	}
}

void TGBufCell(TGBuffer *tgBuffer, int x, int y, TGCharInfo character) {
	if (x >= tgBuffer->size.X || y >= tgBuffer->size.Y || y < 0 || x < 0) return;
	tgBuffer->buffer[(tgBuffer->size.X * y) + x] = character;
	#ifdef _WIN32
	tgBuffer->windowsDrawBuffer[(tgBuffer->size.X * y) + x].Char.AsciiChar = character.AsciiChar;
	tgBuffer->windowsDrawBuffer[(tgBuffer->size.X * y) + x].Char.UnicodeChar = character.UnicodeChar;
	tgBuffer->windowsDrawBuffer[(tgBuffer->size.X * y) + x].Attributes = character.attributes.attributes;
	#endif
}

void TGBufAttr(TGBuffer *tgBuffer, int x, int y, TGAttributes attr) {
	if (x > tgBuffer->size.X || y > tgBuffer->size.Y || y < 0 || x < 0) return;
	TGCalculateAttrs(&attr);
	tgBuffer->buffer[(tgBuffer->size.X * y) + x].attributes = attr;
	#ifdef _WIN32
	tgBuffer->windowsDrawBuffer[(tgBuffer->size.X * y) + x].Attributes = attr.attributes;
	#endif
}

void TGBufFree(TGBuffer *drawBuffer) {
	#ifdef _WIN32
	free(drawBuffer->windowsDrawBuffer);
	#endif
	free(drawBuffer->buffer);
}

void TGCalculateAttrs(TGAttributes* attrs){
	int a = 0;
    #ifdef _WIN32
	a |= attrs->color;
	if(attrs->underlined) a |= COMMON_LVB_UNDERSCORE;
	#else
	a = COLOR_PAIR(attrs->color);
	if(attrs->bold) a |= A_BOLD;
	if(attrs->underlined) a |= A_UNDERLINE;
	#endif
	attrs->attributes = a;
}

TGContext* TG() {
	int width, height;
    #ifdef _WIN32
	TGMainContext.screenBufferHandle = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(TGMainContext.screenBufferHandle, &info);
	width = info.dwSize.X;
	height = info.dwSize.Y;
	// Hook up the input handle
	TGMainContext.inputHandle = GetStdHandle(STD_INPUT_HANDLE);
	// Use this buffer
	SetConsoleActiveScreenBuffer(TGMainContext.screenBufferHandle);
	// Update the buffer sizes
	GetConsoleScreenBufferInfo(TGMainContext.screenBufferHandle, &info);
	TGBufSize(&TGMainContext.drawBuffer, info.dwSize.X, info.dwSize.Y);
	TGBufClear(&TGMainContext.drawBuffer);
	// Set up a better input mode
	GetConsoleMode(TGMainContext.inputHandle, &TGPreviousInputMode);
	SetConsoleMode(TGMainContext.inputHandle, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT);
    #else
    TGMainContext.screenBufferHandle = initscr(); // Linux is beautiful
	start_color();
	nodelay(TGMainContext.screenBufferHandle, true);
	mouseinterval(0);
	keypad(stdscr, true);
	use_default_colors();
	cbreak();
	noecho();
	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, &TGPreviousInputMode);
	getmaxyx(TGMainContext.screenBufferHandle, height, width);
    #endif
	TGMainContext.drawBuffer = TGBufCreate(width, height);
	return &TGMainContext;
}

void TGUpdate() {
    #ifdef _WIN32
	COORD size = { TGMainContext.drawBuffer.size.X, TGMainContext.drawBuffer.size.Y }; // Buffer size
	COORD pos = { 0, 0 }; // Start of the buffer coord
	SMALL_RECT rect = {
		.Left = 0,
		.Top = 0,
		.Right = size.X - 1,
		.Bottom = size.Y - 1
	}; // Rect to draw to on destination
	WriteConsoleOutput(
		TGMainContext.screenBufferHandle,
		TGMainContext.drawBuffer.windowsDrawBuffer,
		size,
		pos,
		&rect
	);
    #else
	int i = 0;
	while(i < TGMainContext.drawBuffer.size.X * TGMainContext.drawBuffer.size.Y){
		int x = i % TGMainContext.drawBuffer.size.X;
		int y = (i - x) / TGMainContext.drawBuffer.size.X;
		attron(TGMainContext.drawBuffer.buffer[i].attributes.attributes);
		mvwaddch(TGMainContext.screenBufferHandle, y, x, TGMainContext.drawBuffer.buffer[i].UnicodeChar);
		attroff(TGMainContext.drawBuffer.buffer[i].attributes.attributes);
		i++;
	}
    wrefresh(TGMainContext.screenBufferHandle);
    #endif
}

void TGSetCursorVisible(bool visible) {
    #ifdef _WIN32
	// Copy the already-available cursor info
	CONSOLE_CURSOR_INFO info;
	GetConsoleCursorInfo(TGMainContext.screenBufferHandle, &info);
	// Modify the cursor visibility
	info.bVisible = visible;
	SetConsoleCursorInfo(TGMainContext.screenBufferHandle, &info);
    #else
	curs_set(visible);
    #endif
}

// You should be able to use a TGContext's input buffer rather than creating your own
// for maximum memory conservation
TGInput TGGetInput() {
	TGInput input = { 0 };
    #ifdef _WIN32
	int numberRead;
	INPUT_RECORD inputRecord;
	ReadConsoleInput(
		TGMainContext.inputHandle,
		&inputRecord,
		1,
		&numberRead
	);
	if (inputRecord.EventType == KEY_EVENT) {
		input.eventType = TG_EVENT_KEY;
		input.event.keyEvent.key = inputRecord.Event.KeyEvent.uChar.UnicodeChar;
		input.event.keyEvent.ctrlDown = inputRecord.Event.KeyEvent.dwControlKeyState & RIGHT_CTRL_PRESSED || 
			inputRecord.Event.KeyEvent.dwControlKeyState & LEFT_CTRL_PRESSED;
	}
	else if (inputRecord.EventType == MOUSE_EVENT) {
		input.eventType = TG_EVENT_MOUSE;
		if(inputRecord.Event.MouseEvent.dwEventFlags & MOUSE_MOVED){
			input.event.mouseEvent.action = TG_MOUSE_MOVE;
		}
		else{
			input.event.mouseEvent.action = TG_MOUSE_CLICK;
			switch (inputRecord.Event.MouseEvent.dwButtonState) {
			case FROM_LEFT_1ST_BUTTON_PRESSED:
				input.event.mouseEvent.button = TG_MOUSE_LEFT; break;
			case FROM_LEFT_2ND_BUTTON_PRESSED:
				input.event.mouseEvent.button = TG_MMB; // Might be MMB, might be rightmost. No break.
			case RIGHTMOST_BUTTON_PRESSED:
				input.event.mouseEvent.button = TG_MOUSE_RIGHT; break;
			default:
				input.event.mouseEvent.button = -1;
			}
		}
	}
	else if (inputRecord.EventType == WINDOW_BUFFER_SIZE_EVENT) {
		input.eventType = TG_EVENT_RESIZE;
		input.event.resizeEvent.oldSize = TGMainContext.drawBuffer.size;
		CONSOLE_SCREEN_BUFFER_INFO info;
		GetConsoleScreenBufferInfo(TGMainContext.screenBufferHandle, &info);
		input.event.resizeEvent.newSize = info.dwSize;
		TGHandleResizeEvent(input);
	}
	else {
		input.empty = true; // Sometimes, there are events that need to be ignored
	}
    #else
	int ch = wgetch(TGMainContext.screenBufferHandle);
	if(ch == KEY_RESIZE){
		input.eventType = TG_EVENT_RESIZE;
		input.event.resizeEvent.oldSize = TGMainContext.drawBuffer.size;
		int width, height;
		getmaxyx(TGMainContext.screenBufferHandle, height, width);
		input.event.resizeEvent.newSize.X = width;
		input.event.resizeEvent.newSize.Y = height;
		TGHandleResizeEvent(input);
	}
	else if(ch == KEY_MOUSE){
		input.eventType = TG_EVENT_MOUSE;
		MEVENT ev;
		if(getmouse(&ev) == OK){
			input.event.mouseEvent.position.X = ev.x;
			input.event.mouseEvent.position.Y = ev.y;
			input.event.mouseEvent.action = TG_MOUSE_CLICK;
			if(ev.bstate & BUTTON1_PRESSED) input.event.mouseEvent.button = TG_MOUSE_LEFT;
			else if(ev.bstate & BUTTON2_PRESSED) input.event.mouseEvent.button = TG_MMB;
			else if(ev.bstate & BUTTON3_PRESSED) input.event.mouseEvent.button = TG_MOUSE_RIGHT;
			else if(ev.bstate & REPORT_MOUSE_POSITION) input.event.mouseEvent.action = TG_MOUSE_MOVE;
			else input.event.mouseEvent.button = -1;
		}
		else{
			//input.empty = true;
		}
	}
	else if(ch == ERR){
		input.empty = true;
	}
	else{
		input.eventType = TG_EVENT_KEY;
		input.event.keyEvent.key = ch;
	}
    #endif
	return input;
}

void TGHandleResizeEvent(TGInput input) {
    #ifdef _WIN32
	if (input.eventType == TG_EVENT_RESIZE) {
		TGBufSize(&TGMainContext.drawBuffer, input.event.resizeEvent.newSize.X, input.event.resizeEvent.newSize.X);
		TGBufClear(&TGMainContext.drawBuffer);
		CONSOLE_SCREEN_BUFFER_INFO info;
		GetConsoleScreenBufferInfo(TGMainContext.screenBufferHandle, &info);
		if (info.srWindow.Bottom + 1 < info.dwSize.Y){
			// Size the buffer to the window
			CONSOLE_SCREEN_BUFFER_INFO info;
			GetConsoleScreenBufferInfo(TGMainContext.screenBufferHandle, &info);
			COORD size = {
				info.srWindow.Right + 1,
				info.srWindow.Bottom + 1
			};
			TGBufSize(&TGMainContext.drawBuffer, size.X, size.Y);
			SetConsoleScreenBufferSize(TGMainContext.screenBufferHandle, size);
			TGBufClear(&TGMainContext.drawBuffer);
		}
	}
	#else
		TGBufSize(&TGMainContext.drawBuffer, input.event.resizeEvent.newSize.X, input.event.resizeEvent.newSize.Y);
    #endif
}

int TGTitle(const char *title) {
    #ifdef _WIN32
	return SetConsoleTitle(title);
    #else
	printf("\033]0;%s\007", title);
	fflush(stdout);
    return 1; // Don't know how to check if that worked. Just hope and pray?
    #endif
}

void TGEnd(){
	TGBufFree(&TGMainContext.drawBuffer);
	#ifdef _WIN32
	SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE));
	SetConsoleMode(TGMainContext.inputHandle, TGPreviousInputMode);
	#else
	mousemask(TGPreviousInputMode, NULL);
	endwin();
	#endif
}

int TGColorID(){
	return TGCurrentColorID++;
}

TGColor TGColorCreate(int foreground, int background){
	TGColor color = { .foreground = foreground, .background = background };
	#ifdef _WIN32
	int a = 0;
	if(foreground == TG_RED) a |= FOREGROUND_RED;
	else if(foreground == TG_GREEN) a |= FOREGROUND_GREEN;
	else if(foreground == TG_BLUE) a |= FOREGROUND_BLUE;
	else if(foreground == TG_YELLOW) a |= FOREGROUND_RED | FOREGROUND_GREEN;
	else if(foreground == TG_MAGENTA) a |= FOREGROUND_RED | FOREGROUND_BLUE;
	else if(foreground == TG_CYAN) a |= FOREGROUND_GREEN | FOREGROUND_BLUE;
	else if(foreground == TG_WHITE) a |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	if(background == TG_RED) a |= BACKGROUND_RED;
	else if(background == TG_GREEN) a |= BACKGROUND_GREEN;
	else if(background == TG_BLUE) a |= BACKGROUND_BLUE;
	else if(background == TG_YELLOW) a |= BACKGROUND_RED | BACKGROUND_GREEN;
	else if(background == TG_MAGENTA) a |= BACKGROUND_RED | BACKGROUND_BLUE;
	else if(background == TG_CYAN) a |= BACKGROUND_GREEN | BACKGROUND_BLUE;
	else if(background == TG_WHITE) a |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
	color.id = a;
	#else
	int id = TGColorID();
	color.id = id;
	init_pair(id, foreground, background);
	#endif
	return color;
}