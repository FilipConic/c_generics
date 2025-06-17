# Generic containers in C

Everything is created using C function macros.
You can turn any structure into any container offered here, you just have to add the given fields and **#include** 'container'.h!
Here is a list of the containers created and what you need to add to your struct to turn it into the given container!
Container name | Fields you need
:--------------|:------------------
array.h        |- *generic** buffer
               |- int32_t capacity
               |- int32_t len
               |
queue.h        |> *generic** buffer
               |> int32_t capacity
               |> int32_t len
               |> int32_t head
               |> int32_t tail

