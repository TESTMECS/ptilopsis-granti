//! @file main.c
//! @brief Pomodoro Timer
//! @constant RAYGUI_IMPLEMENTATION
//! 	For raygui.h
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib.h"
#include <stdlib.h>
#include <string.h>

//! @constant VERSION
#define VERSION "0.1.0"
//! @constant SCREEN_WIDTH
//! 	Screen width
#define SCREEN_WIDTH 1280
//! @constant SCREEN_HEIGHT
//! 	Screen height
#define SCREEN_HEIGHT 960
//! @constant MAX_INPUT_CHARS
//! 	Max input chars
#define MAX_INPUT_CHARS 256
//! @constant AUDIO_SOUND_FILE_NAME
//! 	Name of the Audio that plays between pomos.
#define AUDIO_SOUND_FILE_NAME "notify.wav"

//! @section Main_Screen_Styles
#ifndef BG_COLOR
#define BG_COLOR BLACK
#endif
#ifndef MAIN_BG_COLOR
#define MAIN_BG_COLOR 0x000033
#endif
#ifndef MAIN_TEXT_SIZE
#define MAIN_TEXT_SIZE 20
#endif
#ifndef MAIN_TEXT_COLOR
#define MAIN_TEXT_COLOR 0xCC66FFFF
#endif
#ifndef MAIN_BORDER_COLOR
#define MAIN_BORDER_COLOR 0x996633FF
#endif
#define PAUSE_ICON "#131#"
#define PLAY_ICON "#132#"
//! @end
//! @section Input_Screen_Styles
#ifndef INPUT_BG_COLOR
#define INPUT_BG_COLOR 0x1A1A1AFF
#endif
#ifndef INPUT_TEXT_SIZE
#define INPUT_TEXT_SIZE 20
#endif
#ifndef INPUT_TEXT_COLOR
#define INPUT_TEXT_COLOR 0xFFFFFFFF
#endif
#ifndef INPUT_BORDER_COLOR
#define INPUT_BORDER_COLOR 0x996633FF
#endif
#ifndef INPUT_BASE_COLOR
#define INPUT_BASE_COLOR 0x1A1A1AFF
#endif
#ifndef INPUT_BORDER_COLOR_FOCUSED
#define INPUT_BORDER_COLOR_FOCUSED 0x6B83FFFF
#endif
#ifndef INPUT_BASE_COLOR_FOCUSED
#define INPUT_BASE_COLOR_FOCUSED 0x2A2A2AFF
#endif
#ifndef INPUT_TEXT_COLOR_FOCUSED
#define INPUT_TEXT_COLOR_FOCUSED 0xFFFFFFFF
#endif
#ifndef INPUT_BORDER_WIDTH
#define INPUT_BORDER_WIDTH 2
#endif
#ifndef INPUT_TEXT_PADDING
#define INPUT_TEXT_PADDING 5
#endif
//! @end
//! @section Pomo_Styles
#ifndef PSTYLES_TEXT_COLOR
#define PSTYLES_TEXT_COLOR RED
#endif
#ifndef PSTYLES_TIMER_COLOR
#define PSTYLES_TIMER_COLOR ORANGE
#endif
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

void print_help() {
  printf("Options:\n");
  printf("  -h, --help\t\tShow this help message and exit\n");
  printf("  -v, --version\t\tShow version information and exit\n");
  printf("Usage: ptilo -s \"laps:peice:smallest:largest\"\n");
}

