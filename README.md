![Intro](images/Mandelbrot-Set.png)
# Mandelbrot Set

The Mandelbrot set is one of the most popular fractals which is described by the equation ($z_{n+1} = z_n^2 + c$) starting at z = 0. You can even find tiny "mini-copies" of the entire set hidden deep within its world. You just gotta explore...


## Prerequirements:

You will need to install **SDL3** so you can run it. I build it from source so the latest version will do.<br>
You will also need a compiler like **GCC**.

## Compile and Run:
```bash
gcc -o mandelbrot mandelbrot.c -lSDL3 -lm -fopenmp
SDL_VIDEO_DRIVER=x11 ./mandelbrot
```

## Controls: 

**Left-click** -> Check the current point if its in the mandelbrot set or not.<br>
**Right-click** -> Zoom in <br> <br>
KeyBinds: <br>
**L** -> Lava Colors <br>
**B** -> Ocean Colors <br>
**G** -> GreyScale <br>

## Pictures:

<p align="center">
  <img src="images/Screenshot From 2026-03-16 12-27-38.png" width="45%" style="vertical-align:middle"/>
  <span style="border-left: 1px solid #555; height: 300px; margin: 0 10px; display: inline-block; vertical-align: middle;"></span>
  <img src="images/Screenshot From 2026-03-16 12-29-41.png" width="45%" style="vertical-align:middle" />
</p>
<p align="center">
  <img src="images/Screenshot From 2026-03-16 12-28-56.png" width="45%" style="vertical-align:middle"/>
  <span style="border-left: 1px solid #555; height: 300px; margin: 0 10px; display: inline-block; vertical-align: middle;"></span>
  <img src="images/Screenshot From 2026-03-16 12-29-14.png" width="45%" style="vertical-align:middle" />
</p>

##

//to open the markdown preview in vs code CNTR+shift+V