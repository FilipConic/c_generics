# Generic containers in C

Everything is created using C function macros.
Inspired by Sean Barret's [**stb**](https://github.com/nothings/stb) and Tsoding's [**nob.h**](https://github.com/tsoding/nob.h)!

You can turn any structure into any container offered here, you just have to add the given fields and `#include *container*.h`!
Here is a list of the containers created and what you need to add to your struct to turn it into the given container!

To use any of the `.h` files you need to add the flag `-I./src/include`

|Container      | Fields you need     |
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
|               |                     |
|option.h       |**Option Struct**    |
|               |`> generic` value    |
|               |`> char` empty       |

Other usefull libs added like strings, binary heap, regex and hash functions.

To use them you need to add a flag `-L./src/libs`

|Include      | Lib      |
|:------------|:---------|
|string.h     |`-lstring`|
|hashfuncs.h  |`-lhash`  |
|regex.h      |`-lregex` |
|binary_heap.h|`-lheap`  |

