#include <iostream>
#include <curseMenu.hpp>

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	curseMenu skills(CURSES_NOECHO | CURSES_CBREAK | CURSES_KEYPAD);	
	skills.setBackground(COLOR_GREEN);

	curseWindow mainWindow;
   	mainWindow.createWindow(10, 10, 80, 20,  WINDOW_SHADOW | CURSES_KEYPAD);
	mainWindow.setTitle("This is a test window title", WINDOW_TITLE_CENTERED);
	mainWindow.setBackground(COLOR_WHITE);
	mainWindow.setTextColor(COLOR_BLACK);

	mainWindow.putText(4, 2, "fuck me", COLOR_WHITE, COLOR_RED);
	mainWindow.moveCursor(5, 2);
	mainWindow.putText("fuck");
	mainWindow.newLine();
	mainWindow.putText("me");

	curseButton Option1;
	curseButton Option2;
	curseButton Option3;
	mainWindow.createButton(Option1, 10, 10, "First Option");
	mainWindow.createButton(Option2, 11, 10, "Cacner");
	mainWindow.createButton(Option3, 12, 10, "This is a button");

	skills.update();
	mainWindow.update();
		
	int ch = 0;
	bool memes = true;
	while (memes) {
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
			break;
		case 113:
			memes = false;
			break;
		default:break;
		}
		
		mainWindow.update();	
	}

	return 0;
}
