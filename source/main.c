#include <string.h>
#include <stdio.h>

#include <switch.h>

#include <ft2build.h>
#include FT_FREETYPE_H

void my_draw_bitmap( FT_Bitmap * bitmap, FT_Int x, FT_Int y)
{
	u32 screenwidth, screenheight;
	u32 * FRAMEBUFFER = gfxGetFramebuffer((u32*)&screenwidth, (u32*)&screenheight);

	FT_UInt ix, iy;
	u32 pos;
	u32 color;
	
	for (iy = 0; iy < bitmap->rows; iy++)
	{
		for (ix = 0; ix < bitmap->width; ix++)
		{
			pos = (iy + y) * screenwidth + x + ix;
			color = bitmap->buffer[iy * bitmap->width + ix];
	
			FRAMEBUFFER[pos] |= color; //it's all just gonna be split to RGBA anyways
		}
	}
}

void print_text(FT_Face face, const char * text, float x, float y)
{
	int pen_x = x;
	int pen_y = y;

	FT_GlyphSlot slot = face->glyph;
	uint num_chars = strlen(text);
	int error;

	for (uint n = 0; n < num_chars; n++ )
	{
		FT_UInt  glyph_index;


		/* retrieve glyph index from character code */
		glyph_index = FT_Get_Char_Index( face, text[n] );

		/* load glyph image into the slot (erase previous one) */
		error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );
		if ( error )
			continue;  /* ignore errors */

		/* convert to an anti-aliased bitmap */
		error = FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );
		if ( error )
			continue;

		/* now, draw to our target surface */
		my_draw_bitmap( &slot->bitmap,
						pen_x + slot->bitmap_left,
						pen_y - slot->bitmap_top );

		/* increment pen position */
		pen_x += slot->advance.x >> 6;
		pen_y += slot->advance.y >> 6; /* not useful for now */
	}
}

int main(int argc, char **argv)
{
	gfxInitDefault();

	FT_Library library;
	FT_Init_FreeType(&library);

	FT_Face face;
	FT_New_Face(library, "sdmc:/vera.ttf", 0, &face);

	int size = 14;
	FT_Set_Char_Size(face, 0, size << 6, 72, 72);

	while(appletMainLoop())
	{
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();

		//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
		u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

		if (kDown & KEY_PLUS) break; // break in order to return to hbmenu

		print_text(face, "The quick brown fox jumps over the lazy doge", 10, 10);

		gfxFlushBuffers();
		gfxSwapBuffers();
		gfxWaitForVsync();
	}

	gfxExit();

	return 0;
}

