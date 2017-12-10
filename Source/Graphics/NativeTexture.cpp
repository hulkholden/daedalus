/*
Copyright (C) 2013 StrmnNrmn

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "Base/Daedalus.h"
#include "Graphics/NativeTexture.h"

#include <stdlib.h>
#include <stdlib.h>
#include <string.h>

#include "external/libpng/png.h"

#include "Base/MathUtil.h"
#include "Graphics/ColourValue.h"
#include "Graphics/NativePixelFormat.h"
#include "Utility/Profiler.h"

static const u32 kMaxTextureSize = 100 * 1024 * 1024;

inline u32 CalcBytesRequired( u32 pixels )
{
	return pixels * 4;
}

static u32 GetTextureBlockWidth( u32 dimension )
{
	DAEDALUS_ASSERT( GetNextPowerOf2( dimension ) == dimension, "This is not a power of 2" );

	// Ensure that the pitch is at least 16 bytes
	while( CalcBytesRequired( dimension ) < 16 )
	{
		dimension *= 2;
	}

	return dimension;
}

static inline u32 CorrectDimension( u32 dimension )
{
	static const u32 MIN_TEXTURE_DIMENSION = 1;
	return Max( GetNextPowerOf2( dimension ), MIN_TEXTURE_DIMENSION );
}

CRefPtr<CNativeTexture>	CNativeTexture::Create( u32 width, u32 height )
{
	return new CNativeTexture( width, height );
}

CNativeTexture::CNativeTexture( u32 w, u32 h )
:	mWidth( w )
,	mHeight( h )
,	mCorrectedWidth( CorrectDimension( w ) )
,	mCorrectedHeight( CorrectDimension( h ) )
,	mTextureBlockWidth( GetTextureBlockWidth( mCorrectedWidth ) )
,	mpData( NULL )
,	mTextureId( 0 )
{
	glGenTextures( 1, &mTextureId );

	size_t data_len = GetBytesRequired();
	DAEDALUS_ASSERT(data_len < kMaxTextureSize, "Suspiciously large texture: %dx%d", w, h);
	mpData = reinterpret_cast<NativePf8888*>(malloc(data_len));
	memset(mpData, 0, data_len);
}

CNativeTexture::~CNativeTexture()
{
	if (mpData)
		free(mpData);

	glDeleteTextures( 1, &mTextureId );
}

bool CNativeTexture::HasData() const
{
	return mTextureId != 0;
}

void CNativeTexture::InstallTexture() const
{
	glBindTexture( GL_TEXTURE_2D, mTextureId );
}


namespace
{
	void ReadPngData( u32 width, u32 height, u32 stride, u8 ** p_row_table, int color_type, NativePf8888 * p_dest )
	{
		u8 r=0, g=0, b=0, a=0;

		for ( u32 y = 0; y < height; ++y )
		{
			const u8 * pRow = p_row_table[ y ];
			for ( u32 x = 0; x < width; ++x )
			{
				switch ( color_type )
				{
				case PNG_COLOR_TYPE_GRAY:
					r = g = b = *pRow++;
					if ( r == 0 && g == 0 && b == 0 )	a = 0x00;
					else								a = 0xff;
					break;
				case PNG_COLOR_TYPE_GRAY_ALPHA:
					r = g = b = *pRow++;
					if ( r == 0 && g == 0 && b == 0 )	a = 0x00;
					else								a = 0xff;
					pRow++;
					break;
				case PNG_COLOR_TYPE_RGB:
					b = *pRow++;
					g = *pRow++;
					r = *pRow++;
					if ( r == 0 && g == 0 && b == 0 )	a = 0x00;
					else								a = 0xff;
					break;
				case PNG_COLOR_TYPE_RGB_ALPHA:
					b = *pRow++;
					g = *pRow++;
					r = *pRow++;
					a = *pRow++;
					break;
				}

				p_dest[ x ] = NativePf8888( r, g, b, a );
			}

			p_dest = reinterpret_cast< NativePf8888 * >( reinterpret_cast< u8 * >( p_dest ) + stride );
		}
	}

	//*****************************************************************************
	//	Thanks 71M/Shazz
	//	p_texture is either an existing texture (in case it must be of the
	//	correct dimensions and format) else a new texture is created and returned.
	//*****************************************************************************
	CRefPtr<CNativeTexture>	LoadPng( const char * p_filename )
	{
		const size_t	kSignatureSize = 8;
		u8	signature[ kSignatureSize ];

		FILE * fh = fopen( p_filename, "rb" );
		if (fh == NULL)
		{
			return NULL;
		}

		if (fread( signature, sizeof(u8), kSignatureSize, fh ) != kSignatureSize)
		{
			fclose(fh);
			return NULL;
		}

		if (!png_check_sig( signature, kSignatureSize ))
		{
			return NULL;
		}

		png_struct * p_png_struct = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
		if (p_png_struct == NULL)
		{
			return NULL;
		}

		png_info * p_png_info = png_create_info_struct( p_png_struct );
		if (p_png_info == NULL)
		{
			png_destroy_read_struct( &p_png_struct, NULL, NULL );
			return NULL;
		}

		if (setjmp( png_jmpbuf(p_png_struct) ) != 0)
		{
			png_destroy_read_struct( &p_png_struct, NULL, NULL );
			return NULL;
		}

		png_init_io( p_png_struct, fh );
		png_set_sig_bytes( p_png_struct, kSignatureSize );
		png_read_png( p_png_struct, p_png_info, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_BGR, NULL );

		png_uint_32 width  = png_get_image_width( p_png_struct, p_png_info );
		png_uint_32 height = png_get_image_height( p_png_struct, p_png_info );

		CRefPtr<CNativeTexture>	texture = CNativeTexture::Create( width, height );

		DAEDALUS_ASSERT( texture->GetWidth() >= width, "Width is unexpectedly small" );
		DAEDALUS_ASSERT( texture->GetHeight() >= height, "Height is unexpectedly small" );

		NativePf8888 * data = reinterpret_cast< NativePf8888 * >( malloc(texture->GetBytesRequired()) );
		u32 	stride       = texture->GetStride();
		u8 ** 	row_pointers = png_get_rows( p_png_struct, p_png_info );
		int 	color_type   = png_get_color_type( p_png_struct, p_png_info );
		ReadPngData( width, height, stride, row_pointers, color_type, data );
		texture->SetData( data );

		free(data);
		png_destroy_read_struct( &p_png_struct, &p_png_info, NULL );
		fclose(fh);

		return texture;
	}
}

CRefPtr<CNativeTexture>	CNativeTexture::CreateFromPng( const char * p_filename )
{
	return LoadPng( p_filename );
}

void CNativeTexture::SetData( NativePf8888 * data )
{
	DAEDALUS_PROFILE("Texture SetData");

	// It's pretty gross that we don't pass this in, or better yet, provide a way for
	// the caller to write directly to our buffers instead of setting the data.
	size_t data_len = GetBytesRequired();
	memcpy(mpData, data, data_len);

	if (HasData())
	{
		glBindTexture( GL_TEXTURE_2D, mTextureId );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
					  mCorrectedWidth, mCorrectedHeight,
					  0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, data );
	}
}

u32	CNativeTexture::GetStride() const
{
	return CalcBytesRequired( mTextureBlockWidth );
}

size_t CNativeTexture::GetBytesRequired() const
{
	return static_cast<size_t>(GetStride()) * static_cast<size_t>(mCorrectedHeight);
}
