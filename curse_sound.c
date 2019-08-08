/*
 * Curseball - Sound system
 *
 * Creator - Mason Snyder
 * Version - 0.0
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "curse_sound.h"

Mix_Music *current_music = NULL;
Mix_Chunk *current_sfx = NULL;
char *current_sfx_name = NULL;

/*
 * Starts stuff in sdl to play sound
 */
void sdl_startup() {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
}

/*
 * Shuts down sdl and frees sfx and music
 */
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
} /* sdl_shutdown() */

/*
 * Calls necissary functions to play song specified by filename.
 */
void execute_sfx (char *filename) {
  if ((current_sfx_name != NULL) && (strcmp(filename, current_sfx_name) == 0)) {
    play_sfx();
    return;
  }
  change_sfx(filename);
  play_sfx();
} /* execute_sfx() */

/*
 * Changes the loaded sfx to be played. Only one sfx can be played at a time
 * for each mix_chunk, so this function makes it easy to play multiple sounds
 */
void change_sfx(char *filename) {
  if (current_sfx != NULL) {
    Mix_FreeChunk(current_sfx);
  }
  change_current_name(filename);
  current_sfx = Mix_LoadWAV(filename);
} /* change_sfx() */

/*
 * Changes current sfx name, for easy comparison and use when playing sounds
 */
void change_current_name(char *new_name) {
  if (current_sfx_name != NULL) {
    free(current_sfx_name);
    current_sfx_name = NULL;
  }
  current_sfx_name = malloc((strlen(new_name) + 1) * sizeof(char));
  strcpy(current_sfx_name, new_name);
} /* change_current_name() */

/*
 * This function actually plays the sound, but first checks if sound is on (from the game settings).
 */
void play_sfx() {
  if ((current_sfx == NULL) || (settings[1] == 0)) {
    return;
  }
  Mix_PlayChannel(-1, current_sfx, 0);
} /* play_sfx() */
