#ifndef FIELD_H_SENTRY
#define FIELD_H_SENTRY

void initfield(int max_x, int max_y);

char *getfield();

void addpoint(int x, int y, char c);

void rmpoint(int x, int y);

#endif
