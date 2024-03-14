I know what the title says but that is too ambitious of a project. I lack both the skill and the time.


## Software Rasterizer

I render to a pixel buffer and then blit it to the window's surface. I use sdl2 for the platform layer and
miniaudio for audio, which is a embedded inside `vendor/`. I don't actually rasterize yet. I am a deceitful cheat, among other things.

I am using it to make a game for the Acerola Game Jam.

## compile

**SDL2 is required**

If you are on windows, the cmakelists uses the SDL2 provided inside vendor (registered as a submodule). If you are on linux or mac, it looks for SDL inside your lib directories.
This also means you don't need to download the submodules if you're not on windows.

This will be the case until michaelsoft bindows decides to make a default path for installing libs (30 something major releases, 0 something good ideas)

cmake file is very simple. Literally just do `cmake ..` inside a build directory, no extra flags needed.
```
//Inside the root directory
mdkir build
cd build
cmake ..
```

I personally use ninja + clang, so I do this
```
mdkir build
cd build
cmake .. -DCMAKE_C_COMPILER=clang -G "Ninja"
ninja
```

*Note:* I have not packaged the resources as of now (since they do not belong to me), so you will have to do that yourself. Make a `res/GameAlert.wav` and a `res/song0.wav` in the root directory. Allowing failed
resource loading and fallback options sounds like a good thing to work on.

## contributing

Open an issue before you decide to work on it. I am unsure what one would want to add to this, since even I haven't been very clear about this engine's purpose, but please feel free to do whatever. Doing audio properly and text come to mind. I am probably working on them already so open an issue first.

<!-- //ctrl shift v (my vscode md viewer plugin keybind) -->