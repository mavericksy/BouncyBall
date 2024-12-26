#include <SDL2/SDL.h>
#include <stdio.h>

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 600
#define OFFSET_COORD_X SCREEN_WIDTH / 2
#define OFFSET_COORD_Y SCREEN_HEIGHT / 2

#define COLOUR_WHITE 0xffffffff
#define COLOUR_BLACK 0x00000000
#define COLOUR_LIGHT 0xC7F0D8
#define COLOUR_DARK 0x43523D

#define GRAVITY 0.2
#define DAMPENING 0.8
#define TRAJ_LENGTH 99
#define TRAJ_WIDTH 8

typedef struct Circle {
  double x, y, radius;
  double v_x, v_y;
} Circle;

void FillCircle(SDL_Surface *surface, struct Circle circle, Uint32 colour) {
  //
  double low_x = circle.x - circle.radius;
  double low_y = circle.y - circle.radius;
  double high_x = circle.x + circle.radius;
  double high_y = circle.y + circle.radius;
  double radsq = circle.radius * circle.radius;
  //
  for (double x = low_x; x < high_x; x++) {
    for (double y = low_y; y < high_y; y++) {
      double x_off = (x - circle.x);
      double y_off = (y - circle.y);
      double center_dist_sq = x_off * x_off + y_off * y_off;
      if (center_dist_sq < radsq) {
        SDL_Rect pixel = (SDL_Rect){x, y, 1, 1};
        SDL_FillRect(surface, &pixel, colour);
      }
    }
  }
}

void step(struct Circle *circle) {
  //
  circle->x = circle->x + circle->v_x;
  circle->y = circle->y + circle->v_y;
  circle->v_y += GRAVITY;
  //
  if (circle->x + circle->radius > SCREEN_WIDTH) {
    circle->x = SCREEN_WIDTH - circle->radius;
    circle->v_x = -circle->v_x * DAMPENING;
  }
  //
  if (circle->y + circle->radius > SCREEN_HEIGHT) {
    circle->y = SCREEN_HEIGHT - circle->radius;
    circle->v_y = -circle->v_y * DAMPENING;
  }
  //
  if (circle->y - circle->radius < 0) {
    circle->y = circle->radius;
    circle->v_y = -circle->v_y * DAMPENING;
  }
  //
  if (circle->x - circle->radius < 0) {
    circle->x = circle->radius;
    circle->v_x = -circle->v_x * DAMPENING;
  }
}

void FillTrajectory(SDL_Surface *surface,
                    struct Circle trajectory[TRAJ_LENGTH]) {
  //
  for (int32_t i = 0; i < TRAJ_LENGTH; i++) {
    trajectory[i].radius = TRAJ_WIDTH * (double) i / 100.0;
    FillCircle(surface, trajectory[i], COLOUR_DARK);
  }
}

void UpdateTrajectory(struct Circle trajectory[TRAJ_LENGTH],
                      struct Circle circle) {
  //
  // shift left by 1 and append latest circle to end
  struct Circle traj_shifted[TRAJ_LENGTH];
  for (int32_t i = 1; i < TRAJ_LENGTH; i++)
      traj_shifted[i-1] = trajectory[i];
  //
  for(int32_t i = 0;i<TRAJ_LENGTH;i++)
    trajectory[i] = traj_shifted[i];
  //
  trajectory[TRAJ_LENGTH-1] = circle;
}

int main() {
  //
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("ERROR: SDL INIT: %s", SDL_GetError());
    exit(1);
  }
  //
  SDL_Window *window =
      SDL_CreateWindow("Bouncy Ball", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  if (!window) {
    printf("ERROR: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }
  SDL_Surface *surface = SDL_GetWindowSurface(window);
  //
  SDL_Rect back = (SDL_Rect){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
  SDL_Event event;
  //
  struct Circle circle = (struct Circle) {OFFSET_COORD_X, OFFSET_COORD_Y, 30, 25, 25};
  struct Circle circles[TRAJ_LENGTH] = {{0}};
  //
  int32_t run = 1;
  while (run) {
    //
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        run = 0;
      }
    }
    //
    SDL_FillRect(surface, &back, COLOUR_LIGHT);
    //
    FillTrajectory(surface, circles);
    FillCircle(surface, circle, COLOUR_DARK);
    step(&circle);
    //
    UpdateTrajectory(circles, circle);
    //
    SDL_UpdateWindowSurface(window);
    SDL_Delay(20);
  }
  //
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
