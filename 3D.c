#include <SDL2/SDL.h>
#include <math.h>

int FOV = 90;
int distance = 10;
int windowWidth = 1280;
int windowHeight = 700;
int running = 1;
SDL_Event event;

// Structure to store 3-dimensional points
typedef struct point3D
{
    double x;
    double y;
    double z;
} point3D;

// Structure to store 2-dimensional points
typedef struct point2D
{
    double x;
    double y;
} point2D;

// Structure to store a face of a cube
typedef struct face
{
    struct point3D a, b, c, d;
} face;

// Method to projecta 3-demensional point onto a 2-dimensional plane, returning the 2-dimensional point
point2D project(point3D p)
{
    // Obtain FOV in radians and determine projection factor
    double fov_rad = (FOV * M_PI) / 180;
    double factor = 1 / tan(fov_rad / 2);
    
    // Remove division by zero
    if (p.z == 0) p.z = 0.001;

    // Project points using projection factor and projection formula
    point2D projected;
    projected.x = p.x / p.z * factor * windowWidth + windowWidth / 2;
    projected.y = p.y / p.z * factor * windowWidth + windowHeight / 2;

    // Return the projected point 
    return projected;
}

// Method to rotate three dimensional points about the X axis 
void rotateX(point3D *p, double theta)
{
    p->z -= 11;

    double newY = p->y * cos(theta) - p->z * sin(theta);
    double newZ = p->y * sin(theta) + p->z * cos(theta);

    p->y = newY;
    p->z = newZ + 11;
}

// Method to rotate three dimensional points about the Y axis 
void rotateY(point3D *p, double theta)
{
    p->z -= 11;

    double newX = p->x * cos(theta) + p->z * sin(theta);
    double newZ = -1 * (p->x * sin(theta)) + p->z * cos(theta);

    p->x = newX;
    p->z = newZ + 11;
}

int main()
{
    // Initialize system
    SDL_Init(SDL_INIT_VIDEO);

    // Create window
    SDL_Window *window = SDL_CreateWindow("Cube3D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
    windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    SDL_SetWindowResizable(window, SDL_TRUE);

    // Grab renderer 
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Initialize points
    point3D p1 = {-1, -1, 0 + distance};
    point3D p2 = {1, -1, 0 + distance};
    point3D p3 = {1, 1, 0 + distance};
    point3D p4 = {-1, 1, 0 + distance};
    point3D p5 = {-1, -1, 2 + distance};
    point3D p6 = {1, -1, 2 + distance};
    point3D p7 = {1, 1, 2 + distance};
    point3D p8 = {-1, 1, 2 + distance};

    // Initialize array of face struc holding all necessary cube faces
    face faces[4] = {
        {p1, p2, p3, p4}, 
        {p5, p6, p7, p8}, 
        {p1, p4, p8, p5}, 
        {p2, p3, p7, p6}
    };

    // Render loop
    while(running)
    {
        SDL_PollEvent(&event);

        // If window is closed, end render loop
        if (event.type == SDL_QUIT)
        {
            running = 0;
        }

        if (event.type == SDL_WINDOWEVENT)
        {
            // If window is resized, re-initialize windowWidth and windowHeight
            if (event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                SDL_GetWindowSize(window, &windowWidth, &windowHeight);
            }
        }

        // If mouse is moved within the window, rotate points in directions x and y accordingly
        if (event.type == SDL_MOUSEMOTION)
        {
            double dx = event.motion.xrel;
            double dy = event.motion.yrel;
            double rotationX = -1 * dx * 2 * M_PI / 1000;
            double rotationY = dy * 2 * M_PI / 1000;
            for (int i = 0; i < 4; i++)
            {
                rotateY(&faces[i].a, rotationX);
                rotateY(&faces[i].b, rotationX);
                rotateY(&faces[i].c, rotationX);
                rotateY(&faces[i].d, rotationX);

                rotateX(&faces[i].a, rotationY);
                rotateX(&faces[i].b, rotationY);
                rotateX(&faces[i].c, rotationY);
                rotateX(&faces[i].d, rotationY);

            }
        }
        // Prepare for next frame by clearing previous render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw lines connecting points in every face 
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

        // Display newest render
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    // Close everything before shutting down
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}