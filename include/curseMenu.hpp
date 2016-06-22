#ifndef CURSEMENU_HPP
#define CURSEMENU_HPP

#include <iostream>
#include <unistd.h>
#include <ncurses.h>
#include <string>
#include <vector>
#include <functional>
#include <cmath>

// curses flags
#define CURSES_RAW 	  1
#define CURSES_CBREAK 2
#define CURSES_NOECHO 4
#define CURSES_KEYPAD 8

// window flags
#define WINDOW_SHADOW 1

// window title flags
#define WINDOW_TITLE_CENTERED 1

class vec2{
public:
	int x;
	int y;
	vec2()
	{
		x=y=0;
	}
	vec2(int _y, int _x)
	{
		y = _y;
		x = _x;
	}
};

namespace curseMenu {

	namespace dataEntry {

		template<class T>
		T textBox(std::string caption);
		template<class T>
		T textBox(std::string caption, std::string allowed_chars);
	}

	void initCurses(uint32_t flags);
	void endCurses();

	// refresh the main screen	
	void globalUpdate();

	// set the background color of the screen
	void setBackground(uint c);

	// ui elements that are placed on the windows
	class curseElement{
	private:
		vec2 loc;

		struct{
			std::string text;
			vec2 loc;
		}title;
	public:
		
		virtual void setTitle(std::string str);
		virtual void setTitle(int y, int x, std::string str);
		virtual void setPos(int _y, int _x);

		int getY();
		int getX();

		std::string getIdentifier();
		int getIdentifierY();
		int getIdentifierX();
		
		virtual void trigger(){};
	};

	// a button element
	class curseButton : public curseElement {
	private:
		// the action(s) called when the button is used
		std::function<void()> func;
	public:
		// this allows us to clone a button to place it in the window class
		curseButton* clone();

		// this will call the events trigger function, this must be done since event is private
		void trigger();
		void onTrigger(std::function<void()> t);
	};

	// windows that are placed around the terminal
	class curseWindow : public curseElement{
	private:
		int width;
		int height;

		vec2 cursor;
		vec2 selection;

		uint textColor;
		uint backColor;
		uint highlightColor;
		uint pairIndex;
		uint effectIndex;

		WINDOW *shadow;
		WINDOW *window;

		std::vector<curseElement*> elements;
		struct windowString{
			vec2 loc;
			std::string text;
			uint fg;
			uint bg;
			windowString(vec2 l, std::string t, uint f, uint b)
			{
				loc = l;
				text = t;
				fg = f;
				bg = b;
			}
		};
		std::vector<windowString> text;
	public:
		curseWindow();
		~curseWindow();
		void destroy();
		

		// create a new window inside the terminal
		void createWindow(int y, int x, int w, int h, uint32_t flags);
		void setTitle(std::string str, uint flags);
		void update();

		// various aesthetic functions
		void setBackground(uint c);
		void setTextColor(uint c);
		void setTextColor(uint f, uint b);
		void setHighlight(uint c);
		void resetColors();

		// move the cursor to the specified point
		void moveCursor(int y, int x);
		// move the cursor down one
		void newLine();
		// print text at the cursor
		void putText(std::string s);
		void putTextHighlight(std::string s);

		// print text at the given coordinates
		void putText(int y, int x, std::string s);
		void putTextHighlight(int y, int x, std::string s);
		
		// create a normal button
		void createButton(curseButton &button, int y, int x, std::string s);	

		// move the selection in the given direction
		void movSelRight();
		void movSelLeft();
		void movSelUp();
		void movSelDown();
		
		// returns the last key pressed on the keyboard in the current window
		char getKey();

		// checks for a button under the pointers position, and if there is, it will trigger the button...
		void trigger();
	};
}

#endif //CURSEMENU_HPP
