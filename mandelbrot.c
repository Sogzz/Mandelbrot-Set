#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <complex.h> 
#include <time.h>


#define height 650
#define width 900

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
char* rand_color(char hex[8]) {
    color my_color = {rand() % 256, rand() % 256, rand() % 256};

    //way to convert and store the random color in hex from rgb format
    snprintf(hex, sizeof(&hex) + 2, "0x%02x%02x%02x", my_color.r, my_color.g, my_color.b);
    printf("Random color: Red = %d, Green = %d, Blue = %d\n", my_color.r, my_color.g, my_color.b);
    printf("Hexadecimal representation: %s\n", hex);

    return hex;
}

int check_mandelbrot(double real, double imag) {
    //scaling and resizing the real & imaginary values
    //prepei na to allaxw gia na exei ta values me to width kai height
    real = real * 3.0 - 2.0;
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
    unsigned int hex_value;
    char COLLOR_RANDOM[8];

    rand_color(COLLOR_RANDOM);
    //auth h sscan kanei kati malakies kai diavazei apo string kai kanei oti thes
    sscanf(COLLOR_RANDOM, "%x", &hex_value);

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
                
                unsigned int color_value = (red << 16) | (green << 8) | blue;
                SDL_Rect pixel = {r,i,1,1};
                SDL_FillSurfaceRect(psurface, &pixel, color_value);
            }
        }
    }
}

int main() {
    printf("Running...\n");
    srand(time(NULL));

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        perror("Error");
    }

    //sdl_window is a struct thats why the "weird" syntax
    SDL_Window *pwindow;
    pwindow = SDL_CreateWindow("Mandelbrot Set", width, height, SDL_WINDOW_RESIZABLE);

    SDL_Surface *psurface;
    psurface = SDL_GetWindowSurface(pwindow);
    
    draw_mandelbrot(psurface);
    SDL_UpdateWindowSurface(pwindow);


    //event handler
    int running = 1;
    SDL_Event event;
    while(running) {
        while (SDL_PollEvent(&event)) {

            switch (event.type) {
                //future zoom edw 
                case SDL_EVENT_QUIT:
                    running = 0;
                    break;
            }
        }
    }
    SDL_Quit();
    return 0;
}