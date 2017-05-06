/** Unit test of Link.c.

 @file		TestLink
 @author	Neil
 @std		C89/90
 @version	1.0; 2017-05
 @since		1.0; 2017-05 scraped from TestList.h */

#include <stdlib.h> /* EXIT_ malloc free */
#include <stdio.h>  /* fprintf */
#include <string.h>	/* strcmp */
#include "Orcish.h"

#ifdef __GNUC__ /* <-- GCC */
/*#pragma GCC diagnostic ignored "-Wunused-function"*/ /* doesn't work */
/*#pragma GCC diagnostic ignored "-Wconversion"*/ /* I want this :[ assert */
#elif _MSC_VER /* GCC --><-- MSVC: not a C89/90 compiler; needs a little help;
"Assignment within conditional expression." "<ANSI89/ISO90 name>: The POSIX
name for this item is deprecated. Instead use the ISO C and C++ conformant
name <ISO C++11 name>." */
#pragma warning(disable: 4706)
#pragma warning(disable: 4996)
#elif __BORLANDC__ /* MSVC --><-- BCC */
#elif __MINGW32__ /* BCC --><-- MinGW */
#elif __DJGPP__ /* MinGW --><-- DJGPP */
#endif /* --> */

/** Define class Foo */
struct Foo {
	int key;
	char value[32];
};
/** @implements <Foo>Comparator */
static int Foo_key_cmp(const struct Foo *a, const struct Foo *b) {
	return (b->key < a->key) - (a->key < b->key);
}
/** @implements <Foo>Comparator */
static int Foo_value_cmp(const struct Foo *a, const struct Foo *b) {
	return strcmp(a->value, b->value);
}
/** @implements <Foo>ToString */
static void Foo_to_string(const struct Foo *this, char (*const a)[9]) {
	snprintf(*a, sizeof *a, "%.3d%.3s", this->key, this->value);
}
/** @implements <Foo>Action */
static void Foo_filler(struct Foo *const this) {
	this->key = (float)(rand() / (RAND_MAX + 1.0) * 99);
	Orcish(this->value, sizeof this->value);
}
#define LINK_NAME Foo
#define LINK_TYPE struct Foo
#define LINK_A_NAME Key
#define LINK_A_COMPARATOR &Foo_key_cmp
#define LINK_B_NAME Value
#define LINK_B_COMPARATOR &Foo_value_cmp
#define LINK_TO_STRING &Foo_to_string
#define LINK_TEST &Foo_filler
#include "../src/Link.h"

