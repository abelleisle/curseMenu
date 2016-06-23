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

/**
 * The class used to store the y and x coordinates in a single
 * object for curseElements.
 */
class vec2{
public:
	/**
 	 *	The actual x and y coordinates are stored here
 	 */
	int x;
	int y;

	/**
 	 * @brief The default constructor
 	 * This just sets the x and y variables to 0
 	 */
	vec2()
	{
		x=y=0;
	}

	/**
 	 * @brief The assignment constructor
 	 * @param _y The value that will be passed to y
 	 * @param _X The value that will be passed to x
 	 */
	vec2(int _y, int _x)
	{
		y = _y;
		x = _x;
	}
};

/**
 * @brief The namespace containing the curseMenu library
 * This namespace holds all of the APIs contents and classes
 */
namespace curseMenu {

	/**
 	 *	@brief The namespace holding functions used for data entry
 	 *	These functions are not part of a class and can be accessed by
 	 *	any file that links the header.
 	 */
	namespace dataEntry {

		/**
 		 * @brief Spawns a window that allows the entry of data
 		 * This function spawns a small window that allows the user to enter
 		 * a string of data that is converted to a variety of types
 		 *
 		 * @param caption The text displayed on the new window
 		 */
		template<class T>
		T textBox(std::string caption);
		
		/**
 		 * @brief A textbox with custom tokens
 		 * @see textBox()
 		 * @param allowed_chars The chars that are allowed in the output string
 		 */
		template<class T>
		T textBox(std::string caption, std::string allowed_chars);
	}
	
	/**
 	 * @brief Initializes the curses library
 	 * Initializes the curses library and enables a variety of modes
 	 * based off of the flags passed
 	 * @param flags A series of unsigned ints that are ORed together
 	 * @see curseMenu.hpp
 	 */
	void initCurses(uint32_t flags);
	
	/**
 	 * @brief Ends the curses library
 	 * Does a bit of garbage collection and stop the curses
 	 * terminal mode to return to the 'stock' mode
 	 * Call this right before 'return' in the 'main' function.
 	 */
	void endCurses();

	/**
 	 * @brief Refreshes the background
 	 * This refreshes the background which basically draws
 	 * the back color and the text we requested to draw
 	 */	
	void globalUpdate();

	/**
 	 * @brief Sets the background color of the screen
 	 * This sets the color pair '1' to whatever color we send here
 	 */
	void setBackground(uint c);

	/**
 	 * @brief The base class for all things draw on the screen
 	 * This stores the location and the title of the element
 	 */
	class curseElement{
	private:
		/**
 		 * @brief The location of the element
 		 */
		vec2 loc;

		/**
 		 * @brief Stores the title and it's location
 		 */
		struct titleStruct{
			/// The title itself
			std::string text;
			/// The location of the title
			vec2 loc;
		}title;
	public:
		/**
 		 * @brief Sets the title
 		 * This sets the title itself, and sets the location
 		 * of the title to 0,0
 		 * @see titleStruct
 		 */
		virtual void setTitle(std::string str);
		
		/**
 		 * @brief Sets the title and its location
 		 * @see setTitle()
 		 * 
 		 * @param y The row the title is placed on
 		 * @param x The column the title is placed on
 		 */
		virtual void setTitle(int y, int x, std::string str);
		
		/**
 		 * @brief Sets the location of the curseElement
 		 *
 		 * @param _y The row we want to move the element to
 		 * @param _x The column we want the element moved to
 		 */
		virtual void setPos(int _y, int _x);

		/**
 		 * @brief Returns the row of the element
 		 * @return loc.y
		 */
		int getY();
		
		/**
 		 * @brief Returns the column of the element
 		 * @return loc.x
		 */
		int getX();

		/**
 		 * @brief Returns the title
 		 * @return title.text
 		 */
		std::string getIdentifier();
		
		/**
 		 * @brief Returns the row the column starts on
 		 * @return title.loc.y
 		 */
		int getIdentifierY();
		
		/**
 		 * @brief Returns the column the title starts on
 		 * @return title.loc.x
 		 */
		int getIdentifierX();
		
		/**
 		 * @brief Function called when element is interacted with
 		 * This is mostly used for buttonElements and future interactable
 		 * objects
 		 * 
 		 * @see curseButton
 		 */
		virtual void trigger(){};
	};

