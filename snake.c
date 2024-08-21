#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

struct seg {
  	struct seg *next;
	int x;
	int y;
};
typedef struct seg seg_t;

struct snake {
	int x;
	int y;
	int dx;
	int dy;
	bool quit;
	seg_t *fseg;
};
typedef struct snake snake_t;

typedef struct apple {
	int x;
	int y;
} apple_t;

enum {
	DIR_RIGHT = 0,
	DIR_TOP = 1,
	DIR_LEFT = 2, 
	DIR_BOTTOM = 3,
};

enum {
	SCR_WIDTH = 20,
	SCR_HEIGHT = 20,
};

void seg_fprint(seg_t *s, FILE* stream)
{
	fprintf(stream, "seg_t{x=%d,y=%d}\n", s->x, s->y);
}

seg_t *seg_new(int x, int y)
{
	seg_t *s = malloc(sizeof(seg_t));
	if (!s) {
		fputs("Failed to allocate segment\n", stderr);
		exit(1);
	}
	s->x = x;
	s->y = y;
	s->next = 0;
	return s;
}

void seg_destroy(seg_t *s)
{
	free(s);
}

void snake_fprint(snake_t *s, FILE* stream)
{
	fprintf(stream, "snake_t{\n  "
	                "  x=%d, y=%d,\n"
			"  dx=%d, dy=%d,\n", s->x, s->y, s->dx, s->dy);
	seg_t *seg = s->fseg;
	while (seg) {
		fprintf(stream, "  ");
		seg_fprint(seg, stream);
		seg = seg->next;
	}
	fprintf(stream, "}\n");
}

snake_t *snake_new(int x, int y)
{
	snake_t *s = malloc(sizeof(snake_t));
	if (!s) {
		fputs("Failed to allocate snake\n", stderr);
		exit(1);
	}
	s->x = x;
	s->y = y;
	// Default right direction
	s->dx = 1;
	s->dy = 0;
	s->quit = false;
	s->fseg = seg_new(x, y);
	s->fseg->next = seg_new(x - 1, y);
	s->fseg->next->next = seg_new(x - 2, y);
	return s;
}

void snake_destroy(snake_t *s)
{
	seg_t *nseg = s->fseg;
	seg_t *pseg = 0;
	while (nseg) {
		pseg = nseg;
		nseg = nseg->next;
		seg_destroy(pseg);
	}
	free(s);
	s = 0;
}

void snake_add_seg(snake_t *s)
{
	seg_t *last = s->fseg;
	do {
		last = last->next;
	} while (last->next);
	last->next = seg_new(last->x, last->y);
}

int snake_seg_len(snake_t *s)
{
	int seg_len = 0;
	seg_t *seg = s->fseg;
	while (seg) {
		seg_len++;
		seg = seg->next;
	}
	return seg_len;
}

void snake_set_dir(snake_t *s, int dir)
{
	/* For 4 directions would be easier to hardcode */
	int deg = dir * 90;
	double rad = deg * M_PI / 180;
	s->dx = cos(rad);
	s->dy = -sin(rad); /* Cartesian coords to idk */ 
}

void snake_do_step(snake_t *s)
{
	s->x += s->dx;
	s->y += s->dy;
	seg_t *seg = s->fseg;
	// Tell next segment where to move and move itself
	int prevx = s->x;
	int prevy = s->y;
	int curx, cury = 0;
	while (seg) {
		curx = seg->x;
		cury = seg->y;
		seg->x = prevx;
		seg->y = prevy;
		prevx = curx;
		prevy = cury;
		seg = seg->next;
	} 
}

bool snake_eat(snake_t *s, apple_t *a)
{
	if (s->x == a->x && s->y == a->y) {
		snake_add_seg(s);
		return true;
	} 
	return false;
}

bool snake_occupies_coord(snake_t *s, int x, int y)
{
	seg_t *seg = s->fseg;
	while (seg) {
		if (seg->x == x && seg->y == y) {
			return true;
		}
		seg = seg->next;
	}
	return false;
}

apple_t* apple_new(int x, int y)
{
	apple_t *a = malloc(sizeof(apple_t));
	a->x = x;
	a->y = y;
	return a;
}

void apple_destroy(apple_t *a)
{
	free(a);
}

/**
Try to plant apple. Failed if planted on a snake;

Usage:

bool planted = false;
while (!planted) {
	planted = apple_plant(apple, snake, rand);
}
 **/
bool apple_plant(apple_t *a, snake_t *s, int (*frand)(void))
{
	int x = frand() % SCR_WIDTH;
	int y = frand() % SCR_HEIGHT;
	if (snake_occupies_coord(s, x, y)) {
		return false;
	}
	a->x = x;
	a->y = y;
	return true;
}

