#include "tg.h"
#include <string.h>
#include <stdlib.h>

bool TGRectsEqual(TGRect r1, TGRect r2){
	return TGPointsEqual(r1.start, r2.start) == 0 && TGPointsEqual(r1.size, r2.size) == 0;
}

int TGRectsCmp(TGRect r1, TGRect r2){
	int rect1Area, rect2Area;
	rect1Area = r1.size.X * r1.size.Y;
	rect2Area = r2.size.X * r2.size.Y;
	if(rect1Area == rect2Area) return 0;
	else if(rect1Area < rect2Area) return -1;
	else return 1;
}


// NOTE: (0, 1) and (1, 0) technically equal. Use TGPointsEqual for strict comparison
int TGPointCmp(TGPoint c1, TGPoint c2){
	int difference = (c1.X + c1.Y) - (c2.X + c2.Y);
	if(difference == 0) return 0; // Same distance from (-inf, -inf)
	else if(difference < 0) return -1; // Point1 "smaller" (closer to (-inf, -inf))
	else return 1; // Point2 "smaller"
}

// Strict comparison function, see note for TGPointCmp
bool TGPointsEqual(TGPoint c1, TGPoint c2){
	return c1.X == c2.X && c1.Y == c2.Y;
}

TGBuffer TGBufCreate(int width, int height) {
	TGBuffer TGBuffer;
	TGBuffer.buffer = NULL;
	#ifdef _WIN32
	TGBuffer.systemDrawBuffer = NULL;
	#else
	TGBuffer.systemDrawBuffer = newpad(height, width);
	#endif
	TGBuffer.virtualCursorPosition = (TGPoint) { 0, 0 };
	TGBuffer.currentAttributes = (TGAttributes) { .color = TGDefaultColor.id };
	TGBufSize(&TGBuffer, width, height);
	return TGBuffer;
}

/*TGBuffer TGBufDuplicate(TGBuffer* original) {
	TGBuffer TGBuffer = TGBufCreate(original->size.X, original->size.Y);
	TGBuffer.length = original->length;
	memcpy(TGBuffer.buffer, original->buffer, sizeof(TGCharInfo) * TGBuffer.length);
	return TGBuffer;
}*/

void TGBufSize(TGBuffer* drawBuffer, int width, int height) {
	// Check for stupid resize
	if(drawBuffer->size.X == width && drawBuffer->size.Y == height) return;
	// Using free/ malloc here because we aren't interested in retaining data
	if (drawBuffer->buffer) {
		free(drawBuffer->buffer);
		#ifdef _WIN32
		free(drawBuffer->systemDrawBuffer);
		#else
		wresize(drawBuffer->systemDrawBuffer, height, width);
		#endif
	}
	drawBuffer->length = width * height;
	drawBuffer->buffer = malloc(sizeof(TGCharInfo) * drawBuffer->length);
	#ifdef _WIN32
	drawBuffer->systemDrawBuffer = malloc(sizeof(CHAR_INFO) * drawBuffer->length);
	#endif
	// Copy the size to the buffer record
	drawBuffer->size.X = width;
	drawBuffer->size.Y = height;
}

void TGBufClear(TGBuffer *tgBuffer) {
	int i = 0, limit = tgBuffer->size.X * tgBuffer->size.Y;
	// Create a blank CHAR_INFO
	TGCharInfo clearChar;
	clearChar.character = ' ';
	TGAttributes attrs = { .color = 0 };
	TGCalculateAttrs(&attrs);
	clearChar.attributes = attrs;
	// Set everything to that buffer
	#ifndef _WIN32
	werase(tgBuffer->systemDrawBuffer);
	#endif
	while (i < limit) {
		tgBuffer->buffer[i] = clearChar;
		#ifdef _WIN32
		tgBuffer->systemDrawBuffer[i] = (CHAR_INFO) { .Char.UnicodeChar = ' ' };
		#endif
		i++;
	}
	TGBufCursorPosition(tgBuffer, 0, 0);
}

