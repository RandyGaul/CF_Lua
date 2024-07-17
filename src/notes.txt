~ Lua bindings notes for CF and Box2D ~

Arrays - When calling a C function and passing arrays over to C do *not* send in the length of the array. The array
length gets automatically handled by the binding system.

Callbacks - Passing callbacks to C is always done by sending a string of the function name in Lua, *not* the function itself.

Math types are all flattened. Each type has no keys, and is just a bunch of values. For example 2d vectors (b2Vec2 and v2) are flattened into two floats. If we call a function in C that accepts some vectors, we must pass in each float explicitly from Lua. Example:

```lua
draw_push_color(r,g,b,a)
-- as opposed to
color = {r=r,g=g,b=b,a=a}
draw_push_color(color)
```

Otherwise there aren't too many quirks, and 99% of functions match 1:1 with the C function signatures.


~ Binding New Stuff ~

You have CF and Box2D bindings as examples to follow if you ever want to add new stuff to Lua from C. This includes any other C library. It's possible to bind C++ libraries as well, but, they need to either written in C-style or wrapped in a C API. C++ APIs themselves are notoriously overcomplicated and don't map well to other languages. For this reason I recommend to stick just to APIs that expose simple structs/pointers/handles.


~ Building ~

I'm using latest version of VS2022. Unfortunately older versions won't work because the Box2D authored their C API with the absolute newest features from C, meaning older versions of MSVC just won't compile. This is the _Atomic keyword. Just download/update to latest and it will compile.

To compile with CMake it's easiest to just use msvc2202.cmd from the command line, and then look into build_msv_c2022 folder to open CF_Lua.sln. This should automatically fetch the latest Box2D version and CF version and build it all together. From there it's just a matter of messing around in main.lua.

I haven't made a elegant or well-written main.lua file yet. I'm just testing stuff out on a low-level, but it does show drawing sprites, polylines, making some rigid bodies and static geometry, and I did hook up debug drawing to Box2D.

To get going I'd recommend consider wrapping CF/Box2D functions inside your own Lua wrappers. This would be a good way to tweak the API and get in optional arguments and stuff. I'm sure you already have this layer in your own game projects, as I recall when I read your Suika clone page.
