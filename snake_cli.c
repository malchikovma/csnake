#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <threads.h>
#include <termios.h>
#include <unistd.h>
#include "snake.c"

void field_clear(char field[SCR_WIDTH][SCR_HEIGHT])
{
	for (size_t i = 0; i < SCR_WIDTH; ++i) {
		for (size_t j = 0; j < SCR_HEIGHT; ++j) {
			field[i][j] = '.';
		}
	}
}

void field_place_apple(char field[SCR_WIDTH][SCR_HEIGHT], apple_t *apple)
{
	if (apple->x < SCR_WIDTH && apple->y < SCR_HEIGHT
	 && apple->x > -1        && apple->y > -1         ) {
		field[apple->x][apple->y] = '0';
	}
}

void field_place_snake(char field[SCR_WIDTH][SCR_HEIGHT], snake_t *snake)
{
	seg_t *seg = snake->fseg;
	while (seg) {
		if (seg->x < SCR_WIDTH && seg->y < SCR_HEIGHT
		 && seg->x > -1        && seg->y > -1         ) {
			field[seg->x][seg->y] = '*';
		}
		seg = seg->next;
	}
}

void field_print(char field[SCR_WIDTH][SCR_HEIGHT])
{
	puts("\e[1;1H\e[2J"); // Clear terminal ANSI escape code
	for (size_t j = 0; j < SCR_HEIGHT; ++j) {
		for (size_t i = 0; i < SCR_WIDTH; ++i) {
			putchar(field[i][j]);
		}
		putchar('\n');
	}
}

void ui_thread_sleep(void)
{

		struct timespec half_sec = {
			.tv_sec  = 0,
			         //500 000 000
			.tv_nsec = 500000000
		};
		thrd_sleep(&half_sec, 0);
}

void game_do_step(snake_t *snake, apple_t *apple)
{
	snake_do_step(snake);
	if (!snake_eat(snake, apple)) {
		return;
	}
	bool planted = false;
	while (!planted) {
		planted = apple_plant(apple, snake, rand);
	}
}

int ui_thread_start(void *snake_v)
{
	snake_t *snake = snake_v;
	apple_t *apple = apple_new(0, 0);
	apple_plant(apple, snake, rand);
	char field[SCR_WIDTH][SCR_HEIGHT];
	while (!snake->quit) {
		ui_thread_sleep();
		game_do_step(snake, apple);
		field_clear(field);
		field_place_apple(field, apple);
		field_place_snake(field, snake);
		field_print(field);
	}
	return 0;
}

int input_thread_start(void *snake_v)
{
	snake_t *snake = snake_v;
	struct termios oldt = {0};
	struct termios newt = {0};
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON|ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	int in = 0;
	while (!snake->quit) {
		in = getc(stdin);
		if (in == EOF) {
			in = 'q';
		}
		switch (in) {
		case 'w':
			snake_set_dir(snake, DIR_TOP);
			break;
		case 'a':
			snake_set_dir(snake, DIR_LEFT);
			break;
		case 's':
			snake_set_dir(snake, DIR_BOTTOM);
			break;
		case 'd':
			snake_set_dir(snake, DIR_RIGHT);
			break;
		case 'q':
			snake->quit = true;
			break;
		case '\n':
		default:
			break;
		}
	}

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return 0;
}

int main(void)
{
	snake_t *snake = snake_new(10, 10);
	thrd_t ui_thread = {0};
	thrd_t input_thread = {0};

	thrd_create(&ui_thread, ui_thread_start, snake);
	thrd_create(&input_thread, input_thread_start, snake);

	thrd_join(ui_thread, 0);
	thrd_join(input_thread, 0);

	snake_destroy(snake);
}