	/**
 	 * @brief A button placed on the screen
 	 * A child of curseElement that can be selected and "clicked"
 	 * to call a defined function
 	 * @see func
 	 * @see curseElement
 	 */
	class curseButton : public curseElement {
	private:
		/// the action(s) called when the button is used
		std::function<void()> func;
	public:
		/**
 		 * @brief Clones the button for polymorphism
 		 * @return (*this)
 		 */
		curseButton* clone();

		/**
 		 * @brief Calls func
 		 * When moused over, and the defined key is pressed
 		 * this function will be called
 		 * @see curseElement::trigger()
 		 */
		void trigger();
		
		/**
 		 * @brief Sets the function called on trigger()
 		 * A void function with no argument is passed, this function
 		 * may be a lambda
 		 * 
 		 * @param t The function we want to use for trigger()
 		 * @see func
 		 */
		void onTrigger(std::function<void()> t);
	};

	/**
 	 * @brief Windows placed around the screen
 	 * A window is a sub terminal in the main curses mode,
 	 * we use a window to easily trap input or organize outputs
 	 *
 	 * @see curseElement
 	 */
	class curseWindow : public curseElement{
	private:
		/// How many columns the window takes up
		int width;
		/// How many rows the window takes up
		int height;

		/**
 		 * @brief The location where text is written.
 		 * The place where is text is written to when putText is called.
 		 */
		vec2 cursor;
		
		/**
 		 * @brief The location that will be "clicked".
 		 * The selection is only placed in locations of a clickable object
 		 * such as buttons. This calls this trigger() function of the object.
 		 * 
 		 * @see curseElement::trigger()
 		 */
		vec2 selection;

		/// The foreground color
		uint textColor;
		
		/// The background color
		uint backColor;

		/// The color of the text highlight
		uint highlightColor;

		/**
 		 * @brief Which color pair the window will use.
 		 * ncurses colors are managed with pairs, which is a pair of a
 		 * foreground and a background. Each pair can only be set once which means
 		 * that we have to keep track of which pair is already used.
 		 */
		uint pairIndex;
		
		/**
 		 * @brief Which color pair the highlight will use.
 		 * @see pairIndex
 		 */
		uint effectIndex;

		/**
 		 * @brief The variable curses stores window data in.
 		 * ncurses stores the window data in a WINDOW pointer.
 		 * This is that WINDOW pointer
 		 */
		WINDOW *shadow;
		
		/**
 		 * @brief The shadows data.
 		 * The shadow under a main window is actually another window
 		 * that is black.
 		 *
 		 * @see shadow
 		 */
		WINDOW *window;

		/**
 		 * @brief The vector of interactable elements.
 		 * Every time we add an element we push back an element to this array.
 		 *
 		 * @see curseButton::clone()
 		 */
		std::vector<curseElement*> elements;
		
		/**
 		 * @brief The text that is printed to the window.
 		 * When putText() is called, the text is stored in a one of these
 		 * structures that can keep track of its location, contents, and colors.
 		 *
 		 * @see putText()
 		 */
		struct windowString{
			/// The location of the string
			vec2 loc;

			/// The contents of the string
			std::string text;

			/// Whether or not the string is "highlighted"
			// TODO make bool
			uint fg;
			uint bg;

			/**
 			 * @brief Assignment constructor.
 			 * This constructor is used to push a structure element
 			 * back in a vector.
 			 *
 			 * @param l The location the string will be in.
 			 * @param t The contents of the string
 			 * @param f The foreground color of the text
 			 * @param b The background color of the text
 			 * @see text
 			 */
			windowString(vec2 l, std::string t, uint f, uint b)
			{
				loc = l;
				text = t;
				fg = f;
				bg = b;
			}
		};

		/**
 		 * @brief The text we will print.
 		 * A vector of windowString structs
 		 *
 		 * @see windowString
 		 */
		std::vector<windowString> text;
	public:
		
		/**
 		 * @brief The constructor of curseWindow.
 		 * This sets default values of the curseWindow class.
 		 */
		curseWindow();
		
		/**
 		 * @brief The destructor of curseWindow.
 		 * This deletes the window variable and empties arrays.
 		 * TODO garbage collect
 		 */
		~curseWindow();
		
