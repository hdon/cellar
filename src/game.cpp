#include "oglconsole.h"
#include "game.h"
#include "machine.h"
#include "life.h"
#include "glerror.h"
#include <math.h>
#include <SDL.h>
#include <list>
#ifdef __MACH__
#  include <OpenGL/gl.h>
#else
#  include <GL/gl.h>
#endif
using namespace std;

// TODO: Remove/improve that old "texture manager" thing
#include "texture.h"
TextureManager *texmex;

extern int ScreenWidth, ScreenHeight;

namespace Game
{
    int W=0, H=0;
    bool pause=true;

    MachineGrid *machineGrid=NULL;
    LifeGrid *lifeGrid=NULL;

    // mouse
    int mx=0, my=0;
    int mX=0, mY=0;
    char *mclip = NULL;
    int mclipw=0, mcliph=0;
    bool mdown = false;

    void Init()
    {
    }

    void RStep()
    {
        lifeGrid->Step();
        machineGrid->Step();

        lifeGrid->PStep();
        machineGrid->PStep();
    }
    void Step()
    {
        if(!pause)RStep();
    }

    void Quit()
    {
        if(machineGrid) delete machineGrid;
        if(lifeGrid) delete lifeGrid;
        machineGrid=NULL;
        lifeGrid=NULL;
    }

    void DrawCell(int x, int y)
    {
        glVertex2i(x,y);
        glVertex2i(x+1,y);
        glVertex2i(x+1,y+1);
        glVertex2i(x,y+1);
    }

    void Draw()
    {
        static int Frame = 0;
        Frame++;

        // Configure the GL
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
      //glOrtho(0, 320, 240, 0, 1, -1);
        glOrtho(0, W, H, 0, 1, -1);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE,GL_ONE);

        glBegin(GL_QUADS);

        machineGrid->Draw();
        lifeGrid->Draw();

        // draw mouse
        double intensity = fabs(sin(Frame*0.045)) * 0.2;
        glColor3d(intensity, intensity, intensity);
        glVertex2d(mx-0.5,my-0.5);
        glVertex2d(mx-0.5,mY+1.5);
        glVertex2d(mX+1.5,mY+1.5);
        glVertex2d(mX+1.5,my-0.5);
        glEnd();

        glColor3ub(255,255,255);
        glBegin(GL_LINE_LOOP);
        glVertex2i(mx  ,my  );
        glVertex2i(mx  ,my+1);
        glVertex2i(mx+1,my+1);
        glVertex2i(mx+1,my  );
        glEnd();

        // Relinquish the GL
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glPopAttrib();

