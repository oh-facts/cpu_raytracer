I know what the title says but that is too ambitious of a project. I lack both the skill and the time.


## Software Rasterizer

I render to a pixel buffer and then blit it to the window's surface. I use sdl2 for the platform layer and
miniaudio for audio, which is a embedded inside `vendor/`. I don't actually rasterize yet. I am a deceitful cheat, among other things.

I am using it to make a game for the Acerola Game Jam.

## compile

All dependencies are inside vendor/
They must be downloaded since SDL2 is making my life hard and SDL2_ttf's cmake is making my life harder. Single header libraries are making me hard and I wish every library used a tool to generate a single header library to ship with.

You will have to clone the repo like this -

```
git clone --recursive https://github.com/oh-facts/cpu_raytracer.git
```

If you have already cloned this repo. Then do this -

```
git pull
git submodule update --init --recursive
```

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