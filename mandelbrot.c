#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <complex.h> 
#include <math.h>


#define height 1500
#define width 2000

#define limit 2.0
#define max_iterations 100

//All the structs and future structs (mallon) here
typedef struct {
    int r;
    int g;
    int b;
} color;


//---------------------------All the functions here---------------------------//

unsigned check_mandelbrot_point(double c_real, double c_imag, int max_iteration) {
    double z_real = 0, z_imag = 0;
    unsigned iterations = 0;
    while (z_real*z_real + z_imag*z_imag <= 2.0*limit && iterations < max_iteration) {
        double next_real = z_real*z_real - z_imag*z_imag + c_real;
        z_imag = 2.0 * z_real * z_imag + c_imag;
        z_real = next_real;
        iterations++;
    }
    return iterations;
}

void draw_mandelbrot(SDL_Renderer *prender, SDL_Texture *ptexture, int *pcurrent_width, int *pcurrent_height, uint32_t *pixelBuffer, double zoom, double center_real, double center_imag, int max_iteration) {
    
    //Original viewing window 2.5 x 2.0 ([-2.0, 0.5]real axis and [-1.0, 1.0]imag axis)
    double range_real = 2.5 / zoom;
    double range_imag = 2.0 / zoom;

    //Calculate the top-left corner of the complex plane based on the center and range
    double min_real = center_real - (range_real / 2.0);
    double min_imag = center_imag - (range_imag / 2.0);


    //this pragma tells the compiler to parallelize the outer loop using OpenMP, 
    //with dynamic scheduling to balance the workload across threads
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < *pcurrent_height; i++) {
        for (int r = 0; r < *pcurrent_width; r++) {
            //Map the pixel (x, y) to complex plane (c_real, c_imag)
            //so we divide by the current width/height to get a normalized [0,1] range
            //then scale to the current view range and add the minimum to shift to the correct location
            double c_real = min_real + ((double)r / *pcurrent_width) * range_real;
            double c_imag = min_imag + ((double)i / *pcurrent_height) * range_imag;

            //Run the Mandelbrot algorithm for this specific point
            unsigned iterations = check_mandelbrot_point(c_real, c_imag, max_iteration);
            
            //Coloring type shit...
            if (iterations == max_iteration) {
                pixelBuffer[i * *pcurrent_width + r] = 0x000000FF; //Black
            } else {
                //Greyscale
                uint8_t color_val = (uint8_t)((double)iterations / max_iteration * 255);
                
                //Bit-wise operations that packs the bits into RGBA8888 (0xRRGGBBAA)
                //dont understand this quite well yet
                pixelBuffer[i * (*pcurrent_width) + r] = (color_val << 24) | (color_val << 16) | (color_val << 8) | 0xFF;
            }
        }
    }

    //Update and present type shit...
    SDL_UpdateTexture(ptexture, NULL, pixelBuffer, *pcurrent_width * sizeof(uint32_t));
    SDL_RenderClear(prender);
    SDL_RenderTexture(prender, ptexture, NULL, NULL);
    SDL_RenderPresent(prender);
}

/*
 * `pixelBuffer` is dereferenced when the drawing dimensions change, so we
 * actually need a pointer-to-pointer here.  The caller passes `&pixelBuffer`
 * and we reallocate/storage when the zoom changes.
 */