void TGBufCell(TGBuffer *tgBuffer, int x, int y, TGCharInfo character) {
	if (x >= tgBuffer->size.X || y >= tgBuffer->size.Y || y < 0 || x < 0) return;
	TGCalculateAttrs(&character.attributes);
	tgBuffer->buffer[(tgBuffer->size.X * y) + x] = character;
	#ifdef _WIN32
	tgBuffer->systemDrawBuffer[(tgBuffer->size.X * y) + x].Char.AsciiChar = character.character;
	tgBuffer->systemDrawBuffer[(tgBuffer->size.X * y) + x].Char.UnicodeChar = character.character;
	tgBuffer->systemDrawBuffer[(tgBuffer->size.X * y) + x].Attributes = character.attributes.attributes;
	#else
	wattron(tgBuffer->systemDrawBuffer, character.attributes.attributes);
	mvwaddnwstr(tgBuffer->systemDrawBuffer, y, x, &character.character, 1);
	#endif
}

void TGBufAttr(TGBuffer *tgBuffer, int x, int y, TGAttributes attr) {
	if (x > tgBuffer->size.X || y > tgBuffer->size.Y || y < 0 || x < 0) return;
	TGCalculateAttrs(&attr);
	tgBuffer->buffer[(tgBuffer->size.X * y) + x].attributes = attr;
	#ifdef _WIN32
	tgBuffer->systemDrawBuffer[(tgBuffer->size.X * y) + x].Attributes = attr.attributes;
	#else
	attron(attr.attributes);
	mvwaddnwstr(tgBuffer->systemDrawBuffer, y, x,
		&tgBuffer->buffer[(tgBuffer->size.X * y) + x].character, 1);
	#endif
}