        static unsigned int err=0;
        glError(NULL, &err);
    }

    void DefaultSetup(int w, int h)
    {
        if (lifeGrid) {
            LifeGrid *p = lifeGrid;
            lifeGrid = NULL;
            delete p;
        }
        if (machineGrid) {
            MachineGrid *p = machineGrid;
            machineGrid = NULL;
            delete p;
        }

        W = w;
        H = h;

        machineGrid = new MachineGrid(W,H);
        lifeGrid = new LifeGrid(W,H);
    }

    void Mouse(int x, int y)
    {
        x = int((x+double(ScreenWidth) /W/2) / (double(ScreenWidth) / W));
        y = int((y+double(ScreenHeight)/H/2) / (double(ScreenHeight)/ H));

        if (mdown)
        {
            if (x>mX) mX = x;
            else if (x<mx) mx = x;
            if (y>mY) mY = y;
            else if (y<my) my = y;
        }
        else
        {
            mX = mx = x;
            mY = my = y;
        }
    }

    bool MouseButton(int button, bool down)
    {
        if (button == 2) if (down) MakeLife(mx,my);
        if (button == 1)
        {
            mdown = down;
        }
        return true;
    }

    bool SDLEvent(SDL_Event *e)
    {
        switch (e->type)
        {
            case SDL_KEYDOWN:
                if (e->key.state)
                switch (e->key.keysym.sym)
                {
                    default:
                        return false;
                    case ' ':
                        RStep();
                        return true;
                    case SDLK_RETURN:
                        pause = ! pause;
                        return true;
                    case 'n':
                        /* TODO: clear all grids */
                        return true;
                    case SDLK_DOWN:
                        if (e->key.keysym.mod & KMOD_SHIFT)
                            mY++;
                        else {
                            mY=++my;
                            mX=mx;
                        }
                        return true;
                    case SDLK_UP:
                        if (e->key.keysym.mod & KMOD_SHIFT)
                            mY--;
                        else {
                            mY=--my;
                            mX=mx;
                        }
                        return true;
                    case SDLK_LEFT:
                        if (e->key.keysym.mod & KMOD_SHIFT)
                            mX--;
                        else {
                            mX=--mx;
                            mY=my;
                        }
                        return true;
                    case SDLK_RIGHT:
                        if (e->key.keysym.mod & KMOD_SHIFT)
                            mX++;
                        else {
                            mX=++mx;
                            mY=my;
                        }
                        return true;

                    case SDLK_BACKSPACE:
                        machineGrid->DestroyAllSparks();
                        break;
                        
                    case 'y': //yank
                        if (mclip) delete[] mclip;
                        mclipw = mX-mx+1;
                        mcliph = mY-my+1;
                        OGLCONSOLE_Print("Yanked %ix%i\n", mclipw, mcliph);
                        mclip = new char[mclipw*mcliph];
                        for (int x=0; x<mclipw; x++)
                            for (int y=0; y<mcliph; y++)
                                mclip[x+y*mclipw] = *machineGrid->Gell(x+mx,y+my);
                        return true;

                    case 'p': //paste
                        if (mclip) {
                            for (int x=0; x<mclipw; x++)
                                for (int y=0; y<mcliph; y++)
                                    *machineGrid->Gell(x+mx,y+my) = mclip[x+y*mclipw];
                        }
                        return true;

                    case 's': {
                        char *c = machineGrid->Gell(mx, my);
                        switch (*c) {
                            case MachineGrid::MachineWire:
                                *c = MachineGrid::MachineSpark;
                                break;
                            case MachineGrid::MachineSpark:
                                *c = MachineGrid::MachineHotWire;
                                break;
                            case MachineGrid::MachineHotWire:
                                *c = MachineGrid::MachineWire;
                                break;

                            case MachineGrid::MachineSWire:
                            case MachineGrid::MachineHotSWire:
                                *c = MachineGrid::MachineSSpark;
                                break;
                            case MachineGrid::MachineSSpark:
                                *c = MachineGrid::MachineSWire;
                                break;
                            case MachineGrid::MachineOWire:
                            case MachineGrid::MachineHotOWire:
                                *c = MachineGrid::MachineOSpark;
                                break;
                            case MachineGrid::MachineOSpark:
                                *c = MachineGrid::MachineOWire;
                                break;
                            case MachineGrid::MachineKiller:
                                *c = MachineGrid::KillerSpark;
                                break;
                            case MachineGrid::KillerSpark:
                                *c = MachineGrid::MachineKiller;
                                break;
                        }
                        return true;
                    }

                    case 'a':
                        {
                            char *c = machineGrid->Gell(mx, my);
                            switch (*c) {
                                case MachineGrid::MachineWire:
                                case MachineGrid::MachineHotWire:
                                    *c = MachineGrid::MachineSWire;
                                    break;
                                case MachineGrid::MachineSWire:
                                case MachineGrid::MachineHotSWire:
                                    *c = MachineGrid::MachineOWire;
                                    break;
                                case MachineGrid::MachineOWire:
                                case MachineGrid::MachineHotOWire:
                                    *c = MachineGrid::MachineLifer;
                                    break;
                                case MachineGrid::MachineLifer:
                                    *c = MachineGrid::MachineKiller;
                                    break;
                                case MachineGrid::MachineKiller:
                                case MachineGrid::KillerSpark:
                                    *c = MachineGrid::MachineClocker;
                                    break;
                                default:
                                case MachineGrid::MachineClocker:
                                    *c = MachineGrid::MachineWire;
                                    break;
                            }
                            OGLCONSOLE_Print("Incrementing %i,%i to %i\n", mx, my, *c);
                        }
                        return true;

                    case 'x':
                        {
                            char *c = machineGrid->Gell(mx, my);
                            switch (*c) {
                                case MachineGrid::MachineWire:
                                case MachineGrid::MachineHotWire:
                                    *c = MachineGrid::MachineClocker;
                                    break;
                                case MachineGrid::MachineOWire:
                                case MachineGrid::MachineHotOWire:
                                    *c = MachineGrid::MachineSWire;
                                    break;
                                case MachineGrid::MachineLifer:
                                    *c = MachineGrid::MachineOWire;
                                    break;
                                case MachineGrid::MachineKiller:
                                case MachineGrid::KillerSpark:
                                    *c = MachineGrid::MachineLifer;
                                    break;
                                default:
                                case MachineGrid::MachineClocker:
                                    *c = MachineGrid::MachineKiller;
                                    break;
                                case MachineGrid::MachineSWire:
                                case MachineGrid::MachineHotSWire:
                                    *c = MachineGrid::MachineWire;
                                    break;
                            }
                            OGLCONSOLE_Print("Incrementing %i,%i to %i\n", mx, my, *c);
                        }
                        return true;

                    case 'l':
                        lifeGrid->Clear();
                        return true;

                    case 'm':
                        machineGrid->Clear();
                        return true;

                    case 'g':
                        OGLCONSOLE_Print("Cursor position: %i,%i Life:%i Machine:%i\n", mx, my,
                                *lifeGrid->Gell(mx,my), *machineGrid->Gell(mx,my));
                        return true;
                }
        }
        return false;
    }

    void MakeLife(int x, int y)
    {
        *lifeGrid->Gell(x, y) = 1;
        *lifeGrid->Bell(x, y) = 1;
    }

    // C style
    bool Save(const char *fname)
    {
        return true;
    }
};

