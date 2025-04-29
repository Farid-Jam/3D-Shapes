#include <SDL2/SDL.h>
#include <math.h>

int FOV = 90;
int distance = 11;
int windowWidth = 1280;
int windowHeight = 700;
int running = 1;
SDL_Event event;
int numFaces = 6;
int culling = 1; // 0 to turn off, any other number to turn on

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
    point3D a, b, c, d;
    point3D normal;
    double dot;
} face;

// Method to determine whether the normal of a face points towards the camera 
point3D findNormal(face face)
{
    point3D edge1 = {face.d.x - face.a.x, face.d.y - face.a.y, face.d.z - face.a.z};
    point3D edge2 = {face.b.x - face.a.x, face.b.y - face.a.y, face.b.z - face.a.z};

    point3D normal = {
        (edge1.y * edge2.z) - (edge1.z * edge2.y),
        (edge1.z * edge2.x) - (edge1.x * edge2.z),
        (edge1.x * edge2.y) - (edge1.y * edge2.x)
    };

    return normal;
}

// Method to determine how much a faces normal vector differs from a vector from the camera to the center of a face
double findDot(face face, point3D camPos)
{
    point3D center = {
        ((face.a.x + face.b.x + face.c.x + face.d.x) / 4),
        ((face.a.y + face.b.y + face.c.y + face.d.y) / 4), 
        ((face.a.z + face.b.z + face.c.z + face.d.z) / 4)
    };

    point3D vector = {
        camPos.x - center.x,
        camPos.y - center.y,
        camPos.z - center.z
    };

    double dot = vector.x * face.normal.x + vector.y * face.normal.y + vector.z * face.normal.z;

    return dot;
}

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
    p->z -= distance;

    double newY = p->y * cos(theta) - p->z * sin(theta);
    double newZ = p->y * sin(theta) + p->z * cos(theta);

    p->y = newY;
    p->z = newZ + distance;
}

// Method to rotate three dimensional points about the Y axis 
void rotateY(point3D *p, double theta)
{
    p->z -= distance;

    double newX = p->x * cos(theta) + p->z * sin(theta);
    double newZ = -1 * (p->x * sin(theta)) + p->z * cos(theta);

    p->x = newX;
    p->z = newZ + distance;
}

void fillTriangle(SDL_Renderer *renderer, point2D p1, point2D p2, point2D p3)
{
    if (p1.y > p2.y) {point2D temp = p1; p1 = p2; p2 = temp;}
    if (p2.y > p3.y) {point2D temp = p2; p2 = p3; p3 = temp;}
    if (p1.y > p3.y) {point2D temp = p3; p3 = p1; p1 = temp;}
}

void fillFace(SDL_Renderer *renderer, face f)
{
    point2D v1 = project(f.a);
    point2D v2 = project(f.b);
    point2D v3 = project(f.c);
    point2D v4 = project(f.d);

    fillTriangle(renderer, v1, v2, v3);
    fillTriangle(renderer, v1, v3, v4);
}

int main()
{
    // Initialize system
    SDL_Init(SDL_INIT_VIDEO);

    // Initialize camera position
    point3D camera = {0, 0, 0};

    // Create window
    SDL_Window *window = SDL_CreateWindow("Cube3D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
    windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    SDL_SetWindowResizable(window, SDL_TRUE);

    // Grab renderer 
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Initialize points
    point3D p1 = {-1, -1, -1 + distance};
    point3D p2 = {1, -1, -1 + distance};
    point3D p3 = {1, 1, -1 + distance};
    point3D p4 = {-1, 1, -1 + distance};
    point3D p5 = {-1, -1, 1 + distance};
    point3D p6 = {1, -1, 1 + distance};
    point3D p7 = {1, 1, 1 + distance};
    point3D p8 = {-1, 1, 1 + distance};

    // Initialize array of face struc holding all necessary cube faces
    face faces[6] = {
        {p1, p2, p3, p4}, 
        {p5, p8, p7, p6}, 
        {p1, p4, p8, p5}, 
        {p2, p6, p7, p3},
        {p4, p3, p7, p8},
        {p5, p6, p2, p1}
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
            for (int i = 0; i < numFaces; i++)
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

        // Draw lines connecting points in every visible face
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int i = 0; i < numFaces; i++)
        {
            point2D v1 = project(faces[i].a);
            point2D v2 = project(faces[i].b);
            point2D v3 = project(faces[i].c);
            point2D v4 = project(faces[i].d);
            faces[i].normal = findNormal(faces[i]);
            faces[i].dot = findDot(faces[i], camera);
            if (faces[i].dot > 0|| !culling ){
                SDL_RenderDrawLine(renderer, v1.x, v1.y, v2.x, v2.y);
                SDL_RenderDrawLine(renderer, v2.x, v2.y, v3.x, v3.y);
                SDL_RenderDrawLine(renderer, v3.x, v3.y, v4.x, v4.y);
                SDL_RenderDrawLine(renderer, v4.x, v4.y, v1.x, v1.y);
            }
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