#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib.h"
#include <stdlib.h>
#include <string.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_INPUT_CHARS 256

typedef struct {
  int tomatoes;
  int peice;
  int smallest;
  int largest;
  int state;
  int input_step;
  int pomodoro_state;
  int loopC;
  int lapC;
  int countdown;
  bool is_paused;
  bool input_done;
  bool pomodoro_done;
  char input[MAX_INPUT_CHARS];
  const char *prompt;
} Pomodoro;

typedef struct {
  Rectangle rect;
  bool state;
} Button;

Pomodoro P;

bool FINISH = false;

int *make_it_int(char input[]);
int make_it_seconds(int input[]);
// state
void get_user_input();
void pomodoro();

int main(void) {
  P.state = 0;
  P.input_step = 0;
  P.input_done = false;
  P.pomodoro_done = false;
  P.pomodoro_state = 0;
  P.loopC = 0;
  P.lapC = 0;
  P.is_paused = false;
  strcpy(P.input, "");
  P.prompt = "Please input your loop";

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pomodoro Timer");
  SetWindowMinSize(SCREEN_WIDTH, SCREEN_HEIGHT);
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    switch (P.state) {
    case 0: {
      get_user_input();
      if (P.input_done) {
        P.state = 1;
      }
      break;
    }
    case 1: {
      pomodoro();
      if (P.pomodoro_done) {
        P.state = 2;
      }
      break;
    }
    case 2: {
      BeginDrawing();
      ClearBackground(RAYWHITE);
      DrawText("Congrats", (GetScreenWidth() / 2.0f) - 10,
               GetScreenHeight() * 0.5f, 50, RED);
      EndDrawing();
      break;
    }
    }
  }
  CloseWindow();
  return 0;
}

int *make_it_int(char input[]) {
  int *buffer = (int *)malloc(3 * sizeof(int));
  char *copy = strdup(input);
  char *token = strtok(copy, ":");
  int i = 0;
  while (token != NULL && i < 3) {
    buffer[i] = atoi(token);
    token = strtok(NULL, ":");
    i++;
  }
  free(copy);
  return buffer;
}

int make_it_seconds(int input[]) {
  int h = input[0];
  int m = input[1];
  int s = input[2];
  return h * 3600 + m * 60 + s;
}

void get_user_input() {
  static int *peice_time = NULL;
  static int *smallest_time = NULL;
  static int *biggest_time = NULL;

  BeginDrawing();
  ClearBackground(RAYWHITE);
  GuiGetStyle(DEFAULT, BACKGROUND_COLOR);

  Rectangle bounds = {(GetScreenWidth() / 2.0f) - (255 / 2),
                      GetScreenHeight() * 0.45f, 255, 255};

  int result = GuiTextInputBox(bounds, "Input", P.prompt, "Ok;Cancel", P.input, 255, NULL);

  if (result == 1) {
    switch (P.input_step) {
    case 0: {
      P.tomatoes = atoi(P.input);
      P.input_step++;
      P.prompt = "Please input lap duration";
      P.input[0] = '\0';
      break;
    }
    case 1: {
      peice_time = make_it_int(P.input);
      P.peice = make_it_seconds(peice_time);
      P.input_step++;
      P.prompt = "Please input your short break duration";
      P.input[0] = '\0';
      break;
    }
    case 2: {
      smallest_time = make_it_int(P.input);
      P.smallest = make_it_seconds(smallest_time);
      P.input_step++;
      P.prompt = "Please input your long break duration";
      P.input[0] = '\0';
      break;
    }
    case 3: {
      biggest_time = make_it_int(P.input);
      P.largest = make_it_seconds(biggest_time);
      P.input_done = true;
      free(peice_time);
      free(smallest_time);
      free(biggest_time);
      break;
    }
    }
  }
  EndDrawing();
}

void pomodoro() {
  static char icon[16] = "#132#";

  if (P.loopC == 0 && P.lapC == 0 && P.countdown == 0) {
    P.countdown = P.peice * 60;
  }

  BeginDrawing();
  ClearBackground(RAYWHITE);
  GuiGetStyle(DEFAULT, BACKGROUND_COLOR);

  GuiToggle((Rectangle){(GetScreenWidth() / 2.0f) - 25,
                        GetScreenHeight() * 0.8, 50, 50},
            icon, &P.is_paused);

  if (P.is_paused) {
    strcpy(icon, "#131#");
  } else {
    strcpy(icon, "#132#");

    if (P.countdown >= 0) {
      switch (P.pomodoro_state) {
      case 0: {
        DrawText(TextFormat("Tomato %i/%i, Peice %i/4", P.loopC + 1, P.tomatoes,
                            P.lapC + 1),
                 (GetScreenWidth() / 2.0f) - 100, GetScreenHeight() * 0.3f,
                 20, RED);
        DrawText(TextFormat("%i sec left...", P.countdown / 60),
                 (GetScreenWidth() / 2.0f) - 50, GetScreenHeight() * 0.5f, 20,
                 ORANGE);
        break;
      }
      case 1: {
        DrawText("Start small rest...", (GetScreenWidth() / 2.0f) - 20,
                 GetScreenHeight() * 0.3f, 20, RED);
        DrawText(TextFormat("%i sec left...", P.countdown / 60),
                 (GetScreenWidth() / 2.0f) - 50, GetScreenHeight() * 0.5f, 20,
                 ORANGE);
        break;
      }
      case 2: {
        DrawText("Start long rest...", (GetScreenWidth() / 2.0f) - 20,
                 GetScreenHeight() * 0.3f, 20, RED);
        DrawText(TextFormat("%i sec left...", P.countdown / 60),
                 (GetScreenWidth() / 2.0f) - 50, GetScreenHeight() * 0.5f, 20,
                 ORANGE);
        break;
      }
      }
      P.countdown--;
    } else {
      switch (P.pomodoro_state) {
      case 0: {
        if (P.lapC < 3) {
          P.lapC++;
          P.countdown = P.smallest * 60;
          P.pomodoro_state = 1;
        } else {
          P.lapC = 0;
          P.loopC++;
          P.countdown = P.largest * 60;
          P.pomodoro_state = 2;
        }
        if (P.loopC >= P.tomatoes) {
          P.pomodoro_done = true;
        }
        break;
      }
      case 1: {
        P.countdown = P.peice * 60;
        P.pomodoro_state = 0;
        break;
      }
      case 2: {
        P.countdown = P.peice * 60;
        P.pomodoro_state = 0;
        break;
      }
      }
    }
  }
  EndDrawing();
}
