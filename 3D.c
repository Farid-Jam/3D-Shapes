#include <SDL2/SDL.h>
#include <math.h>

int FOV = 90;
int distance = 10;
int windowWidth;
int windowHeight;

typedef struct point3D
{
    double x;
    double y;
    double z;
} point3D;

typedef struct point2D
{
    double x;
    double y;
} point2D;

typedef struct face
{
    struct point3D a, b, c, d;
} face;

point2D project(point3D p)
{
    double fov_rad = (FOV * M_PI) / 180;
    double factor = 1/tan(fov_rad / 2);
    
    if (p.z == 0) p.z = 0.001;

    point2D projected;
    projected.x = p.x / p.z * factor * windowWidth + windowWidth / 2;
    projected.y = p.y / p.z * factor * windowWidth + windowHeight / 2;

    return projected;
}

int main()
{
    SDL_InitSubSystem(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Shapes3D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
    1280, 700, SDL_WINDOW_SHOWN);
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    int running = 1;
    SDL_Event event;
    point3D p1 = {-1, -1, 0 + distance};
    point3D p2 = {1, -1, 0 + distance};
    point3D p3 = {1, 1, 0 + distance};
    point3D p4 = {-1, 1, 0 + distance};
    point3D p5 = {-1, -1, 1 + distance};
    point3D p6 = {1, -1, 1 + distance};
    point3D p7 = {1, 1, 1 + distance};
    point3D p8 = {-1, 1, 1 + distance};

    face faces[4] = {
        {p1, p2, p3, p4}, 
        {p5, p6, p7, p8}, 
        {p1, p4, p8, p5}, 
        {p2, p3, p7, p6}
    };


    while(running)
    {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT)
        {
            running = 0;
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int i = 0; i < 4; i++)
        {
            point2D v1 = project(faces[i].a);
            point2D v2 = project(faces[i].b);
            point2D v3 = project(faces[i].c);
            point2D v4 = project(faces[i].d);
            SDL_RenderDrawLine(renderer, v1.x, v1.y, v2.x, v2.y);
            SDL_RenderDrawLine(renderer, v2.x, v2.y, v3.x, v3.y);
            SDL_RenderDrawLine(renderer, v3.x, v3.y, v4.x, v4.y);
            SDL_RenderDrawLine(renderer, v4.x, v4.y, v1.x, v1.y);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
}