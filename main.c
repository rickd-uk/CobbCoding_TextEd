#include <curses.h>
#include <stdio.h>

#define ctrl(x) ((x) & 0x1f)

int main(void) {
  // initializes the library
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();

  int row, col;
  getmaxyx(stdscr, row, col);

  mvprintw(row - 1, 0, "NORMAL");
  move(0, 0);

  int ch = getch();
  addch(ch);

  while (ch != ctrl('q')) {
    ch = getch();

    if (ch == 263) {
      int x, y;
      getyx(stdscr, y, x);
      move(y, x - 1);
      delch();
    } else if (ch == ctrl('q')) {
      printw("CTRL + q\n");
      break;
    } else {
      addch(ch);
    }
  }
  refresh();

  endwin();

  return 0;
}
