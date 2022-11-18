#ifndef BLUECHIHUAHUA_ENUM
#define BLUECHIHUAHUA_ENUM

#include <stddef.h>

static inline const char *enum_to_string(int i, const char *const *table,
					 int table_size) {
	if (i < 0 || i >= table_size)
		return NULL;
	return table[i];
}

#define ELEMENTSOF(x)                                                        \
	(__builtin_choose_expr(!__builtin_types_compatible_p(typeof(x),      \
							     typeof(&*(x))), \
			       sizeof(x) / sizeof((x)[0]), NULL))

#define ENUM_TO_STRING(i, table) \
	(enum_to_string((int)(i), table, ELEMENTSOF(table)))

#endif