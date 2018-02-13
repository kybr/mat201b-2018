# Using the Voro++ library

Here's an example of using the Voro++ library.

To install the library on macOS, we use Homewbrew:

```bash
brew install homebrew/science/voro++
```

This installs header (.h) files in `/usr/local/include` and a compiled library in `/usr/local/lib`.

Note the file `flags.cmake`. This signals the AlloSystem build script to use the contents of `flags.cmake` to help build. The contents of this file will vary based on the library you want to use.

## Improvements

Go look at what the openFrameworks addon does...

[ofxVoro](https://github.com/patriciogonzalezvivo/ofxVoro/blob/master/src/ofxVoro.cpp)
