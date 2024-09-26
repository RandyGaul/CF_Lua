# Lua Bindings for CF and Box2D

This repo provides bindings to Lua for [CF](https://github.com/RandyGaul/cute_framework) and Box2D. An executable file is generated called `CF_Lua` (feel free to rename this to whatever you like). You pass in a path to your `main.lua` file as the only command line parameter. The executable will start up your Lua script and call the `main` function. From there it's just a matter of calling into CF functions from Lua. All of the functions/constants are bound in `wrap_cf.cpp`, and you may view all bound functions/constants by calling `dump_lua_api` from within Lua.

Make sure to do `git submodule update --init --recursive` once you clone the repo.

# Special Cases

Arrays - When calling a C function and passing arrays over to C, do *NOT* send in the length of the array. The array length gets automatically handled by the binding system.

Callbacks - Passing callbacks to C is done by sending a *string of the function name* in Lua, not the function itself. Example for fixed updates:

```lua
function update()
   -- do my fixed update here
end

function draw() end

while app_is_running() do
    app_update("update")
    draw()
end
```

Math types are all flattened. Each type has no keys, and is just a bunch of values. For example, 2d vectors (b2Vec2 and v2) are flattened into two floats. If we call a function in C that accepts some vectors, we must pass in each float explicitly from Lua. Example:

```lua
draw_push_color(r,g,b,a)
-- as opposed to
color = {r=r,g=g,b=b,a=a}
draw_push_color(color)
```

Otherwise there aren't too many quirks, and 99% of functions match 1:1 with the C function signatures.

Smalltalk made a cool hotreloader, you can check it out here: https://gist.github.com/waldnercharles/a3e314afccfac40ce4fbf42b83093c54


# Binding New Stuff

You have CF and Box2D bindings as examples to follow if you ever want to add new stuff to Lua from C. This includes any other C library. It's possible to bind C++ libraries as well, but they need to either written in C-style or wrapped in a C API.

C++ APIs themselves are notoriously overcomplicated and don't map well to other languages. For this reason, I recommend to only stick to APIs that expose simple structs/pointers/handles. Bind.h has a ton of notes at the top of the header about how to bind and expose new things to Lua.


# Building

I'm using latest version of VS2022. Unfortunately older versions won't work because Box2D authored their C API with the absolute newest features from C, meaning older versions of MSVC just won't compile. This is the _Atomic keyword. Just download/update to the latest version of MSVC and it will compile.

To compile with CMake it's easiest to just use msvc2202.cmd from the command line, and then look into build_msv_c2022 folder to open CF_Lua.sln. CMake should automatically fetch the latest Box2D and CF versions and then build it all together. From there, it's just a matter of messing around in main.lua.

I haven't made an elegant or well-written main.lua file yet. I'm just testing stuff out at low-level, but it does show drawing sprites, polylines, making some rigid bodies and static geometry. And I did also hook up debug drawing to Box2D.

To get going, I'd recommend to consider wrapping the CF/Box2D Lua functions inside your own Lua wrappers. This would be a good way to tweak the API and get in optional arguments and stuff.