		/**
 		 * @brief Removes the window.
 		 * This does no call the destructor, but it deletes the
 		 * window and shows the others below it.
 		 */
		void destroy();
		
		/**
 		 * @brief Puts data in the window variable.
 		 * We pass the location, the size and various flags in order to
 		 * enable certain modes in curses.
 		 *
 		 * @param y The row in which we want to place the window
 		 * @param x The column in which we want to place the window
 		 * @param w The width of the window
 		 * @param h The height of the window
 		 * @param flags A series of uints ORed together to enable window modes
 		 */
		void createWindow(int y, int x, int w, int h, uint32_t flags);
		
		/**
 		 * @brief Set the title of the window.
 		 *
 		 * @param str The contents of the title
 		 * @param flags A series of uints ORed together for certain title modes
 		 */
		void setTitle(std::string str, uint flags);
		
		/**
 		 * @brief Updates the window.
 		 * This draws the text and buttons on the screen.
 		 */
		void update();
		
		/**
 		 * @briefs Clears the window.
 		 * The text, buttons and borders are cleared from the window
 		 * and are not redrawn until we call update() again.
 		 */
		void clean();

		/**
 		 * @brief Returns the window variable.
 		 * NEVER USE THIS.
 		 * This is awful practice...
 		 */
		WINDOW* getWindow();
	
		/**
 		 * @brief Sets the window background color.
 		 * 
 		 * @param c The color of the background.
 		 */
		void setBackground(uint c);
		
		/**
 		 * @brief Sets the window text color.
 		 *
 		 * @param c The color of the text.
 		 */
		void setTextColor(uint c);
		
		/**
 		 * @brief Sets the foreground and background of text.
 		 *
 		 * @param f The color of the text itself.
 		 * @param b The color of the background of the text.
 		 */
		void setTextColor(uint f, uint b);
		
		/**
 		 * @brief Sets the background color of highlighted text.
 		 *
 		 * @param c The color of the highlight
 		 */
		void setHighlight(uint c);
		
		/**
 		 * @brief Resets the color pair to the default values.
 		 * This is called after we change background color for text.
 		 */
		void resetColors();

		/**
 		 * @brief Moves the cursor.
 		 * Moves the cursor to the specified location.
 		 *
 		 * @param y The row we want to move the cursor to.
 		 * @param x The column we want to move the cursor to.
 		 * @see cursor
 		 */
		void moveCursor(int y, int x);
		
		/**
 		 * @brief Moves the cursor down a line.
 		 * @see cursor
 		 */
		void newLine();
		
		/**
 		 * @brief Puts text on the window.
 		 * This function actually pushes text into the text array.
 		 * The location of the string is determined by the location of
 		 * the cursor.
 		 *
 		 * @param s The contents of the text.
 		 * @see cursor
		 */
		void putText(std::string s);
		
		/**
 		 * @brief Places highlighted text on the screen.
 		 * Very similar to putText().
 		 *
 		 * @see putText()
 		 */
		void putTextHighlight(std::string s);

		/**
 		 * @brief Places string at given coordinates.
 		 *
 		 * @param y The row we want the string to go to
 		 * @param x The column we want the string to go to
 		 * @see putText()
 		 */
		void putText(int y, int x, std::string s);
		
		/**
 		 * @brief Places a highlighted string at the given coordinates.
 		 *
 		 * @see putText()
 		 */
		void putTextHighlight(int y, int x, std::string s);
		
		/**
 		 * @brief Puts a button on our window.
 		 * Places a button on the back of the elements vector.
 		 *
 		 * @param button The button object we want to clone
 		 * @param y The row we want the button on
 		 * @param x The column we want the button on
 		 * @param s The text on the button
 		 * @see elements
 		 */
		void createButton(curseButton &button, int y, int x, std::string s);	

		/// Move the selection right
		void movSelRight();

		/// Move the selection left
		void movSelLeft();

		/// Move the selection up
		void movSelUp();

		/// Move the selection down
		void movSelDown();
		
		/**
 		 * @brief Returns the last key pressed.
 		 * Window specific key capturing.
 		 */
		char getKey();

		/**
 		 * @brief Calls the selected elements trigger() function.
 		 * If the element under the selection has a trigger() function it is called.
 		 */
		void trigger();
	};
}

#endif //CURSEMENU_HPP
