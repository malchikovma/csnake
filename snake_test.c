#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "snake.c"

/*
Ideas:
ASSERT macro - print method and line
TEST macro - add function to array of tests
*/

/** Return non-zero in case of error. */
int assert(bool expr, char* msg)
{
	if (expr) {
		return 0;
	}
	fprintf(stderr, "%s\n", msg);
	return 1;
}

int test_seg_new(void)
{
	seg_t *s = seg_new(1, 2);
	bool fail = assert(s->x == 1 && s->y == 2, "Unexpected seg coord");
	return fail;
}

int test_snake_set_dir(void)
{
	bool fail = false;
	snake_t *s = snake_new(0, 0);
	snake_set_dir(s, DIR_RIGHT);
	if (assert(s->dx == 1 && s->dy == 0, "Unexpected right dir")) {
		fail = true;
		goto CLEANUP;
	}

	snake_set_dir(s, DIR_TOP);
	if (assert(s->dx == 0 && s->dy == -1, "Unexpected top dir")) {
		fail = true;
		goto CLEANUP;
	}

	snake_set_dir(s, DIR_LEFT);
	if (assert(s->dx == -1 && s->dy == 0, "Unexpected left dir")) {
		fail = true;
		goto CLEANUP;
	}

	snake_set_dir(s, DIR_BOTTOM);
	if (assert(s->dx == 0 && s->dy == 1, "Unexpected bottom dir")) {
		fail = true;
		goto CLEANUP;
	}

	CLEANUP:
	snake_destroy(s);
	return fail;
}

int test_snake_do_step_right(void)
{
	bool fail = false;
	snake_t *s = snake_new(0, 0); // See default position
	snake_set_dir(s, DIR_RIGHT);
	snake_do_step(s);
	// Every segment x coordinate moved by 1
	bool correct_head_coords = s->x == 1 && s->y == 0;
	if (assert(correct_head_coords , "Incorrect head coordinates")) {
		fail = true;
		goto CLEANUP;
	}
	seg_t *first_seg = s->fseg;
	bool correct_1st_seg_coords = first_seg->x == 1
			           && first_seg->y == 0;
	if (assert(correct_1st_seg_coords, "Incorrect 1st seg coords")) {
		fail = true;
		goto CLEANUP;
	}
	seg_t *sec_seg = first_seg->next;
	bool correct_2nd_seg_coords = sec_seg->x == 0 && sec_seg->y == 0;
	if (assert(correct_2nd_seg_coords, "Incorrect 2nd seg coords")) {
		fail = true;
		goto CLEANUP;
	}
	seg_t *last_seg = sec_seg->next;
	bool correct_last_seg_coords = last_seg ->x == -1 && sec_seg->y == 0;
	if (assert(correct_last_seg_coords, "Incorrect last seg coords")) {
		fail = true;
		goto CLEANUP;
	}

	CLEANUP:
	snake_destroy(s);
	return fail;
}

int test_snake_do_step_bottom(void)
{
	bool fail = false;
	snake_t *s = snake_new(0, 0); // See default position
	snake_set_dir(s, DIR_BOTTOM);
	snake_do_step(s);
	// Every segment x coordinate moved by 1
	bool correct_head_coords = s->x == 0 && s->y == 1;
	if (assert(correct_head_coords , "Incorrect head coordinates")) {
		fail = true;
		goto CLEANUP;
	}
	seg_t *first_seg = s->fseg;
	bool correct_1st_seg_coords = first_seg->x == 0
			           && first_seg->y == 1;
	if (assert(correct_1st_seg_coords, "Incorrect 1st seg coords")) {
		fail = true;
		goto CLEANUP;
	}
	seg_t *sec_seg = first_seg->next;
	bool correct_2nd_seg_coords = sec_seg->x == 0 && sec_seg->y == 0;
	if (assert(correct_2nd_seg_coords, "Incorrect 2nd seg coords")) {
		fail = true;
		goto CLEANUP;
	}
	seg_t *last_seg = sec_seg->next;
	bool correct_last_seg_coords = last_seg ->x == -1 && sec_seg->y == 0;
	if (assert(correct_last_seg_coords, "Incorrect last seg coords")) {
		fail = true;
		goto CLEANUP;
	}

	CLEANUP:
	snake_destroy(s);
	return fail;
}

