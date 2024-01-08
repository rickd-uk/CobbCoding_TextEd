#include <curses.h>
#include <stdio.h>

int main(void) {
  // initializes the library
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();

  int ch = getch();

  printw("%d\n", ch);
  while (ch != 'q') {
    ch = getch();
    printw("%d\n", ch);
  }

  refresh();
  /* getch(); */
  endwin();

  return 0;
}
