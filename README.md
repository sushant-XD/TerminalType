# Terminal Typing Speed Checker Application

This is a Terminal Typing Speed Checker application built for Linux Systems. Relies on Termios for input, output, and cursor control.
Takes in raw keyboard inputs and handles manually, ansi escape sequences used for colors and terminal UI management.
Widget-based UI development system written up using termios and ansi escape sequences. 
I've tried to make this modular and useful for the work that a terminal typing application needs.
There's no terminal UI framework like BubbleTea or ink because this is meant to be a very simple application, and I wanted to have total control over the terminal (and the UI is really simple).

### Running
1) Clone the repository: ` git clone https://github.com/sushant-XD/TerminalType.git `
2) Create a build folder: ` mkdir build`
3) Go to build folder: `cd build`
4) make the project: `cmake .. && make`
6) Run `TerminalType`([if installed globally](#To-install-globally) or run `./TerminalType` (from inside build folder)
7) Change settings, run the test.

### To install globally
1) Clone the repository: ` git clone https://github.com/sushant-XD/TerminalType.git `
2) Create a build folder go into that: ` mkdir build && cd build`
3) Run:
```
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
cmake --build .
sudo cmake --install .
TeerminalType
```

## Features to Implement 
This is a very crude implementation, and there are a few things that will make this project actually useful. 

- Let users use a text file or custom text in addition to already available ones
- right now, if a character is wrong, all the next sequence of characters are marked as wrong. User should be able to change this and make it like normal typing games like monkeytype and 10ff
- Create a config file to change UI and default settings easily
- Modify/Refactor the UI drawing and updating code so that its easy to draw/maintain the UI elements
