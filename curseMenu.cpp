#include <curseMenu.hpp>
namespace curseMenu {
	namespace dataEntry {
		template<class T>
		T textBox(std::string caption)
		{
			(void)caption;
			T meme;
			return meme;		
		}
		
		template<> int textBox<int>(std::string caption)
		{
			curseWindow box;
   			box.createWindow(15, 15, 40, 10,  WINDOW_SHADOW | CURSES_KEYPAD);
			box.setTitle(caption, WINDOW_TITLE_CENTERED);
			box.setBackground(COLOR_GREEN);
			box.setTextColor(COLOR_BLACK);
			
			box.update();
			getch();
			box.destroy();
			
			return 32;
		}
		
		template<class T> 
		T textBox(std::string caption, std::string allowed_chars)
		{
			(void)caption;
			(void)allowed_chars;
			T meme;
			return meme;
		}
	}
	uint __current_pair__ = 3;

	// creation of the curses menu
	void initCurses(uint32_t flags)
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
		if (flags & 1) {
			raw();
		}
		if (flags & 2) {
			cbreak();
		}
		if (flags & 4) {
			noecho();
			curs_set(0);
		}
		if (flags & 8) {
			keypad(stdscr, true);
		}
		//bkgd(COLOR_PAIR(1));	
	}

	void endCurses()
	{
		endwin();
	}

	// refresh the window
	void globalUpdate()
	{
		refresh();
	}

	// set the background color
	void setBackground(uint color)
	{
		// color pair 1 will always be the backdrop
		init_pair(1, color, color);
		bkgd(COLOR_PAIR(1));
	}

	void curseElement::setTitle(std::string str)
	{
		title.loc = vec2(0, 2);
		title.text = str;
	}

	void curseElement::setTitle(int y, int x, std::string str)
	{
		title.loc = vec2(y, x);
		title.text = str;
	}

	void curseElement::setPos(int _y, int _x)
	{
		loc.y = _y;
		loc.x = _x;
	}

	int curseElement::getY()
	{
		return loc.y;
	}

	int curseElement::getX()
	{
		return loc.x;
	}

	std::string curseElement::getIdentifier()
	{
		return title.text;
	}

	int curseElement::getIdentifierY()
	{
		return title.loc.y;
	}

	int curseElement::getIdentifierX()
	{
		return title.loc.x;
	}

	curseButton* curseButton::clone()
	{
		return new curseButton(*this);
	}

	void curseButton::trigger()
	{
		try {
			func();
		} catch (std::bad_function_call) {
			// this is just caught to avoid a crash
			// nothing actually needs to be done	
		}
	}

	void curseButton::onTrigger(std::function<void()> t)
	{
		func = t;
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
		highlightColor = textColor = COLOR_BLACK;
		backColor = COLOR_WHITE;

		// enable these default options
		// TODO
		// init_pair(pairIndex, textColor, backColor);
		// init_pair(effectIndex, backColor, textColor);
	}

	curseWindow::~curseWindow()
	{
	}

	void curseWindow::destroy()
	{
		if (shadow) {
			wborder(shadow, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
			wrefresh(shadow);
			delwin(shadow);
		}
		
		wborder(window, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
		wrefresh(window);
		delwin(window);
	}

	// creates a new window in our class
	void curseWindow::createWindow(int y, int x, int w, int h, uint32_t flags)
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
		double x = 2;

		// if the title will be centered
		if (flags & 1)
			x = (width / 2.0f) - (str.length() / 2.0f);

		curseElement::setTitle(0, floor(x), str);
	}

	// refresh the window
	void curseWindow::update()
	{
		init_pair(pairIndex, textColor, backColor);
		init_pair(effectIndex, backColor, highlightColor);
		
		if (shadow)
			wrefresh(shadow);

		//wbkgd(window, COLOR_PAIR(pairIndex));
		box(window, 0, 0);

		mvwprintw(window, getIdentifierY(), getIdentifierX(), getIdentifier().c_str());

		int eIndexY = 0;	
		for (auto &e : elements) {
			if (selection.y == eIndexY) {
				wattron(window, COLOR_PAIR(effectIndex));	
				mvwprintw(window, e->getY(), e->getX(), e->getIdentifier().c_str());
				wattroff(window, COLOR_PAIR(effectIndex));
				resetColors();
			} else {
				mvwprintw(window, e->getY(), e->getX(), e->getIdentifier().c_str());
			}
			eIndexY++;
			
			wrefresh(window);
		}

		for (auto &t : text) {
			if (t.fg != textColor || t.bg != backColor) {
				wattron(window, COLOR_PAIR(effectIndex));
				mvwprintw(window, t.loc.y, t.loc.x, t.text.c_str());
				wattroff(window, COLOR_PAIR(effectIndex));
			} else {
				mvwprintw(window, t.loc.y, t.loc.x, t.text.c_str());
			}
			wrefresh(window);
		}
		wrefresh(window);	
	}

	// TODO make this check for x location
	void curseWindow::trigger()
	{
		int eIndexY = 0;
		for (auto &e : elements) {
			if (selection.y == eIndexY) {
				e->trigger();
			}
			eIndexY++;
		}
	}

	// set the background color of the window
	void curseWindow::setBackground(uint color)
	{
		backColor = color;
	}

	// set the text color of the window
	void curseWindow::setTextColor(uint color)
	{
		textColor = color;
	}

	// set the text color including the background
	// TODO
	void curseWindow::setTextColor(uint fg, uint bg)
	{
		(void)fg;
		(void)bg;
	}

	void curseWindow::setHighlight(uint c)
	{
		highlightColor = c;
	}

	// reset the colors back to the predetermined values
	// TODO
	void curseWindow::resetColors()
	{
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
		text.push_back(windowString(cursor, str, textColor, backColor));
	}

	void curseWindow::putTextHighlight(std::string str)
	{
		text.push_back(windowString(cursor, str, backColor, highlightColor));
	}

	void curseWindow::putText(int y, int x, std::string str)
	{
		moveCursor(y, x);
		putText(str);
	}

	void curseWindow::putTextHighlight(int y, int x, std::string str)
	{
		moveCursor(y, x);
		putTextHighlight(str);
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
}