void TGBufFree(TGBuffer *drawBuffer) {
	#ifdef _WIN32
	free(drawBuffer->systemDrawBuffer);
	#else
	if(drawBuffer->systemDrawBuffer != stdscr){
		delwin(drawBuffer->systemDrawBuffer);
	}
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

void TGBufCursorPosition(TGBuffer *buffer, int x, int y){
	buffer->virtualCursorPosition.X = x;
	buffer->virtualCursorPosition.Y = y;
}

void TGBufCursorMove(TGBuffer *buffer, int amount){
	int currentIndex = buffer->virtualCursorPosition.X + (buffer->size.X * buffer->virtualCursorPosition.Y);
	int maxIndex = buffer->size.X * buffer->size.Y;
	currentIndex += amount; // Amount can be negative
	if(currentIndex < 0) currentIndex = 0;
	if(currentIndex > maxIndex) currentIndex = maxIndex;
	buffer->virtualCursorPosition.X = currentIndex % buffer->size.X;
	buffer->virtualCursorPosition.Y = (currentIndex - buffer->virtualCursorPosition.X) / buffer->size.X;
}

void TGBufAddLString(TGBuffer *buffer, char *str){
	int length = strlen(str);
	int currentIndex = buffer->virtualCursorPosition.X + (buffer->size.X * buffer->virtualCursorPosition.Y);
	int indexStart = currentIndex;
	int maxIndex = buffer->size.X * buffer->size.Y;
	while(currentIndex <= maxIndex && length > currentIndex - indexStart){
		TGCharInfo info = { 0 };
		info.attributes = buffer->currentAttributes;
		info.character = str[currentIndex - indexStart];
		TGBufCell(buffer, buffer->virtualCursorPosition.X, buffer->virtualCursorPosition.Y, info);
		TGBufCursorMove(buffer, 1); // Move the cursor "forwards" by one
		currentIndex++;
	}
}

void TGBufAddLStringAttr(TGBuffer *buffer, char *str, TGAttributes attrs){
	int length = strlen(str);
	int currentIndex = buffer->virtualCursorPosition.X + (buffer->size.X * buffer->virtualCursorPosition.Y);
	int indexStart = currentIndex;
	int maxIndex = buffer->size.X * buffer->size.Y;
	while(currentIndex <= maxIndex && length > currentIndex - indexStart){
		TGCharInfo info = { 0 };
		info.attributes = attrs;
		info.character = str[currentIndex - indexStart];
		TGBufCell(buffer, buffer->virtualCursorPosition.X, buffer->virtualCursorPosition.Y, info);
		TGBufCursorMove(buffer, 1); // Move the cursor "forwards" by one
		currentIndex++;
	}
}

void TGBufAddString(TGBuffer *buffer, wchar_t *str){
	int length = wcslen(str);
	int currentIndex = buffer->virtualCursorPosition.X + (buffer->size.X * buffer->virtualCursorPosition.Y);
	int indexStart = currentIndex;
	int maxIndex = buffer->size.X * buffer->size.Y;
	while(currentIndex <= maxIndex && length > currentIndex - indexStart){
		TGCharInfo info = { 0 };
		info.attributes = buffer->currentAttributes;
		info.character = str[currentIndex - indexStart];
		TGBufCell(buffer, buffer->virtualCursorPosition.X, buffer->virtualCursorPosition.Y, info);
		#ifdef _WIN32
		TGBufCursorMove(buffer, 1); // Move the cursor "forwards" by one
		#else
		TGBufCursorMove(buffer, wcwidth(info.character)); // Move the cursor "forwards" by one
		#endif
		currentIndex++;
	}
}

void TGBufAddStringAttr(TGBuffer *buffer, wchar_t *str, TGAttributes attrs){
	int length = wcslen(str);
	int currentIndex = buffer->virtualCursorPosition.X + (buffer->size.X * buffer->virtualCursorPosition.Y);
	int indexStart = currentIndex;
	int maxIndex = buffer->size.X * buffer->size.Y;
	while(currentIndex <= maxIndex && length > currentIndex - indexStart){
		TGCharInfo info = { 0 };
		info.attributes = attrs;
		info.character = str[currentIndex - indexStart];
		TGBufCell(buffer, buffer->virtualCursorPosition.X, buffer->virtualCursorPosition.Y, info);
		#ifdef _WIN32
		TGBufCursorMove(buffer, 1); // Move the cursor "forwards" by one
		#else
		TGBufCursorMove(buffer, wcwidth(info.character)); // Move the cursor "forwards" by one
		#endif
		currentIndex++;
	}
}

// BLIT will copy one **system** buffer to another, but not the TG Buffers,
// making the effect temporary. This is efficient for items that move
// around the screen frequently
void TGBufBlit(TGBuffer *source, TGBuffer *dest, TGPoint location){
	#ifdef _WIN32
	// Copy "rectangle" of source systemDrawBuffer to "rectangle" of destination draw buffer
	int x, y;
	for (y = location.Y; y < dest->size.Y && y < source->size.Y; y++) {
		for (x = location.X; x < dest->size.X && x < source->size.X; x++) {
			dest->systemDrawBuffer[x + (y * dest->size.X)] = source->systemDrawBuffer[x + (y * source->size.X)];
		}
	}
	#else
	copywin(source->systemDrawBuffer, dest->systemDrawBuffer, 0, 0, location.Y, location.X,
		location.Y + (source->size.Y - 1), location.X + (source->size.X - 1), true);
	#endif
}

void TGBufCopy(TGBuffer *source, TGBuffer *dest, TGPoint location){
	
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
	// Set up a better input mode
	GetConsoleMode(TGMainContext.inputHandle, &TGPreviousInputMode);
	SetConsoleMode(TGMainContext.inputHandle, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT);
	// Default Color
	TGDefaultColor.id = info.wAttributes;
	// Extract foreground color
	if ((TGDefaultColor.id & (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)) == (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE))
		TGDefaultColor.foreground = TG_WHITE;
	else if ((TGDefaultColor.id & (FOREGROUND_RED | FOREGROUND_GREEN)) == (FOREGROUND_RED | FOREGROUND_GREEN)) 
		TGDefaultColor.foreground = TG_YELLOW;
	else if ((TGDefaultColor.id & (FOREGROUND_RED | FOREGROUND_BLUE)) == (FOREGROUND_RED | FOREGROUND_BLUE))
		TGDefaultColor.foreground = TG_MAGENTA;
	else if ((TGDefaultColor.id & (FOREGROUND_GREEN | FOREGROUND_BLUE)) == (FOREGROUND_GREEN | FOREGROUND_BLUE))
		TGDefaultColor.foreground = TG_CYAN;
	else if ((TGDefaultColor.id & FOREGROUND_RED) == FOREGROUND_RED)
		TGDefaultColor.foreground = TG_RED;
	else if ((TGDefaultColor.id & FOREGROUND_GREEN) == FOREGROUND_GREEN)
		TGDefaultColor.foreground = TG_GREEN;
	else if ((TGDefaultColor.id & FOREGROUND_BLUE) == FOREGROUND_BLUE)
		TGDefaultColor.foreground = TG_BLUE;
	else TGDefaultColor.foreground = TG_BLACK;
	// Extract background color
	if ((TGDefaultColor.id & (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE)) == (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE))
		TGDefaultColor.background = TG_WHITE;
	else if ((TGDefaultColor.id & (BACKGROUND_RED | BACKGROUND_GREEN)) == (BACKGROUND_RED | BACKGROUND_GREEN))
		TGDefaultColor.background = TG_YELLOW;
	else if ((TGDefaultColor.id & (BACKGROUND_RED | BACKGROUND_BLUE)) == (BACKGROUND_RED | BACKGROUND_BLUE))
		TGDefaultColor.background = TG_MAGENTA;
	else if ((TGDefaultColor.id & (BACKGROUND_GREEN | BACKGROUND_BLUE)) == (BACKGROUND_GREEN | BACKGROUND_BLUE))
		TGDefaultColor.background = TG_CYAN;
	else if ((TGDefaultColor.id & BACKGROUND_RED) == BACKGROUND_RED)
		TGDefaultColor.background = TG_RED;
	else if ((TGDefaultColor.id & BACKGROUND_GREEN) == BACKGROUND_GREEN)
		TGDefaultColor.background = TG_GREEN;
	else if ((TGDefaultColor.id & BACKGROUND_BLUE) == BACKGROUND_BLUE)
		TGDefaultColor.background = TG_BLUE;
	else TGDefaultColor.background = TG_BLACK;
	#else
    TGMainContext.screenBufferHandle = initscr(); // Linux is beautiful
	TGMainContext.inputHandle = newwin(0, 0, 1, 1);
	start_color();
	nodelay(TGMainContext.inputHandle, true);
	mouseinterval(0);
	keypad(stdscr, true);
	use_default_colors();
	cbreak();
	noecho();
	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, &TGPreviousInputMode);
	getmaxyx(TGMainContext.screenBufferHandle, height, width);
	setlocale(LC_ALL, ""); // Turn on UTF8
	pair_content(0, &TGDefaultColor.foreground, &TGDefaultColor.background);
    #endif
	TGMainContext.drawBuffer = TGBufCreate(width, height);
	#ifndef _WIN32
	delwin(TGMainContext.drawBuffer.systemDrawBuffer); // This is wasteful, but only occurs once.
	TGMainContext.drawBuffer.systemDrawBuffer = stdscr;
	#endif
	TGBufClear(&TGMainContext.drawBuffer);
	return &TGMainContext;
}

