/*
 * Copyright (C) 2011-2012 Me and My Shadow
 *
 * This file is part of Me and My Shadow.
 *
 * Me and My Shadow is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Me and My Shadow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Me and My Shadow.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "Settings.h"
#include "MusicManager.h"
#include "LevelPackManager.h"
#include "Globals.h"

#include <SDL/SDL.h>
#include <string>
#include <vector>

//gettext function
//message: The message to translate.
//Returns: The translated string or the original string if there is not translation available.
#define _(message) (dictionaryManager!=NULL?dictionaryManager->get_dictionary().translate(message).c_str():std::string(message).c_str())
//gettext function
//dictionaryManager: Pointer to the dictionaryManager to use for the translation.
//message: The message to translate.
//Returns: The translated string or the original string if there is not translation available.
#define _C(dictionaryManager, message) ((dictionaryManager)!=NULL?(dictionaryManager)->get_dictionary().translate(message).c_str():std::string(message).c_str())
//dummy function for xgettext
//message: The message to translate.
//Returns: message parameter
#define __(message) (message)

//Loads an image.
//file: The image file to load.
//Returns: The SDL_surface containing the image.
SDL_Surface* loadImage(std::string file);

//Method for drawing an SDL_Surface onto another.
//x: The x location to draw the source on the desination.
//y: The y location to draw the source on the desination.
//source: The SDL_Surface to draw.
//dest: The SDL_Surface to draw on.
//clip: Rectangle which part of the source should be drawn.
void applySurface(int x,int y,SDL_Surface* source,SDL_Surface* dest,SDL_Rect* clip);

//Method used to draw an rectangle.
//x: The top left x location of the rectangle.
//y: The top left y location of the rectangle.
//w: The width of the rectangle,
//h: The height of the rectangle.
//dest: The SDL_Surface to draw on.
//color: The color of the rectangle border to draw.
void drawRect(int x,int y,int w,int h,SDL_Surface* dest,Uint32 color=0);

//Method used to draw filled boxes with an anti-alliased border.
//Mostly used for GUI components.
//x: The top left x location of the box.
//y: The top left y location of the box.
//w: The width of the box,
//h: The height of the box.
//dest: The SDL_Surface to draw on.
//alpha: The alpha of the box to draw.
void drawGUIBox(int x,int y,int w,int h,SDL_Surface* dest,Uint32 color);

//Method used to draw a line.
//x1: The x location of the start point.
//y1: The y location of the start point.
//x2: The x location of the end point.
//y2: The y location of the end point.
//dest: The SDL_Surface to draw on.
//color: The color of the line to draw.
void drawLine(int x1,int y1,int x2,int y2,SDL_Surface* dest,Uint32 color=0);

//Method used to draw a line with some arrows on it.
//x1: The x location of the start point.
//y1: The y location of the start point.
//x2: The x location of the end point.
//y2: The y location of the end point.
//dest: The SDL_Surface to draw on.
//color: The color of the line to draw.
//spacing: The spacing between arrows.
//offset: Offset of first arrow relative to the start point.
//xize, ysize: The size of arrow.
void drawLineWithArrow(int x1,int y1,int x2,int y2,SDL_Surface* dest,Uint32 color=0,int spacing=16,int offset=0,int xsize=5,int ysize=5);

//Method that will load the fonts needed for the game.
//NOTE: It's separate from loadFiles(), since it might get called separatly from the code when changing the language.
bool loadFonts();
//Method that will load the default theme again.
//NOTE: It's separate from loadFiles(), since it might get called separatly from the code when changing resolution.
bool loadTheme();
//This method will attempt to create the screen/window.
//NOTE: It's separate from init(), since it might get called separatly from the code when changing resolution.
bool createScreen();

//Method that is called when a fullscreen window is created.
//It will choose the resolution that is closest to the configured one.
void pickFullscreenResolution();

//This method is used to configure the window that is created by createScreen.
//NOTE: It will do it in a WM specific way, so if the wm is unkown it will do nothing.
//initial: Boolean that is true if it's the first time the window is configured.
void configureWindow(bool initial);

//Call this method when receive SDL_VIDEORESIZE event.
void onVideoResize();

//Initialises the game. This is done almost at the beginning of the program.
//It initialises: SDL, SDL_Mixer, SDL_ttf, the screen and the block types.
//Returns: True if everything goes well.
bool init();
//Loads some important files, like the background music and the default theme.
//Returns: True if everything goes well.
bool loadFiles();

//This method will load the settings from the settings file.
//Returns: False if there's an error while loading.
bool loadSettings();
//This method will save the settings to the settings file.
//Returns: False if there's an error while saving.
bool saveSettings();
//Method used to get a pointer to the settings object.
//Returns: A pointer to the settings object.
Settings* getSettings();

//Method used to get a pointer to the MusicManager object.
//Returns: A pointer to the MusicManager object.
MusicManager* getMusicManager();

//Method used to get a pointer to the LevelPackManager object.
//Returns: A pointer to the LevelPackManager object.
LevelPackManager* getLevelPackManager();

//Method that will, depending on the rendering backend, draw the screen surface to the screen.
void flipScreen();

//Method used to clean up before quiting meandmyshadow.
void clean();

//Sets what the nextState will be.
//newstate: Integer containing the id of the newstate.
void setNextState(int newstate);
//Method that will perform the state change.
//It will fade out and in.
void changeState();

//This method is called when music is stopped.
//NOTE: This method is outside the MusicManager because it couldn't be called otherwise.
//Do not call this method anywhere in the code!
void musicStoppedHook();

//Checks collision between two SDL_Rects.
//a: The first rectangle.
//b: The second rectangle.
//Returns: True if the two rectangles collide.
bool checkCollision(const SDL_Rect& a,const SDL_Rect& b);

//This method will check if the mouse is near a screen edge.
//r: An array of SDL_Rect, does nothing if mouse inside these rectange(s).
//count: Number of rectangles.
//If so it will move the camera.
//Note: This function only works with the leveleditor.
void setCamera(const SDL_Rect* r,int count);

//Parse the commandline arguments.
//argc: Integer containing the number of aruguments there are.
//argv: The arguments.
//Returns: -1 if something goes wrong while parsing,
//          0 if version is shown,
//          1 if everything is alright
int parseArguments(int argc, char** argv);

//From http://en.wikipedia.org/wiki/Clamping_(graphics)
//x: The value to clamp.
//min: The minimum x can be.
//max: The maximum x can be.
//Returns: Integer containing the clamped value.
int inline clamp(int x,int min,int max){
	return (x>max)?max:(x<min)?min:x;
}

//Enumeration containing the different messagebox button combinations.
enum msgBoxButtons{
	//Only one button with the text OK.
	MsgBoxOKOnly=0,
	//Two buttons, one saying OK, the other Cancel.
	MsgBoxOKCancel=1,
	//Three buttons, Abort, Retry, Ignore.
	MsgBoxAbortRetryIgnore=2,
	//Three buttons, Yes, No or Cancel.
	MsgBoxYesNoCancel=3,
	//Two buttons, one saying Yes, the other No.
	MsgBoxYesNo=4,
	//Two buttons, one saying Retry, the other Cancel.
	MsgBoxRetryCancel=5,
};

//Enumeration containing the different result that can be retrieved from a messagebox.
//It represents the button that has been pressed.
enum msgBoxResult{
	//The OK button.
	MsgBoxOK=1,
	//The cancel button.
	MsgBoxCancel=2,
	//The abort button.
	MsgBoxAbort=3,
	//The retry button.
	MsgBoxRetry=4,
	//The ignore button.
	MsgBoxIgnore=5,
	//The yes button.
	MsgBoxYes=6,
	//The no button.
	MsgBoxNo=7,
};

//Method that prompts the user with a notification and/or question.
//prompt: The message the user is prompted with.
//buttons: Which buttons the messagebox should have.
//title: The title of the message box.
//Returns: A msgBoxResult which button has been pressed.
msgBoxResult msgBox(std::string prompt,msgBoxButtons buttons,const std::string& title);

//This method will show a file dialog in which the user can select a file.
//NOTE: It doesn't support entering folders.
//fileName: String that will contain the result, it can also be used to already chose the file.
//title: The title of the fileDialog window.
//extension: The extension the files must have, leave empty for all files.
//path: The path to list the files of.
//isSave: If the dialog is for saving files, and not loading.
//verifyFile: Boolean if the selected should be verified.
//files: Boolean if the fileDialog should display files, if not it will display directories.
bool fileDialog(std::string& fileName,const char* title=NULL,const char* extension=NULL,const char* path=NULL,bool isSave=false,bool verifyFile=false,bool files=true);

#endif
