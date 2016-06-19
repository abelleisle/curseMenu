#ifndef CURSEMENU_HPP
#define CURSEMENU_HPP

#include <iostream>
#include <unistd.h>
#include <ncurses.h>
#include <string>
#include <vector>

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

// ui elements that are placed on the windows
class curseElement{
private:
	int x;
	int y;
	
	std::string title;
public:
	
	virtual void setTitle(std::string str);
	virtual void setPos(int _y, int _x);

	int getY();
	int getX();
	std::string getIdentifier();
};

// a button element
class curseButton : public curseElement{
private:
public:
	curseButton* clone();
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
	uint pairIndex;
	uint effectIndex;

	WINDOW *shadow;
	WINDOW *window;

	std::vector<curseElement*> elements;
public:
	curseWindow();

	// create a new window inside the terminal
	void createWindow(int x, int y, int w, int h, uint32_t flags);
	void setTitle(std::string str, uint flags);
	void update();

	// various aesthetic functions
	void setBackground(uint c);
	void setTextColor(uint c);
	void setTextColor(uint f, uint b);
	void resetColors();

	// move the cursor to the specified point
	void moveCursor(int y, int x);
	// move the cursor down one
	void newLine();
	// print text at the cursor
	void putText(std::string s);
	void putText(std::string s, uint f, uint b);

	// print text at the given coordinates
	void putText(int y, int x, std::string s);
	void putText(int y, int x, std::string s, uint f, uint b);
	
	// create a normal button
	void createButton(curseButton &button, int y, int x, std::string s);	

	// move the selection
	void movSelRight();
	void movSelLeft();
	void movSelUp();
	void movSelDown();
	char getKey();
};

// the class for our ncurses menu API
class curseMenu{
private:
public:
	// constructor and destructor
	curseMenu(uint32_t flags);
	~curseMenu();

	// refresh the main screen	
	void update();

	// set the background color of the screen
	void setBackground(uint c);

};

#endif //CURSEMENU_HPP
