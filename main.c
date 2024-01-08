#include <curses.h>
#include <stdio.h>

int main(void) {
  // initializes the library
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();

  int ch = getch();

  if (ch == KEY_F(1)) {
    printw("F1 key pressed!\n");
  }

  refresh();
  getch();
  endwin();

  return 0;
}