void events(int *prunning, SDL_Renderer *prender, SDL_Texture *ptexture, int *pcurrent_width, int *pcurrent_height, float *pzoom, uint32_t **ppixelBuffer, double *pcenter_real, double *pcenter_imag, int *pmax_iterations) {
    uint32_t *pixelBuffer = *ppixelBuffer;
    SDL_Event event;    
    /* Accumulate mouse-wheel changes and redraw once after processing all events */
    int wheel_scrolled = 0;
    float wheel_factor = 1.0f;
    int times = 0; // count wheel events in this batch

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                *prunning = 0;
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                /* if (event.wheel.direction == SDL_MOUSEWHEEL_NORMAL) {
                    event.wheel.x *= 0.9;
                    event.wheel.y *= 0.9;
                } */

                if (event.wheel.y != 0) {
                    times++;
                    float factor = (event.wheel.y > 0) ? 0.9f : 1.1f; // scroll up -> zoom in
                    /* accumulate multiplicative factor, handle multiple notches */
                    wheel_scrolled = 1;
                    wheel_factor *= powf(factor, (float)times);
                }
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if(event.button.button == SDL_BUTTON_LEFT) {
                    //getting the position of the mouse
                    double clickX = event.button.x;
                    double clickY = event.button.y;
                    printf("Mouse clicked at: (%f, %f)\n", clickX, clickY);

                    //Metatrpoume raw pixel coordinates se normalized range [0,1]
                    double normalized_real = clickX / (double) *pcurrent_width;
                    double normalized_im = clickY / (double) *pcurrent_height;
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
                    float prev_cords[max_iterations][2] = {{clickX, clickY}};

                    for (int i = 0; i < max_iterations; i++) {
                        z = cpow(z, 2) + c;

                        //apo normalized kai scaled metatrepoume piso se raw pixel values
                        clickX = (creal(z) + 2.0)/2.5 * (double)*pcurrent_width;
                        clickY = (cimag(z) + 1.0)/2.0 * (double)*pcurrent_height;
                        
                        //if it exceeds the limit breaks the loop
                        if(cabs(z) > limit) {
                            break;
                        }
                        
                        printf("Drawed between cords: (%f, %f) -> (%f, %f)\n", prev_cords[i][0], prev_cords[i][1], clickX, clickY);
                        //assings the cords to the 2-dimentional array
                        if(i < max_iterations-1) {
                            prev_cords[i+1][0] = clickX;
                            prev_cords[i+1][1] = clickY;
                        }
                        SDL_SetRenderDrawColor(prender, 255, 0, 0, 255); //rgba
                    }
                    //draw and visualize the lines
                    SDL_RenderLines(prender, (const SDL_FPoint *)prev_cords, max_iterations);
                    SDL_Delay(1); // Delay to visualize the line drawing
                    SDL_RenderPresent(prender);
                    
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    //Getting the mouse position on click
                    int clickX = event.button.x;
                    int clickY = event.button.y;

                    //normalizing the cords [0,1] range
                    double normalized_real = (double)clickX / (double)(*pcurrent_width);
                    double normalized_imag = (double)clickY / (double)(*pcurrent_height);
                    
                    //doing the same as before but with the current zoom and center
                    double range_real = 2.5 / *pzoom;
                    double range_imag = 2.0 / *pzoom;

                    //dividing by 2
                    double min_real = *pcenter_real - (range_real / 2.0);
                    double min_imag = *pcenter_imag - (range_imag / 2.0);
                    
                    //Calculate exact clicked coordinate on the complex plane
                    double real = min_real + (normalized_real * range_real);
                    double imag = min_imag + (normalized_imag * range_imag);
                    
                    //Set new center to exactly where the user clicked
                    *pcenter_real = real;
                    *pcenter_imag = imag;
                    
                    //zoom in by a factor of 2.0 (adjustable)
                    *pzoom *= 2.0f;
                    //Increase max iterations based on zoom level to reveal more detail
                    *pmax_iterations = 100 + (int)(log10(*pzoom) * 300);
                    
                    SDL_RenderClear(prender);
                    draw_mandelbrot(prender, ptexture, pcurrent_width, pcurrent_height, *ppixelBuffer, *pzoom, *pcenter_real, *pcenter_imag, *pmax_iterations);
                }
                break;
        }
    }
    //If any wheel events occurred, apply accumulated factor and redraw once
    /* if (wheel_scrolled) {
        *pzoom *= wheel_factor;
        int new_w = (int)fmax(1.0f, fmin(width, (*pcurrent_width) * wheel_factor));
        int new_h = (int)fmax(1.0f, fmin(height, (*pcurrent_height) * wheel_factor));
        *pcurrent_width = new_w;
        *pcurrent_height = new_h;

        free(pixelBuffer);
        pixelBuffer = malloc(new_w * new_h * sizeof *pixelBuffer);
        if (!pixelBuffer) {
            SDL_Log("out of memory allocating pixel buffer");
            *prunning = 0;
            return;
        }
        *ppixelBuffer = pixelBuffer;

        SDL_DestroyTexture(ptexture);
        ptexture = SDL_CreateTexture(prender, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, *pcurrent_width, *pcurrent_height);

        int len = new_w * new_h;
        for (int i = 0; i < len; ++i) pixelBuffer[i] = 0;

        draw_mandelbrot(prender, ptexture, pcurrent_width, pcurrent_height, pixelBuffer);
    } */
}

int main() {
    printf("Running...\n");

    int current_height = height;
    int current_width = width;

    float zoom = 1.0f;
    double center_real = -0.7;
    double center_imag = 0.0;
    int max_iteration = 128;
    
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        //to eida apo enan roso kai eipe einai kalo to perror
        perror("Error");
    }

    //sdl_window is a struct thats why the "weird" syntax
    SDL_Window *pwindow;
    pwindow = SDL_CreateWindow("", current_width, current_height, SDL_WINDOW_RESIZABLE);
    if(!pwindow) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
    }

    SDL_Renderer *prender;
    prender = SDL_CreateRenderer(pwindow, NULL);
    if(!prender) {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
    } else {
        SDL_Log("Renderer: %s", SDL_GetRendererName(prender));
        for (int i = 0; i < SDL_GetNumRenderDrivers(); i++) {
            SDL_Log("%d. %s", i+1, SDL_GetRenderDriver(i));
        }
    }
    //SDL_SetRenderLogicalPresentation(prender, width, height, SDL_LOGICAL_PRESENTATION_STRETCH);

    SDL_Texture *ptexture = SDL_CreateTexture(prender, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, current_width, current_height);
    uint32_t *pixelBuffer = malloc(current_width * current_height * sizeof(uint32_t));

    draw_mandelbrot(prender, ptexture, &current_width, &current_height, pixelBuffer, zoom, center_real, center_imag, max_iteration);
    printf("Succesfully created the mandelbrot set\n");
    
    int running = 1;
    unsigned frames = 0;
    Uint64 previousTime = 0;
    char fps_text[30];

    while(running) {
        Uint64 currentTime = SDL_GetTicks();

        //runs the events (mouse, keyboard, etc)
        events(&running, prender, ptexture, &current_width, &current_height, &zoom, &pixelBuffer, &center_real, &center_imag, &max_iteration);        
        
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
    printf("Destroy everything Maki...except uncle mike...he always brought us sweets\n");
    free(pixelBuffer);
    SDL_DestroyTexture(ptexture);
    SDL_DestroyRenderer(prender);
    SDL_DestroyWindow(pwindow);
    SDL_Quit();
    return 0;
}