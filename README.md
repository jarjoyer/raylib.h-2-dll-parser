I needed a workaround for using raylib in a dll, so I did these macros by hand, until i decided to write this parser.

This c program takes raylib.h and spits out list_of_raylib_functions.h - marcos to use through a function pointer struct across the translation units, giving the opportunity of running raylib in a dll.

To get list_of_raylib_functions.h simply compile & run main.c with raylib.
