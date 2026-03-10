//! @file main.c
//! @brief Pomodoro Timer
//! @constant RAYGUI_IMPLEMENTATION
//! 	For raygui.h
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib.h"
#include <stdlib.h>
#include <string.h>

//! @constant SCREEN_WIDTH
//! 	Screen width
#define SCREEN_WIDTH 1280
//! @constant SCREEN_HEIGHT
//! 	Screen height
#define SCREEN_HEIGHT 960
//! @constant MAX_INPUT_CHARS
//! 	Max input chars
#define MAX_INPUT_CHARS 256

//! @section Main_Screen_Styles
#define MAIN_BG_COLOR 0x000033
#define MAIN_TEXT_SIZE 20
#define MAIN_TEXT_COLOR 0xCC66FFFF
#define MAIN_BORDER_COLOR 0x996633FF
#define PAUSE_ICON "#131#"
#define PLAY_ICON "#132#"
//! @end
//! @section Input_Screen_Styles
#define INPUT_BG_COLOR 0x1A1A1AFF
#define INPUT_TEXT_SIZE 20
#define INPUT_TEXT_COLOR 0xFFFFFFFF
#define INPUT_BORDER_COLOR 0x996633FF
#define INPUT_BASE_COLOR 0x1A1A1AFF
#define INPUT_TEXT_COLOR 0xFFFFFFFF
#define INPUT_BORDER_COLOR_FOCUSED 0x6B83FFFF
#define INPUT_BASE_COLOR_FOCUSED 0x2A2A2AFF
#define INPUT_TEXT_COLOR_FOCUSED 0xFFFFFFFF
#define INPUT_BORDER_WIDTH 2
#define INPUT_TEXT_PADDING 5
//! @end
//! @section Pomo_Styles
#define PSTYLES_TEXT_COLOR RED
#define PSTYLES_TIMER_COLOR ORANGE
//! @end

//! @enum ScreenState
typedef enum {
  //! @member USER_INPUT
  //! 	Entering the time constraints.
  SCREEN_USER_INPUT = 0,
  //! @member POMODORO
  //! Displaying the pomodoro timer.
  SCREEN_POMODORO = 1,
  //! @member END
  //! Ending screen.
  SCREEN_END = 2,
} ScreenState;

//! @enum PomodoroState
typedef enum {
  //! @member POMO_RUNNING
  //! 	Pomodoro is paused.
  POMO_RUNNING = 0,
  //! @member POMO_SHORT_REST
  //! 	Pomodoro is running.
  POMO_SHORT_REST = 1,
  //! @member POMO_LONG_REST
  //! 	Pomodoro is finished.
  POMO_LONG_REST = 2,
} PomodoroState;

//! @enum
typedef enum {
  //! @member LAP
  //! 	User input for laps.
  INPUT_LAP = 0,
  //! @member PEICE
  //! 	User input for peice.
  INPUT_PEICE = 1,
  //! @member SMALLEST
  //! 	User input for smallest break.
  INPUT_SMALLEST = 2,
  //! @member LARGEST
  //! 	User input for largest break.
  INPUT_LARGEST = 3,
} InputState;

//! @struct Pomodoro
typedef struct {
  //! @member laps
  //! 	Number of laps(loops)
  int laps;
  //! @member peice
  //! 	Peice duration
  int peice;
  //! @member smallest
  //! 	Shortest break duration
  uint smallest;
  //! @member largest
  //! 	Longest break duration
  uint largest;
  //! @member state
  //! 	State of the program
  ScreenState state;
  //! @member input_step
  //! 	Step of the input
  InputState input_step;
  //! @member pomodoro_state
  //! 	State of the pomodoro
  PomodoroState pomodoro_state;
  //! @member loopC
  //! 	Current loop
  int loopC;
  //! @member lapC
  //! 	Current lap
  int lapC;
  //! @member countdown
  //! 	Countdown
  int countdown;
  //! @member is_paused
  //! 	Is the pomodoro paused
  bool is_paused;
  //! @member input_done
  //! 	Is the input done
  bool input_done;
  //! @member pomodoro_done
  //! 	Is the pomodoro done
  bool pomodoro_done;
  //! @member input
  //! 	Input
  char input[MAX_INPUT_CHARS];
  //! @member prompt
  //! 	Prompt
  const char *prompt;
} Pomodoro;

//! @struct Button
//! 	Pause Pomodoro timer.
typedef struct {
  //! @member rect
  //! 	Rectangle from raylib
  Rectangle rect;
  //! @member state
  //! 	State of the button
  bool state;
} Button;

//! @variable P
//! 	Pomodoro timer
Pomodoro P;

//! @variable FINISH
//! 	Is the program finished
bool FINISH = false;

