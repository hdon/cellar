#ifndef _SOUND_H
#define _SOUND_H
#include <SDL_mixer.h>

namespace Sound
{
    enum
    {
        Jump,
        Bounce,
        Brick,
        Coin,
        Spring,
        Hit,
        Emerge,
        SFX_MAX
    };

    enum
    {
        MUSIC_TITLE,
        MUSIC_BATTLE,
        MUSIC_OPTIONS
    };

    extern bool EnableAudio, EnableMusic, EnableSFX;

    void Init();
    void Play(int index);
    void Play(int index, int chan);
    void PlaySong(int which);
    void Quit();
};

#endif/*_SOUND_H*/

