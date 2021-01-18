#include "field.h"
#include <stdlib.h>

static char *field;
static int X, Y;

void initfield(int max_x, int max_y)
{
  X = max_x;
  Y = max_y;
  field = malloc(Y * X);
  int lastrow = (Y - 1) * X - 1;
  for (int i = 0; i < X * Y; i++) {
    //  1st row    lastrow      first col     last col
    //  _____    ___________    ________    ____________
    if (i < X || i > lastrow || !(i % X) || (i % X) == (X - 1)) {
      *(field + i) = '#';
    } else {
      *(field + i) = ' ';
    }
  }
}

void addpoint(int x, int y, char c)
{
  int offset = x + y * X;
  *(field + offset) = c;
}

void rmpoint(int x, int y)
{
  addpoint(x, y, ' ');
}

char *getfield()
{
  return field;
}

