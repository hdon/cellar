#include "texture.h"
#ifdef __MACH__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

TextureManager::TextureManager(int count, const char **filenames)
{
	// Load our missingimage image
	printf("Loading \"data/textures/Missing.bmp\"...");
	NotFound = LoadTexture("data/textures/Missing.bmp");
	if (!NotFound) puts("ERROR: Failed to load Missing.bmp");
	else puts("");

	// Allocate list of gl texture names
	GL_TextureNames = new GLuint[count];

	// Fill list with gl texture names
	for (int i=0; i<count; i++)
	{
		printf("Loading \"%s\"...", filenames[i]);
		GLuint x = LoadTexture(filenames[i]);
		if (!x)
		{
			GL_TextureNames[i] = NotFound;
		}
		else
		{
			GL_TextureNames[i] = x;
		}
		puts("");
	}
}

TextureManager::~TextureManager()
{
	delete GL_TextureNames;
}

void TextureManager::operator[](int index)
{
	glBindTexture(GL_TEXTURE_2D, GL_TextureNames[index]);
}

int TextureManager :: LoadTexture (const char *filename)
{
	GLuint retval;
	SDL_Surface *sdlimage;
	void *raw;
	int w, h, bpp;
	Uint8 *srcPixel, *dstPixel;
	Uint32 truePixel;
	GLenum errorCode;

	sdlimage = IMG_Load(filename);

	if ( !sdlimage )
	{
		printf("IMG_Load() error: %s\n", IMG_GetError());
		return 0;
	}

	if ( sdlimage->format->BytesPerPixel < 2 )
	{
		puts("ERROR: Cannot load paletted images!");
		return 0;
	}

	w = sdlimage->w;
	h = sdlimage->h;

	raw = (void *)malloc( w * h * 4 );
	dstPixel = (Uint8 *)raw;

	SDL_LockSurface( sdlimage );

	bpp = sdlimage->format->BytesPerPixel;
	printf("Image is %ix%i with %i color bytes", w, h, bpp);

	for (int y = 0 ; y < h ; y++ )
	{
		for (int x = 0 ; x < w ; x++ )
		{
			srcPixel = (Uint8 *)sdlimage->pixels + y * sdlimage->pitch + x * bpp;
			switch (bpp)
			{
				case 1:
					truePixel = *srcPixel;
					break;
				
				case 2:
					truePixel = *(Uint16 *)srcPixel;
					break;
				
				case 3:
					if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
							truePixel = srcPixel[0] << 16 | srcPixel[1] << 8 | srcPixel[2];
					else	truePixel = srcPixel[0] | srcPixel[1] << 8 | srcPixel[2] << 16;
					break;

				case 4:
					truePixel = *(Uint32 *)srcPixel;
					break;
				
				default:
					printf("Image bpp of %d unusable\n", bpp);
					return 0;
					break;
			}

			SDL_GetRGBA( truePixel, sdlimage->format, &(dstPixel[0]), &(dstPixel[1]), &(dstPixel[2]), &(dstPixel[3]));
			dstPixel++;
			dstPixel++;
			dstPixel++;
			dstPixel++;
		}
	}

	SDL_UnlockSurface( sdlimage );
	SDL_FreeSurface( sdlimage );

	//while ( glGetError() ) { ; }

	glGenTextures( 1, &retval );
	glBindTexture( GL_TEXTURE_2D, retval );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	errorCode = glGetError();
	if ( errorCode != 0 )
	{
		if ( errorCode == GL_OUT_OF_MEMORY )
		{
			printf("ERROR: OpenGL: Out of texture memory!\n");
		}
	return 0;
	}

	gluBuild2DMipmaps( GL_TEXTURE_2D, 4, w, h, GL_RGBA, GL_UNSIGNED_BYTE, (Uint8 *)raw);

	errorCode = glGetError();
	if ( errorCode != 0 )
	{
		if ( errorCode == GL_OUT_OF_MEMORY )
		{
			printf("ERROR: OpenGL: Out of texture memory!\n");
		}
	return 0;
	}

	return retval;
}
