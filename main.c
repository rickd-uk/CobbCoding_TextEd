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

  while (ch != 'q') {
    printw("%c", ch);
    ch = getch();
  }

  refresh();
  /* getch(); */
  endwin();

  return 0;
}
