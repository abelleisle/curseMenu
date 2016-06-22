#include <iostream>
#include <curseMenu.hpp>

using namespace curseMenu;

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	bool running = true;
	
	initCurses(CURSES_NOECHO | CURSES_CBREAK | CURSES_KEYPAD);	
	setBackground(COLOR_BLUE);

	curseWindow mainWindow;
   	mainWindow.createWindow(10, 10, 80, 20,  WINDOW_SHADOW | CURSES_KEYPAD);
	mainWindow.setTitle("This is a test window title", WINDOW_TITLE_CENTERED);
	mainWindow.setBackground(COLOR_WHITE);
	mainWindow.setTextColor(COLOR_BLACK);
	mainWindow.setHighlight(COLOR_RED);

	mainWindow.putTextHighlight(4, 2, "testing");
	mainWindow.moveCursor(5, 2);
	mainWindow.putText("one");
	mainWindow.newLine();
	mainWindow.putText("two");

	curseButton Option1;
	curseButton Option2;
	curseButton Option3;

	Option1.onTrigger([&]{dataEntry::textBox<int>("Enter integer here");});
	Option2.onTrigger([&running]{running = false;});	
	Option3.onTrigger([&]{mainWindow.putText(7, 2, "Clicked");});
	
	mainWindow.createButton(Option1, 10, 10, "First Option");
	mainWindow.createButton(Option2, 11, 10, "Cacner");
	mainWindow.createButton(Option3, 12, 10, "This is a button");

	globalUpdate();
	mainWindow.update();
		
	int ch = 0;
	while (running) {
		ch = mainWindow.getKey();
		switch (ch) {
		case 115:
			mainWindow.movSelDown();
			break;
		case 119:
			mainWindow.movSelUp();
			break;
		case 10:
			// trigger the button event
			mainWindow.trigger();
			break;
		case 113:
			running = false;
			break;
		default:break;
		}
		
		mainWindow.update();	
	}

	endCurses();

	return 0;
}