// forward declaration
int *make_it_int(char input[]);
int make_it_seconds(int input[]);
void get_user_input();
void pomodoro();

void initalize_pomodoro(Pomodoro *p) {
  p->state = 0;
  p->input_step = 0;
  p->input_done = false;
  p->pomodoro_done = false;
  p->pomodoro_state = 0;
  p->loopC = 0;
  p->lapC = 0;
  p->is_paused = false;
  strcpy(P.input, "");
  p->prompt = "How Many Laps?";
}

int main(void) {
  initalize_pomodoro(&P);

  //! Initalize the raylib instance
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pomodoro Timer");
  SetWindowMinSize(SCREEN_WIDTH, SCREEN_HEIGHT);
  SetTargetFPS(60);

  //! Main loop
  while (!WindowShouldClose()) {
    switch (P.state) {
    case SCREEN_USER_INPUT: {
      get_user_input();
      if (P.input_done) {
        P.state = SCREEN_POMODORO;
      }
      break;
    }
    case SCREEN_POMODORO: {
      pomodoro();
      if (P.pomodoro_done) {
        P.state = SCREEN_END;
      }
      break;
    }
    case SCREEN_END: {
      BeginDrawing();
      ClearBackground(BLACK);
      DrawText("Congrats", (GetScreenWidth() / 1.5f) - 10,
               GetScreenHeight() * 0.8f, 50, RED);
      EndDrawing();
      break;
    }
    }
  }
  //! Cleanup
  CloseWindow();
  return 0;
}

//! @function make_it_int
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

//! @function make_it_seconds
int make_it_seconds(int input[]) {
  int h = input[0];
  int m = input[1];
  int s = input[2];
  if (m == 0 && s == 0) {
    return h * 60;
  }
  return h * 3600 + m * 60 + s;
}

