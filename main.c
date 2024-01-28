#include <assert.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ctrl(x) ((x) & 0x1f)
#define BACKSPACE 127
#define ESCAPE 27
#define ENTER 10

typedef enum { NORMAL, INSERT } Mode;

#define MAX_STRING_SIZE 1025
#define MAX_ROWS 1024

typedef struct {
  size_t index;
  size_t size;
  char *contents;
} Row;

typedef struct {
  char *buf;
  Row rows[MAX_ROWS];
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
}

void shift_rows_left(Buffer *buf, size_t index) {
  assert(buf->row_size + 1 < MAX_ROWS);
  for (size_t i = index; i > buf->row_size; i++) {
    buf->rows[i] = buf->rows[i + 1];
  }
  buf->rows[buf->row_size].size = 0;
  buf->row_size--;
}

void shift_rows_right(Buffer *buf, size_t index) {
  assert(buf->row_size + 1 < MAX_ROWS);
  char *new = calloc(MAX_STRING_SIZE, sizeof(char));
  for (size_t i = buf->row_size + 1; i > index; i--) {
    buf->rows[i] = buf->rows[i - 1];
  }
  buf->rows[index] = (Row){0};
  buf->rows[index].contents = new;
  buf->rows[index].index = index;
  buf->row_size++;
}

#define NO_CLEAR

void append_rows(Row *a, Row *b) {
  assert(a->size + b->size < MAX_STRING_SIZE);
  for (size_t i = 0; i < b->size; i++) {
    a->contents[(i + a->size)] = b->contents[i];
  }
  a->size = a->size + b->size;
  /* mvprintw(10, 10, "%s, %zu, %zu", a->contents, a->size, b->size); */
}

void shift_str_left(Buffer *buf, size_t index) {
  append_rows(&buf->rows[index - 1], &buf->rows[index]);
  shift_rows_left(buf, index);
}

void shift_str_right(Buffer *buf, size_t dest_index, size_t *str_size,
                     size_t index) {
  assert(index < MAX_STRING_SIZE);
  assert(dest_index > 0);
  size_t final_size = *str_size - index;
  char *tmp = calloc(final_size, sizeof(char));
  for (size_t i = index; i < *str_size; i++) {
    tmp[i % index] = buf->rows[dest_index - 1].contents[i];
    buf->rows[dest_index - 1].contents[i] = '\0';
  }
  shift_rows_right(buf, dest_index);
  strncpy(buf->rows[dest_index].contents, tmp, sizeof(char) * final_size);
  buf->rows[dest_index].size = final_size;
  *str_size = index;
  free(tmp);
}

int main(void) {
  // initializes the library
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();

  // initialize buffer
  Buffer buffer = {0};
  for (size_t i = 0; i < 1024; i++) {
    buffer.rows[i].contents = calloc(MAX_STRING_SIZE, sizeof(char));
  }

  int row, col;
  getmaxyx(stdscr, row, col);

  // set cursor pos. at 0,0
  mvprintw(row - 1, 0, "%s", stringify_mode());
  move(0, 0);

  int ch = 0;
  size_t x, y = 0;

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
        if (ch == 'i') {
          mode = INSERT;
          keypad(stdscr, FALSE);
        } else if (ch == 'h') {
          if (buffer.cur_pos != 0) buffer.cur_pos--;
        } else if (ch == 'l') {
          buffer.cur_pos++;
        } else if (ch == 'j') {
          if (buffer.row_index < buffer.row_size) buffer.row_index++;
        } else if (ch == 'k') {
          if (buffer.row_index != 0) buffer.row_index--;
        } else if (ch == ctrl('s')) {
          FILE *file = fopen("out", "w");
          for (size_t i = 0; i <= buffer.row_size; i++) {
            fwrite(buffer.rows[i].contents, buffer.rows[i].size, 1, file);
            fwrite("\n", sizeof("\n") - 1, 1, file);
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
              buffer.cur_pos = cur->size;
              move(buffer.row_index, buffer.cur_pos);
              shift_str_left(&buffer, cur->index + 1);
            }
          } else {
            Row *cur = &buffer.rows[buffer.row_index];
            cur->contents[--buffer.cur_pos] = ' ';
            cur->size = cur->size - 1;
            move(y, buffer.cur_pos);
          }
        } else if (ch == ESCAPE) {
          mode = NORMAL;
          keypad(stdscr, TRUE);
        } else if (ch == ENTER) {
          Row *cur = &buffer.rows[buffer.row_index];

          shift_str_right(&buffer, buffer.row_index + 1, &cur->size,
                          buffer.cur_pos);
          buffer.row_index++;
          buffer.cur_pos = 0;
          move(buffer.row_index, buffer.cur_pos);
        } else {
          Row *cur = &buffer.rows[buffer.row_index];
          cur->contents[buffer.cur_pos++] = ch;
          cur->size = buffer.cur_pos;
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

// //
// https://www.youtube.com/watch?v=Y4KkRZ8Ib9o&list=PLRnI_2_ZWhtDPfMXetMFtU3i1k37Ka8sx