void TGUpdate() {
    #ifdef _WIN32
	TGPoint size = { TGMainContext.drawBuffer.size.X, TGMainContext.drawBuffer.size.Y }; // Buffer size
	TGPoint pos = { 0, 0 }; // Start of the buffer TGPoint
	SMALL_RECT rect = {
		.Left = 0,
		.Top = 0,
		.Right = size.X - 1,
		.Bottom = size.Y - 1
	}; // Rect to draw to on destination
	WriteConsoleOutput(
		TGMainContext.screenBufferHandle,
		TGMainContext.drawBuffer.systemDrawBuffer,
		size,
		pos,
		&rect
	);
    #else
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

int TGIsSpecialKey(int keycode){
	#ifdef _WIN32
	switch (keycode) {
		case VK_UP: return TG_KEY_UP;
		case VK_RIGHT: return TG_KEY_RIGHT;
		case VK_DOWN: return TG_KEY_DOWN;
		case VK_LEFT: return TG_KEY_LEFT;
		case VK_BACK: return TG_KEY_BACKSPACE;
		case VK_TAB: return TG_KEY_TAB;
		case VK_ESCAPE: return TG_KEY_ESCAPE;
		case VK_PRIOR: return TG_KEY_PAGE_UP;
		case VK_NEXT: return TG_KEY_PAGE_DOWN;
		case VK_END: return TG_KEY_END;
		case VK_HOME: return TG_KEY_HOME;
		case VK_INSERT: return TG_KEY_INSERT;
		case VK_DELETE: return TG_KEY_DELETE;
	}
	#else
	switch(keycode){
		case KEY_UP: return TG_KEY_UP;
		case KEY_RIGHT: return TG_KEY_RIGHT;
		case KEY_DOWN: return TG_KEY_DOWN;
		case KEY_LEFT: return TG_KEY_LEFT;
		case KEY_BACKSPACE: return TG_KEY_BACKSPACE;
		case KEY_STAB: return TG_KEY_TAB;
		case 27: return TG_KEY_ESCAPE;
		case KEY_PPAGE: return TG_KEY_PAGE_UP;
		case KEY_NPAGE: return TG_KEY_PAGE_DOWN;
		case KEY_END: return TG_KEY_END;
		case KEY_HOME: return TG_KEY_HOME;
		case KEY_IC: return TG_KEY_INSERT;
		case KEY_DC: return TG_KEY_DELETE;
	}
	#endif
	return 0;
}

// You should be able to use a TGContext's input buffer rather than creating your own
// for maximum memory conservation
TGInput TGGetInput() {
	TGInput input = { 0 };
    #ifdef _WIN32
	int numEvents;
	GetNumberOfConsoleInputEvents(TGMainContext.inputHandle, &numEvents);
	int numberRead;
	INPUT_RECORD inputRecord;
	if (numEvents == 0) {
		input.empty = true;
		return input;
	}
	ReadConsoleInput(
		TGMainContext.inputHandle,
		&inputRecord,
		1,
		&numberRead
	);
	if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown) {
		input.eventType = TG_EVENT_KEY;
		int specialKey = TGIsSpecialKey(inputRecord.Event.KeyEvent.wVirtualKeyCode);
		if (specialKey) {
			input.event.keyEvent.special = true;
			input.event.keyEvent.key = specialKey;
		}
		else {
			input.event.keyEvent.key = inputRecord.Event.KeyEvent.uChar.UnicodeChar;
		}
		input.event.keyEvent.ctrlDown = inputRecord.Event.KeyEvent.dwControlKeyState & RIGHT_CTRL_PRESSED ||
			inputRecord.Event.KeyEvent.dwControlKeyState & LEFT_CTRL_PRESSED;
	}
	else if (inputRecord.EventType == MOUSE_EVENT) {
		input.eventType = TG_EVENT_MOUSE;
		if (inputRecord.Event.MouseEvent.dwEventFlags & MOUSE_MOVED) {
			input.event.mouseEvent.action = TG_MOUSE_MOVE;
		}
		else {
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
	int ch = wgetch(TGMainContext.inputHandle);
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
		input.last = true;
	}
	else{
		input.eventType = TG_EVENT_KEY;
		int specialKey = TGIsSpecialKey(ch);
		if(specialKey){
			input.event.keyEvent.special = true;
			input.event.keyEvent.key = specialKey;
		}
		else{
			input.event.keyEvent.key = ch;
		}
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
			TGPoint size = {
				info.srWindow.Right + 1,
				info.srWindow.Bottom + 1
			};
			TGBufSize(&TGMainContext.drawBuffer, size.X, size.Y);
			SetConsoleScreenBufferSize(TGMainContext.screenBufferHandle, size);
		}
	}
	#else
		TGBufSize(&TGMainContext.drawBuffer, input.event.resizeEvent.newSize.X, input.event.resizeEvent.newSize.Y);
    #endif
	TGBufClear(&TGMainContext.drawBuffer);
}

void TGTitle(const wchar_t *title) {
    #ifdef _WIN32
	SetConsoleTitle(title);
    #else
	wprintf(L"\033]0;%ls\007", title);
	fflush(stdout);
    #endif
}

void TGLTitle(const char *title) {
    #ifdef _WIN32
	SetConsoleTitleA(title);
    #else
	printf("\033]0;%s\007", title);
	fflush(stdout);
    #endif
}

void TGSetCursorPosition(int x, int y){
	#ifdef _WIN32
	SetConsoleCursorPosition(TGMainContext.screenBufferHandle, (TGPoint){ x, y });
	#else
	move(y, x);
	#endif
}

TGPoint TGGetCursorPosition(){
	TGPoint position = { 0 };
	#ifdef _WIN32

	#else
	getyx(TGMainContext.screenBufferHandle, position.Y, position.X);
	#endif
	return position;
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