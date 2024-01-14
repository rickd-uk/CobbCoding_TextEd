#include <curses.h>
#include <stdio.h>

#define ctrl(x) ((x) & 0x1f)
#define BACKSPACE 263
#define ESCAPE 27

typedef enum { NORMAL, INSERT } Mode;

Mode mode = NORMAL;

const char *stringify_mode() {
  switch (mode) {
    case NORMAL:
      return "NORMAL";
      break;
    case INSERT:
      return "INSERT";
      break;
    default:
      return "NORMAL";
      break;
  }
  return "NORMAL";
}

int main(void) {
  // initializes the library
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();

  int row, col;
  getmaxyx(stdscr, row, col);

  mvprintw(row - 1, 0, "%s", stringify_mode());
  move(0, 0);

  int ch;

  int x, y = 0;
  while (ch != ctrl('q')) {
    mvprintw(row - 1, 0, "%s", stringify_mode());
    move(y, x);
    ch = getch();

    switch (mode) {
      case NORMAL:
        if (ch == 'i') {
          mode = INSERT;
        }
        break;
      case INSERT:
        if (ch == BACKSPACE) {
          getyx(stdscr, y, x);
          move(y, x - 1);
          delch();
        } else if (ch == ESCAPE) {
          mode = NORMAL;
        } else {
          addch(ch);
        }
        break;
    }
    getyx(stdscr, y, x);
  }
  refresh();

  endwin();

  return 0;
}