void get_user_input() {
  static int *peice_time = NULL;
  static int *smallest_time = NULL;
  static int *biggest_time = NULL;
  //! Initialize the raygui
  BeginDrawing();
  ClearBackground(BLACK);

  //! @see Main_Screen_Styles
  GuiSetStyle(DEFAULT, BACKGROUND_COLOR, MAIN_BG_COLOR);
  GuiSetStyle(DEFAULT, TEXT_SIZE, MAIN_TEXT_SIZE);
  GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, MAIN_TEXT_COLOR);
  GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, MAIN_BORDER_COLOR);

  //! @see Input_Screen_Styles
  GuiSetStyle(TEXTBOX, BORDER_COLOR_NORMAL, INPUT_BORDER_COLOR);
  GuiSetStyle(TEXTBOX, BASE_COLOR_NORMAL, INPUT_BASE_COLOR);
  GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, INPUT_TEXT_COLOR);
  GuiSetStyle(TEXTBOX, BORDER_COLOR_FOCUSED, INPUT_BORDER_COLOR_FOCUSED);
  GuiSetStyle(TEXTBOX, BASE_COLOR_FOCUSED, INPUT_BASE_COLOR_FOCUSED);
  GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED, INPUT_TEXT_COLOR_FOCUSED);
  GuiSetStyle(TEXTBOX, BORDER_WIDTH, INPUT_BORDER_WIDTH);
  GuiSetStyle(TEXTBOX, TEXT_PADDING, INPUT_TEXT_PADDING);

  //! Draw the rectangle
  Rectangle bounds = {(GetScreenWidth() / 2.0f) - (400 / 2),
                      GetScreenHeight() * 0.4f, 400, 300};

  //! Draw the input box
  int result = GuiTextInputBox(bounds, "Input", P.prompt, "Ok;Cancel", P.input,
                               255, NULL);

  if (result == 1) {
    //! Get the input
    switch (P.input_step) {
    case INPUT_LAP: {
      P.laps = atoi(P.input);
      P.input_step++;
      P.prompt = "Pomodoro duration in minutes between 0 and 60. (ex: \"25\") ";
      P.input[0] = '\0';
      break;
    }
    case INPUT_PEICE: {
      peice_time = make_it_int(P.input);
      P.peice = make_it_seconds(peice_time);
      P.input_step++;
      P.prompt = "Short break in minutes between 0 and 60 (ex: \"5\") ";
      P.input[0] = '\0';
      break;
    }
    case INPUT_SMALLEST: {
      smallest_time = make_it_int(P.input);
      P.smallest = make_it_seconds(smallest_time);
      P.input_step++;
      P.prompt = "Long break in minutes between 0 and 60 (ex: \"15\") ";
      P.input[0] = '\0';
      break;
    }
    case INPUT_LARGEST: {
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
  static char icon[16] = PLAY_ICON;
  static double last_time = 0;

  //! Initialize the raygui
  //! [raylib.h] GetTime() : Get elapsed time in seconds since InitWindow()
  if (P.loopC == 0 && P.lapC == 0 && P.countdown == 0) {
    P.countdown = P.peice;
    last_time = GetTime();
  }
  //! Begin Countdown
  double current_time = GetTime();
  if (!P.is_paused && current_time - last_time >= 1.0) {
    P.countdown--;
    last_time = current_time;
  }

  BeginDrawing();
  ClearBackground(BLACK);
  GuiGetStyle(DEFAULT, BACKGROUND_COLOR);

  //! @define Pause Button
  GuiToggle((Rectangle){(GetScreenWidth() / 2.0f) - 25, GetScreenHeight() * 0.8,
                        50, 50},
            icon, &P.is_paused);

  //! @define Skip Button
  if (GuiButton((Rectangle){(GetScreenWidth() / 2.0f) + 50,
                            GetScreenHeight() * 0.8, 50, 50},
                "#133#")) {
    switch (P.pomodoro_state) {
    case POMO_RUNNING: {
      if (P.lapC < 3) { //! Increment laps
        P.lapC++;
        P.countdown = P.smallest;
        P.pomodoro_state = POMO_SHORT_REST;
      } else { //! Increment loops
        P.lapC = 0;
        P.loopC++;
        P.countdown = P.largest;
        P.pomodoro_state = POMO_LONG_REST;
      }
      //! Check if loops are done
      if (P.loopC >= P.laps) {
        P.pomodoro_done = true;
      }
      break;
    }
    case POMO_SHORT_REST:
    case POMO_LONG_REST: {
      //! Reset countdown
      P.countdown = P.peice;
      P.pomodoro_state = POMO_RUNNING;
      break;
    }
    } // end switch
    last_time = GetTime();
  } // end if

  //! Check if pomodoro is paused
  if (P.is_paused) {
    strcpy(icon, PAUSE_ICON);
    last_time = GetTime(); //! Reset last time
  } else {
    strcpy(icon, PLAY_ICON);
  }

  //! Check if countdown is done
  if (P.countdown < 0) {
    switch (P.pomodoro_state) {
    case POMO_RUNNING: {
      if (P.lapC < 3) {
        P.lapC++;
        P.countdown = P.smallest;
        P.pomodoro_state = 1;
      } else {
        P.lapC = 0;
        P.loopC++;
        P.countdown = P.largest;
        P.pomodoro_state = 2;
      }
      if (P.loopC >= P.laps) {
        P.pomodoro_done = true;
      }
      last_time = GetTime();
      break;
    }
    case POMO_SHORT_REST: {
      P.countdown = P.peice;
      P.pomodoro_state = 0;
      last_time = GetTime();
      break;
    }
    case POMO_LONG_REST: {
      P.countdown = P.peice;
      P.pomodoro_state = 0;
      last_time = GetTime();
      break;
    }
    } // end switch
  } // end if

  //! Draw the pomodoro
  //! @see Pomo_Styles
  if (!P.is_paused) {
    switch (P.pomodoro_state) {
    case POMO_RUNNING: {
      DrawText(
          TextFormat("Lap %i/%i, Peice %i/4", P.loopC + 1, P.laps, P.lapC + 1),
          (GetScreenWidth() / 2.0f) - 150, GetScreenHeight() * 0.3f, 30,
          PSTYLES_TEXT_COLOR);
      DrawText(
          TextFormat("%02d:%02d left...", P.countdown / 60, P.countdown % 60),
          (GetScreenWidth() / 2.0f) - 100, GetScreenHeight() * 0.5f, 60,
          PSTYLES_TIMER_COLOR);
      break;
    }
    case POMO_SHORT_REST: {
      DrawText("Start small rest...", (GetScreenWidth() / 2.0f) - 50,
               GetScreenHeight() * 0.3f, 30, PSTYLES_TEXT_COLOR);
      DrawText(
          TextFormat("%02d:%02d left...", P.countdown / 60, P.countdown % 60),
          (GetScreenWidth() / 2.0f) - 100, GetScreenHeight() * 0.5f, 60,
          PSTYLES_TIMER_COLOR);
      break;
    }
    case POMO_LONG_REST: {
      DrawText("Start long rest...", (GetScreenWidth() / 2.0f) - 50,
               GetScreenHeight() * 0.3f, 30, PSTYLES_TEXT_COLOR);
      DrawText(
          TextFormat("%02d:%02d left...", P.countdown / 60, P.countdown % 60),
          (GetScreenWidth() / 2.0f) - 100, GetScreenHeight() * 0.5f, 60,
          PSTYLES_TIMER_COLOR);
      break;
    }
    } // end switch
  } // end if
  EndDrawing();
}
