#ifndef SOME_GAME_HEADER
#define SOME_GAME_HEADER
#include <SDL_events.h>
namespace Game
{
    void Init();
    void Step();
    void Draw();
    void Quit();
    void DefaultSetup();
    void DrawCell(int x, int y);
    void Mouse(int x, int y);
    bool MouseButton(int button, bool down);
    bool Key(int key, bool down);
    bool SDLEvent(SDL_Event *e);
    void MakeLife(int x, int y);
};
#endif