int test_snake_add_seg(void)
{
	bool fail = false;
	snake_t *s = snake_new(10, 10);
	snake_add_seg(s);
	// Start with 3 segments + 1 added
	seg_t *seg = s->fseg->next->next->next;
	if (assert(seg, "4th segment not added")) {
		fail = true;
		goto CLEANUP;
	}

	CLEANUP:
	snake_destroy(s);
	return fail;
}

int test_snake_eat_hit(void)
{
	bool fail = false;
	snake_t *s = snake_new(0, 0);
	apple_t *a = apple_new(0, 0);
	bool hit = snake_eat(s, a);
	if (assert(hit, "Expected true on hit")) {
		fail = true;
		goto CLEANUP;
	}
	if (assert(snake_seg_len(s) == 4, "Expected new segment")) {
		fail = true;
		goto CLEANUP;
	}
	CLEANUP:
	snake_destroy(s);
	apple_destroy(a);
	return fail;
}

int test_snake_eat_miss(void)
{
	bool fail = false;
	snake_t *s = snake_new(0, 0);
	apple_t *a = apple_new(1, 0);
	bool hit = snake_eat(s, a);
	if (assert(!hit, "Expected false on miss")) {
		fail = true;
		goto CLEANUP;
	}
	if (assert(snake_seg_len(s) == 3, "Expected no new segment")) {
		fail = true;
		goto CLEANUP;
	}
	CLEANUP:
	snake_destroy(s);
	apple_destroy(a);
	return fail;
}

int test_snake_occupies_coord(void)
{
	bool fail = false;
	snake_t *s = snake_new(2, 0); // Coords: 0:0, 1:0, 2:0
	bool occ_cell_0_0 = snake_occupies_coord(s, 0, 0);
	if (assert(occ_cell_0_0, "Expected to occupy 0:0 coord")) {
		fail = true;
		goto CLEANUP;
	}
	bool occ_cell_1_0 = snake_occupies_coord(s, 1, 0);
	if (assert(occ_cell_1_0, "Expected to occupy 1:0 coord")) {
		fail = true;
		goto CLEANUP;
	}
	bool occ_cell_2_0  = snake_occupies_coord(s, 2, 0);
	if (assert(occ_cell_2_0, "Expected to occupy 2:0 coord")) {
		fail = true;
		goto CLEANUP;
	}
	bool occ_cell_3_0 = snake_occupies_coord(s, 3, 0);
	if (assert(!occ_cell_3_0 , "Expected not to occupy 3:0 coord")) {
		fail = true;
		goto CLEANUP;
	}

	CLEANUP:
	snake_destroy(s);
	return fail;
}

int test_rand_0(void)
{
	return 0;
}

int test_rand_1(void)
{
	return 1;
}

int test_apple_plant(void)
{
	bool fail = false;
	apple_t *a = apple_new(0, 0);
	snake_t *s = snake_new(0, 0);
	bool planted = apple_plant(a, s, test_rand_0); // test_rand_0 gives 0:0 coordinate
	if (assert(!planted, "Should fail when planted on a snake")) {
		fail = true;
		goto CLEANUP;
	}
	planted = apple_plant(a, s, test_rand_1); // test_rand_1 gives 1:1 coordinate
	if (assert(planted, "Should be ok to plant not on a snake")) {
		fail = true;
		goto CLEANUP;
	}
	bool new_coords = a->x == 1 && a->y == 1;
	if (assert(new_coords, "Unexpected new apple coordinates")) {
		fail = true;
		goto CLEANUP;
	}
	CLEANUP:
	snake_destroy(s);
	apple_destroy(a);
	return fail;
}

/** Return non-zero in case of error. */
typedef int (*test_func)(void);

int main(void)
{
        /* Add test function here. */
	test_func tests[] = {
		test_seg_new,
		test_snake_set_dir,
		test_snake_do_step_right,
		test_snake_do_step_bottom,
		test_snake_add_seg,
		test_snake_eat_hit,
		test_snake_eat_miss,
		test_snake_occupies_coord,
		test_apple_plant,
	};
	size_t num_of_tests = sizeof(tests) / sizeof(test_func);
	for (size_t i = 0; i < num_of_tests; ++i) {
		if (tests[i]()) {
			fprintf(stderr, "\nTest failed: %zu\n", i);
			return EXIT_FAILURE;
		}
		fprintf(stderr, ".");
		if (i == num_of_tests - 1) {
			fprintf(stderr, "\nAll tests are passed\n");
		}
	}
	return EXIT_SUCCESS;
}
