/*
 * Mammut videogame prototype
 *
 * LANGUAGE: C99 (would prefer C11, but SDL2 largely provides the missing bits)
 * 
 * FILE DESCRIPTION: the main game loop as well as the main()
 * AUTHOR:           Sio
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_events.h>

#include "data.h"

void GameLoop( GameInstance* instance );

int main( int argc, char** argv, char** envv )
{
    SDL_SetMainReady();
    SDL_Init( SDL_INIT_EVERYTHING );
    atexit( SDL_Quit );
    
    IMG_Init( IMG_INIT_PNG | IMG_INIT_JPG );
    atexit( IMG_Quit );
    
    
    // seed random number generator
    srand( time( 0 ) );
    
    GameInstance* instance = GameInstance_new( 2 );
    GameLoop( instance );
    GameInstance_delete( instance );
    instance = NULL;
    
    exit( 0 );
}


const SDL_Rect NUMBER_POSITION = {
    .w = 64,
    .h = 64,
    .x = 220,
    .y = 66
};

const SDL_Rect SPEAR_CARD_BUTTON_POSITION = {
    .w = 64,
    .h = 64,
    .x = 300,
    .y = 66
};

const SDL_Rect BERRY_CARD_BUTTON_POSITION = {
    .w = 64,
    .h = 64,
    .x = 366,
    .y = 66
};

const SDL_Rect ROLL_DICE_BUTTON_POSITION = {
    .w = 64,
    .h = 64,
    .x = 2, 
    .y = 2  
};

void GameLoop_step( GameInstance* instance, TurnChanges* t )
{
    // NOTE: rolling dice and ending turn are equivalent (cards may only be played before rolling dice); thus, we don't need an "end turn" button
    SDL_bool player_endedTurn = SDL_FALSE;
    
    SDL_Event ev;
    while( SDL_PollEvent( &ev ) ) {
        int x = 0;
        int y = 0;
        switch( ev.type ) {
            case SDL_QUIT:
                GameInstance_delete( instance );
                exit( 0 );
                break;
            case SDL_KEYDOWN:
                if( ev.key.keysym.sym == SDLK_ESCAPE ) {
                    GameInstance_delete( instance );
                    exit( 0 );
                }
                break;
            case SDL_MOUSEBUTTONUP:
                // TODO: check what the player wants done;
                // if "roll dice" button has been clicked, set player_endedTurn to SDL_TRUE
                x = ev.button.x;
                y = ev.button.y;
                
                if(  x > ROLL_DICE_BUTTON_POSITION.x
                  && ( x < ROLL_DICE_BUTTON_POSITION.x
                     + ROLL_DICE_BUTTON_POSITION.w )
                  && y > ROLL_DICE_BUTTON_POSITION.y
                  && ( y < ROLL_DICE_BUTTON_POSITION.y
                     + ROLL_DICE_BUTTON_POSITION.h ) ) {
                    // get a random number between 1 and 6
                    t -> diceroll_result = ( rand() % 6 ) + 1;
                    player_endedTurn = SDL_TRUE;
                    break;
                }
                // if spear card has been clicked, set the spear card played var
                if(  x > SPEAR_CARD_BUTTON_POSITION.x
                  && ( x < SPEAR_CARD_BUTTON_POSITION.x
                     + SPEAR_CARD_BUTTON_POSITION.w )
                  && y > SPEAR_CARD_BUTTON_POSITION.y
                  && ( y < SPEAR_CARD_BUTTON_POSITION.y
                     + SPEAR_CARD_BUTTON_POSITION.h ) ) {
                    t -> spear_card_played = SDL_TRUE;
                    break;
                }
                
                if(  x > BERRY_CARD_BUTTON_POSITION.x
                  && ( x < BERRY_CARD_BUTTON_POSITION.x
                     + BERRY_CARD_BUTTON_POSITION.w )
                  && y > BERRY_CARD_BUTTON_POSITION.y
                  && ( y < BERRY_CARD_BUTTON_POSITION.y
                     + BERRY_CARD_BUTTON_POSITION.h ) ) {
                    t -> berry_card_played = SDL_TRUE;
                    break;
                }
                
                // if a player clicks any player's icon, set that player as the spear card target
                for( int i = 0; i < instance -> player_count ; i++ ) {
                    Player* current = instance -> players[i];
                    int screen_width = 0;
                    int screen_height = 0;
                    SDL_GetWindowSize( instance -> win, &screen_width, &screen_height );
                    SDL_Rect p_pos = current -> screenPos( current, screen_width, screen_height );
                    
                    if(  x > p_pos.x
                      && ( x < p_pos.x + p_pos.w )
                      && y > p_pos.y
                      && ( y < p_pos.y + p_pos.h ) ) {
                        t -> spear_card_target = i;
                        break;
                    }
                }
                break;
            default:
                // do nothing
                break;
        }
    }
    
    if( player_endedTurn == SDL_TRUE ) {
        // player has done something, initiate instance step
        instance -> step( instance, t );
        TurnChanges_destroy( t );
        *t = TurnChanges_construct();
        instance -> turn_counter++;
    }
    
    // render
    SDL_RenderCopy( instance -> ren, instance -> background, NULL, NULL );
    SDL_RenderCopy( instance -> ren, instance -> roll_dice_button_icon, NULL, &ROLL_DICE_BUTTON_POSITION );
    for( int i = 0; i < instance -> player_count; i++ ) {
        Player* current = instance -> players[i];
        int screen_width = 0;
        int screen_height = 0;
        SDL_GetWindowSize( instance -> win, &screen_width, &screen_height );
        SDL_Rect dst = current -> screenPos( current, screen_width, screen_height );
        SDL_RenderCopy( instance -> ren, instance -> player_icons[i], NULL, &dst );
    }
    // render index of current player to screen
    SDL_RenderCopy( instance -> ren, instance -> numbers[instance -> current_player], NULL, &NUMBER_POSITION );
    SDL_RenderCopy( instance -> ren, instance -> berry_card_icon, NULL, &BERRY_CARD_BUTTON_POSITION );
    SDL_RenderCopy( instance -> ren, instance -> spear_card_icon, NULL, &SPEAR_CARD_BUTTON_POSITION );
    
    SDL_RenderPresent( instance -> ren );
}

void GameLoop( GameInstance* instance )
{
    TurnChanges t = TurnChanges_construct();
    while( 1 ) {
        GameLoop_step( instance, &t );
    }
    TurnChanges_destroy( &t );
}