/** Animal virtual functions. */
struct AnimalVt;
/** Define class Animal. */
struct Animal {
	const struct AnimalVt *vt;
	int x;
	char name[16];
};
/** @implements <Animal>Comparator */
static int Animal_name_cmp(const struct Animal *a, const struct Animal *b) {
	return strcmp(a->name, b->name);
}
/** @implements <Animal>Comparator */
static int Animal_x_cmp(const struct Animal *a, const struct Animal *b) {
	return (b->x < a->x) - (a->x < b->x);
}
/** @implements <Animal>ToString */
static void Animal_to_string(const struct Animal *this, char (*const a)[9]) {
	snprintf(*a, sizeof *a, "%d%s", this->x, this->name);
}
/* @implements <Animal>Action */
static void Animal_filler(struct Animal *const this) {
	Orcish(this->name, sizeof this->name);
	this->vt = 0;
	this->x  = (unsigned)(200.0 * rand() / (RAND_MAX + 1.0) - 100.0);
}
#define LINK_NAME Animal
#define LINK_TYPE struct Animal
#define LINK_C_NAME Name
#define LINK_C_COMPARATOR &Animal_name_cmp
#define LINK_D_NAME X
#define LINK_D_COMPARATOR &Animal_x_cmp
#define LINK_TO_STRING &Animal_to_string
#define LINK_TEST &Animal_filler
#include "../src/Link.h"
struct Sloth {
	struct AnimalLinkNode animal;
	unsigned lazy;
};
struct Llama {
	struct AnimalLinkNode animal;
	unsigned chomps;
};
struct Bear {
	struct AnimalLinkNode animal;
	struct Animal *riding;
};
/** @implements <Animal>Action */
static void sloth_act(struct Animal *const animal) {
	struct Sloth *const sloth = (struct Sloth *)animal;
	printf("Sloth %s at %d has been sleeping %u hours.\n", animal->name, animal->x, sloth->lazy);
}
/** @implements <Animal>Action */
static void llama_act(struct Animal *const animal) {
	struct Llama *const llama = (struct Llama *)animal;
	printf("Llama %s at %d has chomped %u fingers today.\n", animal->name, animal->x, llama->chomps);
}
/** @implements <Animal>Action */
static void bear_act(struct Animal *const animal) {
	struct Bear *const bear = (struct Bear *)animal;
	printf("Bear %s at %d is riding on llama %s.\n", animal->name, animal->x,
		bear->riding->name);
}
static const struct AnimalVt {
	void (*const act)(struct Animal *const);
} sloth_vt = { &sloth_act }, llama_vt = { &llama_act }, bear_vt = { &bear_act };
/* the linked-list */
static struct AnimalLink animals;
static void Animal_init(struct AnimalLinkNode *const this) {
	Animal_filler((struct Animal *)this);
	AnimalLinkAdd(&animals, this);
}
static void Sloth_init(struct Sloth *const sloth) {
	Animal_init(&sloth->animal);
	sloth->animal.data.vt = &sloth_vt;
	sloth->lazy = (unsigned)(100.0 * rand() / (RAND_MAX + 1.0) + 20.0);
}
static void Llama_init(struct Llama *const llama) {
	Animal_init(&llama->animal);
	llama->animal.data.vt = &llama_vt;
	llama->chomps = (unsigned)(10.0 * rand() / (RAND_MAX + 1.0) + 1.0);
}
static void Bear_init(struct Bear *const bear, const struct Llama *const llamas,
	const size_t llamas_size) {
	Animal_init(&bear->animal);
	bear->animal.data.vt = &bear_vt;
	bear->riding = (struct Animal *)(llamas
		+ (unsigned)((double)llamas_size * rand() / (RAND_MAX + 1.0)));
	/* Overwrite to make the Bear and the Llama the same. */
	bear->animal.data.x = bear->riding->x;
}
/* @implements AnimalAction */
static void act(struct Animal *const this) {
	this->vt->act(this);
}
/** Test BlockMove. */
static void test_block_move() {
	struct Sloth sloths[3];
	const size_t sloths_size = sizeof sloths / sizeof *sloths;
	struct Llama llamas[6];
	const size_t llamas_size = sizeof llamas / sizeof *llamas;
	struct Bear bears[2];
	const size_t bears_size = sizeof bears / sizeof *bears;
	struct Sloth others[9];
	const size_t others_size = sizeof others / sizeof *others;
	size_t i;
	assert(others_size >= sloths_size);
	for(i = 0; i < sloths_size; i++) Sloth_init(sloths + i);
	for(i = 0; i < llamas_size; i++) Llama_init(llamas + i);
	for(i = 0; i < bears_size; i++)  Bear_init(bears + i, llamas, llamas_size);
	printf("Unsorted: by name %s; by x %s.\n", AnimalLinkNameToString(&animals),
		AnimalLinkXToString(&animals));
	AnimalLinkSort(&animals);
	printf("Sorted: by name %s; by x %s.\n", AnimalLinkNameToString(&animals),
		AnimalLinkXToString(&animals));
	printf("By name:\n");
	AnimalLinkNameForEach(&animals, &act);
	printf("By x:\n");
	AnimalLinkXForEach(&animals, &act);
	_Animal_in_order(&animals);
	memcpy(others, sloths, sizeof sloths);
	for(i = 0; i < sloths_size; i++) sloths[i].animal.data.name[0] = '!';
	printf("Moved sloths: %s.\n", AnimalLinkNameToString(&animals));
	AnimalLinkBlockMove(&animals, sloths, sizeof sloths, others);
	printf("Block move: %s.\n", AnimalLinkNameToString(&animals));
	for(i = sloths_size; i < others_size; i++) Sloth_init(others + i);
	printf("New sloths: %s.\n", AnimalLinkNameToString(&animals));
	AnimalLinkSort(&animals);
	printf("Sorted: by name %s; by x %s.\n", AnimalLinkNameToString(&animals),
		AnimalLinkXToString(&animals));
	printf("By name:\n");
	AnimalLinkNameForEach(&animals, &act);
	printf("By x:\n");
	AnimalLinkXForEach(&animals, &act);
	_Animal_in_order(&animals);
}

/** Entry point.
 @return Either EXIT_SUCCESS or EXIT_FAILURE. */
int main(void) {

	FooLinkTest();
	AnimalLinkTest();
	test_block_move();
	printf("Test succeeded.\n\n");

	return EXIT_SUCCESS;
}
