#include <curseMenu.hpp>

uint __current_pair__ = 3;

// creation of the curses menu
curseMenu::curseMenu(uint32_t flags)
{	
	// start curses
	initscr();

	if (has_colors() == FALSE) {
		printw("Terminal does not support colors!");
	} else {
		start_color();
		// shadows will always be color pair 2
		init_pair(2, COLOR_BLACK, COLOR_BLACK);
	}

	// change modes of curses based off of the given flags
	if (flags & 1)
		raw();
	if (flags & 2)
		cbreak();
	if (flags & 4)
		noecho();
	if (flags & 8)
		keypad(stdscr, true);

	//bkgd(COLOR_PAIR(1));	
}

// destruction of the menu
curseMenu::~curseMenu()
{
	// end the curses screen
	endwin();
}

// refresh the window
void curseMenu::update()
{
	refresh();
}

// set the background color
void curseMenu::setBackground(uint color)
{
	// color pair 1 will always be the backdrop
	init_pair(1, color, color);
	bkgd(COLOR_PAIR(1));
}

void curseElement::setTitle(std::string str)
{
	title = str;
}

void curseElement::setPos(int _y, int _x)
{
	y = _y;
	x = _x;
}

int curseElement::getY()
{
	return y;
}

int curseElement::getX()
{
	return x;
}

std::string curseElement::getIdentifier()
{
	return title;
}

curseButton* curseButton::clone()
{
	return new curseButton(*this);
}

// basic constructor for the window class
curseWindow::curseWindow()
{
	window = nullptr;
	shadow = nullptr;

	// define the two color index locations we can use for our window
	pairIndex = __current_pair__;
	effectIndex = pairIndex + 1;
	__current_pair__ += 2;

	selection = vec2(0,0);

	// set default values for color
	textColor = COLOR_BLACK;
	backColor = COLOR_WHITE;

	// enable these default options
	init_pair(pairIndex, textColor, backColor);
	init_pair(effectIndex, backColor, textColor);
}

// creates a new window in our class
void curseWindow::createWindow(int x, int y, int w, int h, uint32_t flags)
{
	width = w;
	height = h;
	window = newwin(h, w, y, x);

	if (flags & 1) {
		shadow = newwin(h, w, y+1, x+2);
		wbkgd(shadow, COLOR_PAIR(2));
	}
	if (flags & 8) {
		keypad(window, TRUE);
	}
	
	wbkgd(window, COLOR_PAIR(pairIndex));
	box(window, 0, 0);
}

// set the window title
void curseWindow::setTitle(std::string str, uint flags)
{
	curseElement::setTitle(str);
	double x = 2;

	// if the title will be centered
	if (flags & 1)
		x = (width / 2.0f) - (str.length() / 2.0f);

	mvwprintw(window, 0, x, str.c_str());
}

// refresh the window
void curseWindow::update()
{
	if (shadow)
		wrefresh(shadow);
	int eIndexY = 0;	
	for (auto &e : elements) {
		if (selection.y == eIndexY)
			putText(e->getY(), e->getX(), e->getIdentifier(), COLOR_WHITE, COLOR_RED);
		else
			putText(e->getY(), e->getX(), e->getIdentifier());
		eIndexY++;
	}
	
	wrefresh(window);
}

// set the background color of the window
void curseWindow::setBackground(uint color)
{
	backColor = color;
	init_pair(pairIndex, textColor, backColor);
}

// set the text color of the window
void curseWindow::setTextColor(uint color)
{
	textColor = color;
	init_pair(pairIndex, textColor, backColor);
}

// set the text color including the background
void curseWindow::setTextColor(uint fg, uint bg)
{
	init_pair(pairIndex, fg, bg);
}

// reset the colors back to the predetermined values
void curseWindow::resetColors()
{
	init_pair(pairIndex, textColor, backColor);
}

void curseWindow::moveCursor(int y, int x)
{
	wmove(window, y, x);
	cursor = vec2(y, x);
}

void curseWindow::newLine()
{
	moveCursor(cursor.y + 1, cursor.x);	
}

void curseWindow::putText(std::string str)
{
	wprintw(window, str.c_str());
}

void curseWindow::putText(std::string str, uint f, uint b)
{
	init_pair(effectIndex, f, b);
	wattron(window, COLOR_PAIR(effectIndex));
	wprintw(window, str.c_str());
	wattroff(window, COLOR_PAIR(effectIndex));
}

void curseWindow::putText(int y, int x, std::string str)
{
	moveCursor(y, x);
	putText(str);
}

void curseWindow::putText(int y, int x, std::string str, uint f, uint b)
{
	moveCursor(y, x);
	putText(str, f, b);
}

void curseWindow::createButton(curseButton &button, int y, int x, std::string s)
{
	button.setTitle(s);
	button.setPos(y, x);	
	
	elements.push_back(button.clone());
}

void curseWindow::movSelRight()
{
	selection.x++;
}

void curseWindow::movSelLeft()
{
	selection.x--;
}

void curseWindow::movSelUp()
{
	selection.y--;
}

void curseWindow::movSelDown()
{
	selection.y++;
}

char curseWindow::getKey()
{
	return wgetch(window);
}
