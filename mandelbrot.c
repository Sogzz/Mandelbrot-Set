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

void events(int *prunning, SDL_Renderer *prender) {
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
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                //getting the position of the mouse
                double clickX = event.button.x;
                double clickY = event.button.y;
                printf("Mouse clicked at: (%f, %f)\n", clickX, clickY);

                //Metatrpoume raw pixel coordinates se normalized range [0,1]
                double normalized_real = clickX / (double)width;
                double normalized_im = clickY / (double)height;
                printf("Normalized cords: (%f, %f)\n", normalized_real, normalized_im);

                //apo normalized sto scale tou mandelbrot pou evala
                //mporei na ta kanw define pio panw gia poio eukolh allagh
                double real = normalized_real * 2.5 - 2.0;
                double imag = normalized_im * 2.0 - 1.0;
                printf("Mandelbroted cords: (%f, %f)\n\n", real, imag);

                //xekinaei h sunartisi mandelbrot apo auto to shmeio
                double _Complex c = real + imag*I;
                double _Complex z = 0;

                //Track previous point so we can draw a line
                //mporei kai na ginei pinakas auto sto mellon
                double prev_cords[max_iterations][2] = {{clickX, clickY}};

                for (int i = 0; i < max_iterations; i++) {
                    z = cpow(z, 2) + c;

                    //apo normalized kai scaled metatrepoume piso se raw pixel values
                    clickX = (creal(z) + 2.0)/2.5 * (double)width; 
                    clickY = (cimag(z) + 1.0)/2.0 * (double)height;
                    
                    //if it exceeds the limit breaks the loop
                    if(cabs(z) > limit) {
                        break;
                    }
                    
                    printf("Drawed between cords: (%f, %f) -> (%f, %f)\n", prev_cords[i][0], prev_cords[i][1], clickX, clickY);

                    if(i < max_iterations-1) {
                        prev_cords[i+1][0] = clickX;
                        prev_cords[i+1][1] = clickY;
                    }
                }
                SDL_SetRenderDrawColor(prender, 255, 0, 0, 255); //rgba
                SDL_RenderLines(prender, (const SDL_FPoint *)prev_cords, max_iterations);
                SDL_RenderPresent(prender);

                SDL_Delay(1); // Delay to visualize the line drawing
                break;
        }
    }
}

int main() {
    printf("Running...\n");
    
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        //to eida apo enan roso kai eipe einai kalo to perror
        perror("Error");
    }

    //sdl_window is a struct thats why the "weird" syntax
    SDL_Window *pwindow;
    pwindow = SDL_CreateWindow("", width, height, SDL_WINDOW_RESIZABLE);

    SDL_Surface *psurface;
    psurface = SDL_GetWindowSurface(pwindow);

    SDL_Renderer *prender;
    prender = SDL_CreateRenderer(pwindow, NULL);
    if(!prender) {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
    }
    SDL_SetRenderLogicalPresentation(prender, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    
    
    draw_mandelbrot(psurface);
    SDL_UpdateWindowSurface(pwindow);
    printf("Succesfully created the mandelbrot set\n");
    
    int running = 1;

    unsigned frames = 0;
    Uint64 previousTime = 0;
    char fps_text[30];

    while(running) {
        Uint64 currentTime = SDL_GetTicks();

        //runs the events (mouse, keyboard, etc)
        events(&running, prender);        
        
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