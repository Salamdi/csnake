#include "snake.h"
#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "field.h"

#define KEY_SPACE 32
#define MIN_SIZE 10
#define SPEED_CHANGE 20000
enum dir { up, down, right, left };
static char *field;
static int  width, height, offsetx, offsety, direction,
            foodx, foody, gameon, period = 500000, score = 0;
node *head = NULL;

static void render_field()
{
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      mvaddch(i+offsety, j+offsetx, *(field + i * width + j));
    }
  }
  refresh();
}

static void addnode(node *nd)
{
  if (!nd->nxt->nxt) {
    node *tail = nd->nxt;
    node *btail = nd;
    node *nnd = malloc(sizeof(node));
    nnd->x = tail->x + (tail->x - btail->x);
    nnd->y = tail->y + (tail->y - btail->y);
    nnd->nxt = NULL;
    tail->nxt = nnd;
  } else {
    addnode(nd->nxt);
  }
}

int bodycollision(node *chain, int x, int y)
{
  if (!chain)
    return 0;
  if (x == chain->x && y == chain->y)
    return 1;
  return bodycollision(chain->nxt, x, y);
}

static void addfood()
{
  int r;
  do {
    r = rand() % (width * height);
    foodx = r % width;
    foody = r / width;
  } while (r < width
    || r > (height - 1) * width - 1
    || !(r % width)
    || (r % width) == (width - 1)
    || bodycollision(head, foodx, foody)); 
  addpoint(foodx, foody, 'O');
}

static void movehead(node *nd)
{
  switch (direction) {
  case up:
    nd->y--;
    break;
  case down:
    nd->y++;
    break;
  case right:
    nd->x++;
    break;
  case left:
    nd->x--;
    break;
  }
  if (nd->x == foodx && nd->y == foody) {
    rmpoint(foodx, foody);
    addfood();
    addnode(head);
    period -= SPEED_CHANGE;
    score++;
  }
  if (nd->y == 0
    || nd->y == height - 1
    || nd->x == 0
    || nd->x == width - 1
    || bodycollision(head->nxt, head->x, head->y)) {
    gameon = 0;
  }
}

void addchain(node *nd)
{
  if (!nd)
    return;
  addpoint(nd->x, nd->y, 'X');
  addchain(nd->nxt);
}

void rmchain(node *nd)
{
  if (!nd)
    return;
  rmpoint(nd->x, nd->y);
  rmchain(nd->nxt);
}

static void movechain(node *nd)
{
  if (!(nd->nxt)) {
    return;
  }
  movechain(nd->nxt);
  nd->nxt->x = nd->x;
  nd->nxt->y = nd->y;
  if (nd == head) {
    movehead(nd);
  }
}

static void nextturn()
{
  rmchain(head);
  movechain(head);
  addchain(head);
}

int main(int argc, char **argv)
{
  if (argc == 3) {
    width = atoi(argv[1]);
    width = width > MIN_SIZE ? width : MIN_SIZE;
    height = atoi(argv[2]);
    height = height > MIN_SIZE ? height : MIN_SIZE;
  } else if (argc == 2) {
    width = atoi(argv[1]);
    width = width > MIN_SIZE ? width : MIN_SIZE;
    height = width;
  } else {
    width = MIN_SIZE;
    height = MIN_SIZE;
  }
  srand(time(NULL));
  int row, col;
  initscr();
  getmaxyx(stdscr, row, col);
  cbreak();
  keypad(stdscr, 1);
  noecho();
  curs_set(0);
  offsetx = (col - width) / 2;
  offsety = (row - height) / 2;
  initfield(width, height);
  nodelay(stdscr, 1);
  field = getfield();
  direction = right;
  head = malloc(sizeof(node));
  node *body = malloc(sizeof(node));
  head->x = width / 2;
  head->y = height / 2;
  body->x = head->x - 1;
  body->y = head->y;
  head->nxt = body;
  body->nxt = NULL;
  addchain(head);
  addfood();
  gameon = 1;
  render_field();
  while (gameon) {
    int key = getch();
    switch(key) {
    case KEY_RIGHT:
      if (direction != left) {
        direction = right;
      }
      break;
    case KEY_LEFT:
      if (direction != right) {
        direction = left;
      }
      break;
    case KEY_UP:
      if (direction != down) {
        direction = up;
      }
      break;
    case KEY_DOWN:
      if (direction != up) {
        direction = down;
      }
      break;
    case KEY_SPACE:
      addnode(head);
    }
    nextturn();
    mvprintw(1, 1, "score: %d", score);
    render_field();
    usleep(period);
  }
  endwin();
  return 0;
}

