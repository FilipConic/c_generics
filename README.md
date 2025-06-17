# Generic containers in C

Everything is created using C function macros.
You can turn any structure into any container offered here, you just have to add the given fields and **#include** 'container'.h!
Here is a list of the containers created and what you need to add to your struct to turn it into the given container!
|Container name | Fields you need     |
|:--------------|:--------------------|
|array.h        |**Array struct**     |
|               |`> generic*` buffer  |
|               |`> uint32_t` capacity|
|               |`> uint32_t` len     |
|               |                     |
|queue.h        |**Queue struct**     |
|               |`> generic*` buffer  |
|               |`> uint32_t` capacity|
|               |`> uint32_t` len     |
|               |`> uint32_t` head    |
|               |`> uint32_t` tail    |
|               |                     |
|stack.h        |**Stack struct**     |
|               |`> generic*` buffer  |
|               |`> uint32_t` capacity|
|               |`> uint32_t` len     |
|               |                     |
|hashset.h      |**Hashset struct**   |
|               |`> uint64_t*` bitmap |
|               |`> generic*` buffer  |
|               |`> uint32_t` capacity|
|               |`> uint32_t` len     |
|               |`> uint64_t (*`hash`)(generic)`   |
|               |`> int (*`cmp`)(generic, generic)`|
|               |                     |
|hashmap.h      |**KeyValue struct needed too**|
|               |`> generic1` key     |
|               |`> generic2` value   |
|               |                     |
|               |**Hashmap Struct**   |
|               |`> uint64_t*` bitmap |
|               |`> KeyValue*` buffer |
|               |`> uint32_t` capacity|
|               |`> uint32_t` len     |
|               |`> uint64_t (*`hash`)(generic1)`    |
|               |`> int (*`cmp`)(generic1, generic1)`|
