#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ctrl(x) ((x) & 0x1f)
#define BACKSPACE 127
#define ESCAPE 27
#define ENTER 10
#define ROW_SIZE 1024

typedef enum { NORMAL, INSERT } Mode;

typedef struct {
  size_t index;
  size_t size;
  char *contents;
} Row;

typedef struct {
  char *buf;
  Row rows[ROW_SIZE];
  size_t row_index;
  size_t cur_pos;
  size_t row_size;
} Buffer;

int QUIT = 0;
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

  // initialize buffer
  Buffer buffer = {0};
  for (size_t i = 0; i < ROW_SIZE; i++) {
    buffer.rows[i].contents = calloc(ROW_SIZE, sizeof(char));
  }

  int row, col;
  (void)col;  // intentionally unused (to supress compiler warnings)
  getmaxyx(stdscr, row, col);

  // set cursor pos. at 0,0
  mvprintw(row - 1, 0, "%s", stringify_mode());
  move(0, 0);

  int ch = 0;
  int x, y = 0;

  while (ch != ctrl('q') && QUIT != 1) {
    clear();
    getmaxyx(stdscr, row, col);
    refresh();
    mvprintw(row - 1, 0, "%s", stringify_mode());
    mvprintw(row - 1, col / 2, "%.3zu:%.3zu", buffer.row_index, buffer.cur_pos);

    for (size_t i = 0; i <= buffer.row_size; i++) {
      mvprintw(i, 0, "%s", buffer.rows[i].contents);
    }

    move(y, x);
    ch = getch();

    switch (mode) {
      case NORMAL:
        if (x > buffer.rows[buffer.row_index].size - 1)
          x = buffer.rows[buffer.row_index].size - 1;
        if (ch == 'i') {
          mode = INSERT;
          keypad(stdscr, FALSE);
        } else if (ch == 'h') {
          if (buffer.cur_pos != 0) buffer.cur_pos--;
        } else if (ch == 'l') {
          /* if (buffer.cur_pos < buffer.rows[buffer.row_index].size) */
          buffer.cur_pos++;
        } else if (ch == 'j') {
          if (buffer.row_index < buffer.row_size) buffer.row_index++;
        } else if (ch == 'k') {
          if (buffer.row_index != 0) buffer.row_index -= 1;
        } else if (ch == ctrl('s')) {
          FILE *file = fopen("out", "w");
          for (size_t i = 0; i <= buffer.row_size; i++) {
            fwrite(buffer.rows[i].contents, buffer.rows[i].size, 1, file);
          }
          fclose(file);
          QUIT = 1;
        }
        if (buffer.cur_pos > buffer.rows[buffer.row_index].size) {
          buffer.cur_pos = buffer.rows[buffer.row_index].size;
        }
        x = buffer.cur_pos;
        y = buffer.row_index;
        move(y, x);
        break;
      case INSERT: {
        keypad(stdscr, FALSE);
        if (ch == BACKSPACE || ch == KEY_BACKSPACE) {
          getyx(stdscr, y, x);
          if (buffer.cur_pos == 0) {
            if (buffer.row_index != 0) {
              Row *cur = &buffer.rows[--buffer.row_index];
              move(buffer.row_index, buffer.cur_pos);
              buffer.cur_pos = cur->size;
            }
          } else {
            Row *cur = &buffer.rows[buffer.row_index];
            cur->contents[--buffer.cur_pos] = ' ';
            cur->size = buffer.cur_pos;
            move(y, buffer.cur_pos);
          }
        } else if (ch == ESCAPE) {
          mode = NORMAL;
          keypad(stdscr, TRUE);
        } else if (ch == ENTER) {
          buffer.rows[buffer.row_index]
              .contents[buffer.rows[buffer.row_index].size] = '\n';
          buffer.row_index++;
          buffer.row_size++;
          buffer.cur_pos = 0;
          move(buffer.row_index, buffer.cur_pos);
        } else {
          Row *cur = &buffer.rows[buffer.row_index];
          cur->contents[buffer.cur_pos++] = ch;
          cur->size = buffer.cur_pos;
          /* size_t col_index = strlen(cur->contents); */
          move(y, buffer.cur_pos);
        }
        break;
      }
    }
    getyx(stdscr, y, x);
  }
  refresh();
  endwin();

  return 0;
}

// https://www.youtube.com/watch?v=Y4KkRZ8Ib9o&list=PLRnI_2_ZWhtDPfMXetMFtU3i1k37Ka8sx
