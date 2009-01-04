#ifndef _TEXTURE_H
#define _TEXTURE_H
#ifdef __MACH__
#  include <OpenGL/gl.h>
#else
#  include <GL/gl.h>
#endif

class TextureManager
{
	private:
		GLuint *GL_TextureNames, NotFound;
		int LoadTexture(const char *filename);

	public:
		TextureManager(int count, const char **filenames);
		~TextureManager();

		inline void Bind(int index) {operator[](index);};
		void operator[](int index);
};

#endif/*_TEXTURE_H*/