int main(int argc, char **argv) {
  initalize_pomodoro(&P);
  if (argc > 1 &&
      (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
    print_help();
    exit(EXIT_SUCCESS);
  }
  if (argc > 1 &&
      (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0)) {
    printf("Pomodoro Timer\n");
    printf("Version: %s\n", VERSION);
    exit(EXIT_SUCCESS);
  }
  //! Handle -s and -t args.
  //! @example
  //! ptilo -s "5:25:5:15"
  //! ptilo -t "Dark" (TODO)
  if (argc > 1) {
    int a;
    char *theme = NULL;
    for (int i = 1; i < argc; i++) {
      if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
        char *copy = strdup(argv[++i]);
        char *tok = strtok(copy, ":");
        //! Get laps
        if (tok)
          a = atoi(tok);
        if (a < 0 || a > 10) {
          printf("Laps must be between 0 and 10. Try again:\n");
          print_help();
          exit(EXIT_FAILURE);
        } else {
          //! Set laps
          P.laps = a;
        }
        //! Get peice time
        tok = strtok(NULL, ":");
        if (!tok)
          goto bad_input;
        int *temp_time = make_it_int(tok);
        int temp_seconds = make_it_seconds(temp_time);
        if (temp_seconds < 0 || temp_seconds > 3600) {
          printf("Duration must be between 0 and 60 minutes. Try again:\n");
          print_help();
          exit(EXIT_FAILURE);
        }
        //! Set peice time
        P.peice = temp_seconds;
        //! Get smallest time
        tok = strtok(NULL, ":");
        if (!tok)
          goto bad_input;
        int *temp_time_short = make_it_int(tok);
        int temp_seconds_short = make_it_seconds(temp_time_short);
        if (temp_seconds_short < 0 || temp_seconds_short > 3600) {
          printf("Duration must be between 0 and 60 minutes. Try again:\n");
          print_help();
          exit(EXIT_FAILURE);
        }
        //! Set smallest time
        P.smallest = temp_seconds_short;
        tok = strtok(NULL, ":");
        //! Get largest time
        if (!tok)
          goto bad_input;
        int *temp_time_long = make_it_int(tok);
        int temp_seconds_long = make_it_seconds(temp_time_long);
        if (temp_seconds_long < 0 || temp_seconds_long > 3600) {
          printf("Duration must be between 0 and 60 minutes. Try again:\n");
          print_help();
          exit(EXIT_FAILURE);
        }
        //! Set largest time
        P.largest = temp_seconds_long;
        //! Set input done.
        P.input_done = true;
        free(temp_time_long);
        free(temp_time_short);
        free(temp_time);
        free(copy);
      }
    }
  }

  //! Initalize the raylib instance
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pomodoro Timer");
  SetWindowMinSize(SCREEN_WIDTH, SCREEN_HEIGHT);
  SetTargetFPS(60);
  InitAudioDevice();

  //! Main loop
  while (!WindowShouldClose()) {
    switch (P.state) {
    case SCREEN_USER_INPUT: {
      //! If no cli args then prompt for input.
      if (!P.input_done) {
        get_user_input();
      }
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
      ClearBackground(BG_COLOR);
      DrawText("Congrats", (GetScreenWidth() / 1.5f) - 10,
               GetScreenHeight() * 0.8f, 50, RED);
      EndDrawing();
      break;
    }
    }
  }
  //! Cleanup
  CloseAudioDevice();
  CloseWindow();
  return 0;

bad_input:
  printf("Bad input. Try again:\n");
  print_help();
  exit(EXIT_FAILURE);
}

//! @function make_it_int
int *make_it_int(char input[]) {
  int *buffer = (int *)calloc(3, sizeof(int));
  int h = 0, m = 0, s = 0;
  sscanf(input, "%d:%d:%d", &h, &m, &s);
  buffer[0] = h;
  buffer[1] = m;
  buffer[2] = s;
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
  ClearBackground(BG_COLOR);

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

  //! Draw the rectangle for the input box
  Rectangle bounds = {(GetScreenWidth() / 2.0f) - (400 / 2),
                      GetScreenHeight() * 0.4f, 700, 300};

  //! Draw the input box
  int result = GuiTextInputBox(bounds, "Input", P.prompt, "Ok;Cancel", P.input,
                               255, NULL);

  if (result == 1) {
    //! Get the input
    switch (P.input_step) {
    case INPUT_LAP: {
      int laps_val = atoi(P.input);
      if (laps_val < 0 || laps_val > 10) {
        P.prompt = "Laps must be between 0 and 10. Try again:";
        P.input[0] = '\0';
      } else {
        P.laps = laps_val;
        P.input_step++;
        P.prompt =
            "Pomodoro duration in minutes between 0 and 60. (ex: \"25\") ";
        P.input[0] = '\0';
      }
      break;
    }
    case INPUT_PEICE: {
      int *temp_time = make_it_int(P.input);
      int temp_seconds = make_it_seconds(temp_time);
      if (temp_seconds < 0 || temp_seconds > 3600) {
        P.prompt = "Duration must be between 0 and 60 minutes. Try again:";
        P.input[0] = '\0';
        free(temp_time);
      } else {
        peice_time = temp_time;
        P.peice = temp_seconds;
        P.input_step++;
        P.prompt = "Short break in minutes between 0 and 60 (ex: \"5\") ";
        P.input[0] = '\0';
      }
      break;
    }
    case INPUT_SMALLEST: {
      int *temp_time = make_it_int(P.input);
      int temp_seconds = make_it_seconds(temp_time);
      if (temp_seconds < 0 || temp_seconds > 3600) {
        P.prompt = "Duration must be between 0 and 60 minutes. Try again:";
        P.input[0] = '\0';
        free(temp_time);
      } else {
        smallest_time = temp_time;
        P.smallest = temp_seconds;
        P.input_step++;
        P.prompt = "Long break in minutes between 0 and 60 (ex: \"15\") ";
        P.input[0] = '\0';
      }
      break;
    }
    case INPUT_LARGEST: {
      int *temp_time = make_it_int(P.input);
      int temp_seconds = make_it_seconds(temp_time);
      if (temp_seconds < 0 || temp_seconds > 3600) {
        P.prompt = "Duration must be between 0 and 60 minutes. Try again:";
        P.input[0] = '\0';
        free(temp_time);
      } else {
        biggest_time = temp_time;
        P.largest = temp_seconds;
        P.input_done = true;
        free(peice_time);
        free(smallest_time);
        free(biggest_time);
      }
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

  Sound fx = LoadSound(AUDIO_SOUND_FILE_NAME);

  BeginDrawing();
  ClearBackground(BG_COLOR);
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
      PlaySound(fx);
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
      PlaySound(fx);
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
