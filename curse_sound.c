#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "curse_sound.h"


Mix_Music *current_music = NULL;
Mix_Chunk *current_sfx = NULL;
char *current_sfx_name = NULL;

void sdl_startup() {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
}

void sdl_shutdown() {
  if (current_music != NULL) {
    Mix_FreeMusic(current_music);
    current_music = NULL;
  }
  if (current_sfx != NULL) {
    Mix_FreeChunk(current_sfx);
    current_sfx = NULL;
  }
  Mix_Quit();
  SDL_Quit();
}

void execute_sfx (char *filename) {
  if ((current_sfx_name != NULL) && (strcmp(filename, current_sfx_name) == 0)) {
    play_sfx();
    return;
  }
  change_sfx(filename);
  play_sfx();
}

void change_sfx(char *filename) {
  if (current_sfx != NULL) {
    Mix_FreeChunk(current_sfx);
  }
  change_current_name(filename);
  current_sfx = Mix_LoadWAV(filename);
}

void change_current_name(char *new_name) {
  if (current_sfx_name != NULL) {
    free(current_sfx_name);
    current_sfx_name = NULL;
  }
  current_sfx_name = malloc(strlen(new_name) * sizeof(char));
  strcpy(current_sfx_name, new_name);
}

void play_sfx() {
  if (current_sfx == NULL) {
    return;
  }
  Mix_PlayChannel(-1, current_sfx, 0);
}
