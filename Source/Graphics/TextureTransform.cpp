#include "Base/Daedalus.h"
#include "TextureTransform.h"

#include <string.h>

#include "ColourValue.h"
#include "NativePixelFormat.h"

void ClampTexels( NativePf8888 * data, u32 n64_width, u32 n64_height, u32 native_width, u32 native_height, u32 native_stride )
{
	DAEDALUS_ASSERT( native_stride >= native_width * sizeof( NativePf8888 ), "Native stride isn't big enough" );
	DAEDALUS_ASSERT( n64_width <= native_width, "n64 width greater than native width?" );
	DAEDALUS_ASSERT( n64_height <= native_height, "n64 height greater than native height?" );

	// If any of the rows are short, we need to duplicate the last pixel on the row
	// Stick this in an outer predicate incase they match
	NativePf8888 * p = data;
	if( native_width > n64_width )
	{
		for( u32 y = 0; y < n64_height; ++y )
		{
			NativePf8888 colour = p[ n64_width - 1 ];

			for( u32 x = n64_width; x < native_width; ++x )
			{
				p[ x ] = colour;
			}

			p = AddByteOffset( p, native_stride );
		}
	}
	else
	{
		p = AddByteOffset( p, n64_height * native_stride );
	}

	//
	//	At this point all the rows up to the n64 height have been padded out.
	//	We need to duplicate the last row for every additional native row.
	//
	if( native_height > n64_height )
	{
		const void * last_row = AddByteOffset( data, ( n64_height - 1 ) * native_stride );

		for( u32 y = n64_height; y < native_height; ++y )
		{
			memcpy( p, last_row, native_stride );

			p = AddByteOffset( p, native_stride );
		}
	}
}

static void CopyRow( NativePf8888 * dst, const NativePf8888 * src, u32 pixels )
{
	memcpy( dst, src, pixels * sizeof( NativePf8888 ) );
}

static void CopyRowReverse( NativePf8888 * dst, const NativePf8888 * src, u32 pixels )
{
	u32 last_pixel = pixels * 2 - 1;

	for( u32 i = 0; i < pixels; ++i )
	{
		dst[ last_pixel - i ] = src[ i ];
	}
}


// Assumes width p_dst = 2*width p_src and height p_dst = 2*height p_src
template< bool MirrorS, bool MirrorT >
static void MirrorTexels( NativePf8888 * dst, u32 dst_stride, const NativePf8888 * src, u32 src_stride, u32 width, u32 height )
{
	for( u32 y = 0; y < height; ++y )
	{
		// Copy regular pixels
		CopyRow( dst, src, width );

		if( MirrorS )
		{
			CopyRowReverse( dst, src, width );
		}

		dst = AddByteOffset( dst, dst_stride );
		src = AddByteOffset( src, src_stride );
	}

	if( MirrorT )
	{
		// Copy remaining rows in reverse order
		for( u32 y = 0; y < height; ++y )
		{
			src = AddByteOffset( src, -s32(src_stride) );

			// Copy regular pixels
			CopyRow( dst, src, width );

			if( MirrorS )
			{
				CopyRowReverse( dst, src, width );
			}

			dst = AddByteOffset( dst, dst_stride );
		}
	}
}

void MirrorTexels( bool mirror_s, bool mirror_t,
				   NativePf8888 * dst, u32 dst_stride,
				   const NativePf8888 * src, u32 src_stride,
				   u32 width, u32 height )
{
	if( mirror_s && mirror_t )
	{
		MirrorTexels< true, true >( dst, dst_stride, src, src_stride, width, height );
	}
	else if( mirror_s )
	{
		MirrorTexels< true, false >( dst, dst_stride, src, src_stride, width, height );
	}
	else if( mirror_t )
	{
		MirrorTexels< false, true >( dst, dst_stride, src, src_stride, width, height );
	}
}
