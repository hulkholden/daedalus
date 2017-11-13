/*
Copyright (C) 2005-2007 StrmnNrmn

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


#ifndef GRAPHICS_NATIVETEXTURE_H_
#define GRAPHICS_NATIVETEXTURE_H_

#include "Graphics/NativePixelFormat.h"
#include "Math/Vector2.h"
#include "Utility/RefCounted.h"

#ifdef DAEDALUS_GL
#include "SysGL/GL.h"
#endif

class c32;

class CNativeTexture : public CRefCounted
{
	friend class CRefPtr<CNativeTexture>::_NoAddRefRelease<CNativeTexture>;

		CNativeTexture( u32 w, u32 h );
		~CNativeTexture();

	public:
		static	CRefPtr<CNativeTexture>		Create( u32 width, u32 height );
		static	CRefPtr<CNativeTexture>		CreateFromPng( const char * p_filename );

		void							InstallTexture() const;

		void							SetData( NativePf8888 * data );

		inline u32						GetBlockWidth() const			{ return mTextureBlockWidth; }
		inline u32						GetWidth() const				{ return mWidth; }
		inline u32						GetHeight() const				{ return mHeight; }
		inline u32						GetCorrectedWidth() const		{ return mCorrectedWidth; }
		inline u32						GetCorrectedHeight() const		{ return mCorrectedHeight; }
		u32								GetStride() const;

		inline const NativePf8888 *		GetData() const					{ return mpData; }
		inline NativePf8888 *			GetData()						{ return mpData; }

		u32								GetBytesRequired() const;
		bool							HasData() const;				// If we run out of texture memory, this will return true

	private:
		u32					mWidth;
		u32					mHeight;
		u32					mCorrectedWidth;
		u32					mCorrectedHeight;
		u32					mTextureBlockWidth;		// Multiple of 16 bytes

		NativePf8888 *		mpData;

#ifdef DAEDALUS_GL
		GLuint				mTextureId;
#endif
};

#endif // GRAPHICS_NATIVETEXTURE_H_
