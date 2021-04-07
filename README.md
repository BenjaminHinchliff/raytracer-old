# Raytracer

Got sidetracked on another project and I've been meaning to build one of these for a while. It's also a good opportunity to try to make a project in C that
doesn't end up as a massive mess. I've taken a shot at doing that here, if you have the knowledge, feel free to let me know about improvments (raise an issue).

It's modeled after this [tutorial](https://bheisler.github.io/post/writing-raytracer-in-rust-part-1/), except in C instead of Rust, obviously.

It depends on GSL for vector math stuff, libpng for image output, and json-c for reading scene data. GSL and libpng must be installed, json-c will
be fetched from source if not installed. After that just use cmake to build the library and run the tests with ctest. I'll add more detailed instructions
whenever I get to it.

I'll also add a cli at one point, but right now it's just a library. Take a look at the tests if you want to use it for whatever reason.
