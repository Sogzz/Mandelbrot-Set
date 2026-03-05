#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <complex.h> 


#define height 1500
#define width 2000

#define COLOR_WHITE 0xffffff
#define limit 2
#define max_iterations 100

//All the structs and future structs (mallon) here
typedef struct {
    int r;
    int g;
    int b;
} color;


//---------------------------All the functions here---------------------------//

int check_mandelbrot(double real, double imag) {
    //scaling and resizing the real & imaginary values
    //prepei na to allaxw gia na exei ta values me to width kai height
    real = real * 2.5 - 2.0;
    imag = imag * 2.0 - 1.0;
    
    double _Complex c = real + imag*I;
    double _Complex z = 0;

    for (int i = 0; i < max_iterations; i++) {
        z = cpow(z, 2) + c;
        if (cabs(z) > limit) {
            //returns ta iterations gia to xrwma
            return i;
        }
    }
    return max_iterations;
}

void draw_mandelbrot(SDL_Surface *psurface) {
    for (int r = 0; r < width; r++) {
        for (int i = 0; i < height; i++) {
            unsigned mandelbrot_color = check_mandelbrot((double) r/width, (double) i/height);
            
            if(mandelbrot_color == max_iterations) {
                SDL_Rect pixel = {r,i,1,1};
                SDL_FillSurfaceRect(psurface, &pixel, COLOR_WHITE);
            } else if (mandelbrot_color < max_iterations && mandelbrot_color > 0) {

                //Create colors based on the number of iterations (grey scale)
                unsigned red = (mandelbrot_color * 255) / max_iterations;
                unsigned green = (mandelbrot_color * 255) / max_iterations;
                unsigned blue = (mandelbrot_color * 255) / max_iterations;

                //cant understand this color value yet...
                unsigned int color_value = (red << 16) | (green << 8) | blue;
                SDL_Rect pixel = {r,i,1,1};
                SDL_FillSurfaceRect(psurface, &pixel, color_value);
            }
        }
    }
}

void events(int *prunning) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                *prunning = 0;
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                //getting the directions of the mouse
                float mouseX = event.wheel.mouse_x;
                float mouseY = event.wheel.mouse_y;

                //creating the zoom
                break;
        }
    }
}

int main() {
    printf("Running...\n");
    
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        perror("Error");
    }

    //sdl_window is a struct thats why the "weird" syntax
    SDL_Window *pwindow;
    pwindow = SDL_CreateWindow("", width, height, SDL_WINDOW_RESIZABLE);

    SDL_Surface *psurface;
    psurface = SDL_GetWindowSurface(pwindow);

    SDL_Renderer *prender;
    prender = SDL_CreateRenderer(pwindow, NULL);
    SDL_SetRenderLogicalPresentation(prender, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    
    
    draw_mandelbrot(psurface);
    SDL_UpdateWindowSurface(pwindow);
    printf("Succesfully created the mandelbrot set\n");
    
    int running = 1;

    unsigned frames = 0;
    Uint64 previousTime = 0;
    char fps_text[30];

    SDL_Event event;
    while(running) {
        Uint64 currentTime = SDL_GetTicks();

        //runs the events (mouse, keyboard, etc)
        events(&running);

        
        
        //fps counter
        frames++;        
        //metraei se miliseconds
        if (currentTime >= previousTime + 1000) {
            //vazei to Uint se array gt to sdl gamietai kai thelei array sto window title
            snprintf(fps_text, sizeof(fps_text), "Mandelbrot - FPS: %u", frames);
            SDL_SetWindowTitle(pwindow, fps_text);
            
            if (!SDL_SetWindowTitle(pwindow, fps_text) || !SDL_SyncWindow(pwindow)) {
                SDL_Log("SDL_SetWindowTitle failed: %s", SDL_GetError());
            }
            frames = 0;
            previousTime = currentTime;
        }
    }
    SDL_Quit();
    return 0;
}