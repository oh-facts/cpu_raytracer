I will update this properly later.

## cpu ray tracer

This will be a voxel ray tracer I think. Software rendered. 


**SDL2 is required**

If you are on windows, the cmake uses the SDL2 provided inside vendor (registered as a submodule). If you are on linux or mac, it looks for SDL inside your directories.
This also means you don't need to download the submodules if you're not on windows.

This will be the case until michaelsoft bindows decides to make a default path for installing libs.

## compile

cmake file is very simple. Literally just do `cmake ..` inside a build directory, no extra flags needed. I personally use ninja + clang
```
mdkir build
cd build
cmake ..
```

## contributing

No point atm. this project is in its infancy. Even I don't know what I am doing. When this becomes closer to something sensible, then I'd suggest contributing if you want to.
