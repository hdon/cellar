#include <string>
#include <iostream>
using namespace std;
#include "sound.h"

namespace Sound
{
    /* Simple sound options */
    bool EnableAudio = true;
    bool EnableMusic = true;
    bool EnableSFX = true;

    /* Filenames of sounds */
    const char *SoundFileNames[] =
    {
        "jump.wav",
        "bump.wav",
        "breakblock.wav",
        "coin.wav",
        "springjump.wav",
        "hit.wav",
        "sprout.wav",
    };

    /* Paths to data files */
    string MusicPath = "data/music/";
    string SoundPath = "data/sfx/";

    /* SDL_Mixer store */
    Mix_Music *mix_music;
    Mix_Chunk *mix_sound[SFX_MAX];

    /* Output control (change these in order to change the amount and
     * destination of Sound module output) */
    ostream& verbose = cout;
    ostream& error = cerr;

    /* Initialize the sound module */
    void Init()
    {
        /* Skip disabled audio */
        if (!EnableAudio)
        {
            verbose << "Audio disabled -- skipping initialization" << endl;
            return;
        }

        verbose << "Initializing audio...";

        if (Mix_OpenAudio(22050, AUDIO_S16, 2, 1024))
        {
            error << "Mix_OpenAudio Failure" << endl;
            exit(1);
        }
        Mix_AllocateChannels(32);

        verbose << "Audio initialized." << endl;

        if (! (EnableAudio && EnableSFX)) return;

        for (int i=0; i<SFX_MAX; i++)
        {
            string filename = SoundPath;
            filename += SoundFileNames[i];
            mix_sound[i] = Mix_LoadWAV(filename.c_str());

            verbose << "Loading " << filename << "...";

            if (!mix_sound)
                error << endl << "Could not load " << filename << endl;

            else
                verbose << "done." << endl;
        }

        verbose << "...done" << endl;
    }

    /* Play a sound */
    void Play(int index)
    {
        Play(index,-1);
    }
    void Play(int index, int chan)
    {
        if (! (EnableAudio && EnableSFX)) return;
        Mix_PlayChannel(chan, mix_sound[index], 0);
    }

    /* Callback function for restarting the currently-selected music */
    void ContinueSong()
    {
        Mix_PlayMusic(mix_music, 0);
        Mix_HookMusicFinished(ContinueSong);
    }

    /* Choose a song to play */
    void PlaySong(int which)
    {
        if (! (EnableAudio && EnableMusic)) return;

        /* Determine filename of song */
        const char *Songs[] =
        {
            "whistling.aiff",
            "battle.aiff",
            "voip.aiff"
        };

        string filename = MusicPath;
        filename += Songs[which];

        /* Load music */
        mix_music = Mix_LoadMUS(filename.c_str());

        if (! mix_music)
        {
            error << "Couldn't load song: " << filename << endl;
            return;
        }

        verbose << "Loaded song: " << filename << endl;
        ContinueSong();
    }

    void Quit()
    {
        verbose << "Uninitializing sound...";

        for (int i=0; i<SFX_MAX; i++)
            if (mix_sound[i])
                Mix_FreeChunk(mix_sound[i]);

        verbose << "done" << endl;
    }
};

