#include <curses.h>
#include <stdio.h>

#define ctrl(x) ((x) & 0x1f)
#define BACKSPACE 263

typedef enum { NORMAL, INSERT } Mode;

Mode mode = NORMAL;

char *stringify_mode() {
  switch (mode) {
    case NORMAL:
      return "NORMAL";
      break;
    case INSERT:
      return "INSERT";
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

  mvprintw(row - 1, 0, stringify_mode());
  move(0, 0);

  int ch = getch();
  addch(ch);

  int x, y = 0;
  getyx(stdscr, y, x);
  while (ch != ctrl('q')) {
    mvprintw(row - 1, 0, stringify_mode());
    move(y, x);
    ch = getch();

    if (ch == BACKSPACE) {
      getyx(stdscr, y, x);
      move(y, x - 1);
      delch();
    } else {
      addch(ch);
    }
    getyx(stdscr, y, x);
  }
  refresh();

  endwin();

  return 0;
}
