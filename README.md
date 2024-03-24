I know what the title says but that is too ambitious of a project. I lack both the skill and the time.

## Software Rasterizer

I render to a pixel buffer and then blit it to the window's surface. I use sdl2 for the platform layer and
miniaudio for audio. Both are inside `vendor/`.

## compile

All dependencies are inside `vendor/`. sdl2 is added as a submodule. On linux and apple, the default directory is checked. This means you don't need to download submodules if you're on those OS.

You will have to clone the repo like this -

```
// for windows
git clone --recursive https://github.com/oh-facts/cpu_raytracer.git

// for linux/apple
git clone https://github.com/oh-facts/cpu_raytracer.git

```

If you're on windows and you've already cloned the repo. Do this -

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

Good job! You are so cool! Now, download this [drive link](https://drive.google.com/drive/folders/1M4K7Ur9gShpLSmHQQxYbpiguzK3a7oSH?usp=sharing). It contains the resource folder. Unzip it and place it in the root directory. So,

```
root/
├─ src/
├─ res/
│  ├─ *.wav
│  ├─ *.bmp
│  ...
│
├─ README.md
...
```
Now you should be able to run the binary. (If you don't put the resources, it will segfault because of assert that requires that you have the resources)

## contributing

Open an issue before you decide to work on it. I am unsure what one would want to add to this, since even I haven't been very clear about this engine's purpose, but please feel free to do whatever. Doing audio properly and text come to mind. I am probably working on them already so open an issue first.

<!-- //ctrl shift v (my vscode md viewer plugin keybind) -->