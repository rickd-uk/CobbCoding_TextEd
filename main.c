#include <curses.h>
#include <stdio.h>

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

  while (ch != 'q') {
    ch = getch();

    if (ch == 263) {
      int x, y;
      getyx(stdscr, y, x);
      move(y, x - 1);
      delch();
    } else {
      addch(ch);
    }
    /* printw("%d", ch); */
  }

  refresh();
  /* getch(); */
  endwin();

  return 0;
}
