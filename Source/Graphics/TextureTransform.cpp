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
