![Intro](images/Mandelbrot-Set.png)
# Mandelbrot Set

The Mandelbrot set is one of the most popular fractals which is described by the equation (znext​=z2+c) starting at z = 0. You can even find tiny "mini-copies" of the entire set hidden deep within its world. You just gotta explore...

##
### Compile and Run
```bash
gcc -o mandelbrot mandelbrot.c -lSDL3 -lm -fopenmp
SDL_VIDEO_DRIVER=x11 ./mandelbrot
```
##
### Pictures

| Grayscale | Left-click feature |
| :---: | :---: |
| <img src="images/Screenshot From 2026-03-16 12-27-38.png" width="100%"> | <img src="images/Screenshot From 2026-03-16 12-29-41.png" width="100%"> |

| Zoomed | Mandelbrot |
| :---: | :---: |
| <img src="images/Screenshot From 2026-03-16 12-28-56.png" width="100%"> | <img src="images/Screenshot From 2026-03-16 12-29-14.png" width="100%"> |


