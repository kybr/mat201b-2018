# CGAL example

This shows how to use the AlloSystem to build system's `flags.cmake` feature to
build against the CGAL library. Right now this example is only supported for
Mac OS X >10.9.

For this to work, you'll need to install CGAL using homebrew:

    brew install cgal

Note the contents of `flags.cmake`:

    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -I/usr/local/include" )
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/lib -lCGAL")

This tells the AlloSystem build system to look for headers in
`/usr/local/include` and libraries in `/usr/local/lib` and try to link (`-l`)
CGAL.
