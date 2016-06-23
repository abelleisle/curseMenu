#include <curseMenu.hpp>
namespace curseMenu {
	namespace dataEntry {
		
		template<class T>
		T textBox(std::string caption)
		{
			(void) caption;
			T meme;
			return meme;
		}

		template <> std::string textBox<std::string>(std::string caption)
		{
			curseWindow box;
   			box.createWindow(15, 15, 80, 6,  WINDOW_SHADOW | CURSES_KEYPAD);
			box.setBackground(COLOR_GREEN);
			box.setTextColor(COLOR_BLACK);
			
			box.putText(1,2, caption);
			mvwprintw(box.getWindow(), 4, 2, "________________________________________________________________________");
			box.update();

			char buf;
			std::string out;
			while (buf = getch()) {

				if(buf == (char)KEY_BACKSPACE) {
					out.pop_back();
				} else if(buf == '\n') {
					break;
				} else {
					out.push_back(buf);
				}
				mvwprintw(box.getWindow(), 4, 2, "____________________________________");
				mvwprintw(box.getWindow(), 4, 2, out.c_str());
				box.update();
			}
			box.destroy();
			if (!out.size())
				return 0;

			return out;
		}
		
		/// If we want to enter a float
		template<> int textBox<int>(std::string caption)
		{
			curseWindow box;
   			box.createWindow(15, 15, 40, 6,  WINDOW_SHADOW | CURSES_KEYPAD);
			box.setBackground(COLOR_GREEN);
			box.setTextColor(COLOR_BLACK);
			
			box.putText(1,2, caption);
			mvwprintw(box.getWindow(), 4, 2, "____________________________________");
			box.update();

			char buf;
			std::string out;
			while (buf = getch()) {
				if (isdigit(buf)) {
					out += buf;
				} else if(buf == (char)KEY_BACKSPACE) {
					out.pop_back();
				} else if(buf == '\n') {
					break;
				}
				mvwprintw(box.getWindow(), 4, 2, "____________________________________");
				mvwprintw(box.getWindow(), 4, 2, out.c_str());
				box.update();
			}
			box.destroy();
			if (!out.size())
				return 0;

			std::string::size_type sz;	
			return stoi(out, &sz);
		}

		///	If we want to enter a float
		template<> float textBox<float>(std::string caption)
		{
			curseWindow box;
   			box.createWindow(15, 15, 40, 6,  WINDOW_SHADOW | CURSES_KEYPAD);
			box.setBackground(COLOR_GREEN);
			box.setTextColor(COLOR_BLACK);
			
			box.putText(1,2, caption);
			mvwprintw(box.getWindow(), 4, 2, "____________________________________");
			box.update();

			char buf;
			std::string out;

			int numDec = 0;
			while (buf = getch()) {
				if (isdigit(buf)) {
					out += buf;
				} else if (buf == '.' && !numDec) {
					out += buf;
					numDec++;
				} else if (buf == (char)KEY_BACKSPACE) {
					if (out.back() == '.')
						numDec--;
					out.pop_back();
				} else if (buf == '\n') {
					break;
				}
				mvwprintw(box.getWindow(), 4, 2, "____________________________________");
				mvwprintw(box.getWindow(), 4, 2, out.c_str());
				box.update();
			}
			box.destroy();
			if (!out.size())
				return 0;

			std::string::size_type sz;	
			return stof(out, &sz);
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
		bkgd(COLOR_PAIR(1));
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

	WINDOW* curseWindow::getWindow()
	{
		return window;
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
		
		if (shadow) {
			wbkgd(shadow, COLOR_PAIR(2));
			wrefresh(shadow);
		}
		
		//wbkgd(window, COLOR_PAIR(pairIndex));
		box(window, 0, 0);

		mvwprintw(window, getIdentifierY(), getIdentifierX(), getIdentifier().c_str());

		int selAmt = 0;
		for (auto &e : elements) {
			if (selection.y == e->getY() && selection.x == e->getX()) {
				wattron(window, COLOR_PAIR(effectIndex));	
				mvwprintw(window, e->getY(), e->getX(), e->getIdentifier().c_str());
				wattroff(window, COLOR_PAIR(effectIndex));
				resetColors();
				selAmt++;
			} else {
				mvwprintw(window, e->getY(), e->getX(), e->getIdentifier().c_str());
			}
			
			wrefresh(window);
		}
		if (!selAmt && elements.size()) {
			selection.x = elements[0]->getX();
			selection.y = elements[0]->getY();
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
		
				mvwprintw(window, 18, 0, std::to_string(selection.y).c_str());
				mvwprintw(window, 19, 0, std::to_string(selection.x).c_str());
		wrefresh(window);	
	}

	void curseWindow::clean()
	{
		wclear(window);
	}

	// TODO make this check for x location
	void curseWindow::trigger()
	{
		for (auto &e : elements) {
			if (selection.y == e->getY() && selection.x == e->getX()) {
				e->trigger();
			}
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
		// first is index
		// second is disance
		std::pair<int, int> closest = std::make_pair(0, 100);
		int index = 0;
		for (auto &e : elements) {
			if (e->getX() > selection.x) {
				int range = e->getX() - selection.x;
				if (range < closest.second) {
					closest = std::make_pair(index, range);
				}
			}
			index++;
		}
		selection.x = elements[closest.first]->getX();
		selection.y = elements[closest.first]->getY();
	}

	void curseWindow::movSelLeft()
	{
		// first is index
		// second is disance
		std::pair<int, int> closest = std::make_pair(0, 100);
		int index = 0;
		for (auto &e : elements) {
			if (e->getX() < selection.x) {
				int range = selection.x - selection.x;
				if (range < closest.second) {
					closest = std::make_pair(index, range);
				}
			}
			index++;
		}
		selection.x = elements[closest.first]->getX();
		selection.y = elements[closest.first]->getY();
	}

	void curseWindow::movSelUp()
	{
		// first is index
		// second is disance
		std::pair<int, int> closest = std::make_pair(0, 100);
		int index = 0;
		for (auto &e : elements) {
			if (e->getY() < selection.y) {
				int range = selection.y - e->getY();
				if (range < closest.second) {
					closest = std::make_pair(index, range);
				}
			}
			index++;
		}
		selection.y = elements[closest.first]->getY();
		selection.x = elements[closest.first]->getX();
	}

	void curseWindow::movSelDown()
	{
		// first is index
		// second is disance
		std::pair<int, int> closest = std::make_pair(0, 100);
		int index = 0;
		for (auto &e : elements) {
			if (e->getY() > selection.y) {
				int range = e->getY() - selection.y;
				if (range < closest.second) {
					closest = std::make_pair(index, range);
				}
			}
			index++;
		}
		selection.y = elements[closest.first]->getY();
		selection.x = elements[closest.first]->getX();
	}

	char curseWindow::getKey()
	{
		return wgetch(window);
	}
}
