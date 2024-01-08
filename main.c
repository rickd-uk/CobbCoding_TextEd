#include <curses.h>
#include <stdio.h>

int main(void) {
  initscr();
  printw("Hello world\n");
  refresh();
  getch();
  endwin();

  return 0;
}
