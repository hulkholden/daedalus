/*
Copyright (C) 2009 StrmnNrmn

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
#include "BatchTest.h"

#ifdef DAEDALUS_BATCH_TEST_ENABLED

#include <stdarg.h>

#include <vector>
#include <string>
#include <algorithm>

#include "Config/ConfigOptions.h"
#include "Core/CPU.h"
#include "Core/RSP_HLE.h"
#include "Debug/Dump.h"
#include "RomFile/RomFile.h"
#include "System/IO.h"
#include "System/SystemInit.h"
#include "System/Timing.h"
#include "Utility/Hash.h"
#include "Utility/Timer.h"

static void MakeRomList( const std::string& romdir, std::vector< std::string > & roms )
{
	IO::FindHandleT		find_handle;
	IO::FindDataT		find_data;
	if (IO::FindFileOpen( romdir, &find_handle, find_data ))
	{
		do
		{
			if ( IsRomFilename( find_data.Name ) )
			{
				roms.push_back( IO::Path::Join( romdir, find_data.Name ) );
			}
		}
		while(IO::FindFileNext( find_handle, find_data ));

		IO::FindFileClose( find_handle );
	}
}

static FILE * gBatchFH = NULL;
static FILE * gRomLogFH = NULL;
static CBatchTestEventHandler * gBatchTestEventHandler = NULL;

static EAssertResult BatchAssertHook( const char * expression, const char * file, unsigned int line, const char * msg, ... )
{
	char buffer[ 1024 ];
	va_list va;
	va_start(va, msg);
	vsnprintf( buffer, 1024, msg, va );
	buffer[1023] = 0;
	va_end(va);

	if( gBatchTestEventHandler )
	{
		return gBatchTestEventHandler->OnAssert( expression, file, line, buffer );
	}

	return AR_IGNORE;
}

static void BatchVblHandler( void * arg )
{
	gBatchTestEventHandler->OnVerticalBlank();
}

static void BatchDisplayListHandler( void * arg )
{
	gBatchTestEventHandler->OnDisplayListComplete();
}

static std::string MakeNewLogFilename( const std::string& rundir )
{
	u32 count = 0;
	std::string filepath;
	do
	{
		char filename[64];
		sprintf( filename, "log%04d.txt", count );
		++count;

		filepath = IO::Path::Join(rundir, filename);
	}
	while( IO::File::Exists( filepath ) );
	return filepath;
}

static std::string SprintRunDirectory( const std::string& batchdir, u32 run_id )
{
	char filename[64];
	sprintf( filename, "run%04d", run_id );
	return IO::Path::Join(batchdir, filename);
}

static bool MakeRunDirectory( std::string* rundir, const std::string& batchdir )
{
	// Find an unused directory
	for ( u32 run_id = 0; run_id < 100; ++run_id )
	{
		std::string dir = SprintRunDirectory( batchdir, run_id );

		// Skip if it already exists as a file or directory
		if( IO::Directory::Create( dir ) )
		{
			*rundir = dir;
			return true;
		}
	}

	return false;
}

void BatchTestMain( int argc, char* argv[] )
{
	// TODO: Allow other directories and configuration
	const std::string romdir = g_DaedalusConfig.RomsDir;

	bool	random_order( false );		// Whether to randomise the order of processing, to help avoid hangs
	bool	update_results( false );	// Whether to update existing results
	s32		run_id( -1 );				// New run by default

	for(int i = 1; i < argc; ++i )
	{
		const char * arg( argv[i] );
		if( *arg == '-' )
		{
			++arg;
			if( strcmp( arg, "rand" ) == 0 || strcmp( arg, "random" ) == 0 )
			{
				random_order = true;
			}
			else if( strcmp( arg, "u" ) == 0 || strcmp( arg, "update" ) == 0 )
			{
				update_results = true;
			}
			else if( strcmp( arg, "r" ) == 0 || strcmp( arg, "run" ) == 0 )
			{
				if( i+1 < argc )
				{
					++i;	// Consume next arg
					run_id = atoi( argv[i] );
				}
			}
		}
	}

	std::string batchdir = Dump_GetDumpDirectory("batch");

	std::string rundir;
	if( run_id < 0 )
	{
		if( !MakeRunDirectory( &rundir, batchdir ) )
		{
			printf( "Couldn't start a new run\n" );
			return;
		}
	}
	else
	{
		rundir = SprintRunDirectory( batchdir, run_id );
		if( !IO::Directory::IsDirectory( rundir ) )
		{
			printf( "Couldn't resume run %d\n", run_id );
			return;
		}
	}

	gBatchTestEventHandler = new CBatchTestEventHandler();

	std::string logpath = MakeNewLogFilename( rundir );
	gBatchFH = fopen(logpath.c_str(), "w");
	if( !gBatchFH )
	{
		printf( "Unable to open '%s' for writing", logpath.c_str() );
		return;
	}

	std::vector< std::string > roms;
	MakeRomList( romdir, roms );

	u64 time;
	if( NTiming::GetPreciseTime( &time ) )
	{
		srand( (int)time );
	}

	CTimer	timer;

	//	Set up an assert hook to capture all asserts
	SetAssertHook( BatchAssertHook );

	// Hook in our Vbl handler.
	CPU_RegisterVblCallback( &BatchVblHandler, nullptr );
	RSP_HLE_RegisterDisplayListCallback( &BatchDisplayListHandler, nullptr );

	std::string tmpfilepath = IO::Path::Join(rundir, "tmp.tmp");

	while( !roms.empty() )
	{
		gBatchTestEventHandler->Reset();

		u32 idx = 0;

		// Picking roms in a random order means we can work around roms which crash the emulator a little more easily
		if( random_order )
		{
			idx = rand() % roms.size();
		}

		std::string	rom_to_load;
		rom_to_load.swap( roms[idx] );
		roms.erase( roms.begin() + idx );

		// Make a filename of the form: '<rundir>/<romfilename>.txt'
		std::string rom_logpath = IO::Path::Join(rundir, IO::Path::FindFileName( rom_to_load ));
		IO::Path::SetExtension(&rom_logpath, ".txt");

		bool result_exists = IO::File::Exists(rom_logpath);

		if( !update_results && result_exists )
		{
			// Already exists, skip
			fprintf( gBatchFH, "\n\n%#.3f: Skipping %s - log already exists\n", timer.GetElapsedSecondsSinceReset(), rom_to_load.c_str() );
		}
		else
		{
			fprintf( gBatchFH, "\n\n%#.3f: Processing: %s\n", timer.GetElapsedSecondsSinceReset(), rom_to_load.c_str() );

			gRomLogFH = fopen( tmpfilepath.c_str(), "w" );
			if( !gRomLogFH )
			{
				fprintf( gBatchFH, "#%.3f: Unable to open temp file\n", timer.GetElapsedSecondsSinceReset() );
			}
			else
			{
				fflush( gBatchFH );

				// TODO: use ROM_GetRomDetailsByFilename and the alternative form of ROM_LoadFile with overridden preferences (allows us to test if roms break by changing prefs)
				System_Open( rom_to_load );

				CPU_Run();

				System_Close();

				const char * reason( CBatchTestEventHandler::GetTerminationReasonString( gBatchTestEventHandler->GetTerminationReason() ) );

				fprintf( gBatchFH, "%#.3f: Finished running: %s - %s\n", timer.GetElapsedSecondsSinceReset(), rom_to_load.c_str(), reason );

				// Copy temp file over rom_logpath
				gBatchTestEventHandler->PrintSummary( gRomLogFH );
				fclose( gRomLogFH );
				gRomLogFH = nullptr;
				if( result_exists )
				{
					IO::File::Delete(rom_logpath);
				}
				if( !IO::File::Move(tmpfilepath, rom_logpath) )
				{
					fprintf( gBatchFH, "%#.3f: Coping %s -> %s failed\n", timer.GetElapsedSecondsSinceReset(), tmpfilepath.c_str(), rom_logpath.c_str() );
				}
			}

		}

	}

	RSP_HLE_UnregisterDisplayListCallback( &BatchDisplayListHandler, nullptr );
	CPU_UnregisterVblCallback( &BatchVblHandler, nullptr );
	SetAssertHook( nullptr );

	fclose( gBatchFH );
	gBatchFH = nullptr;

	delete gBatchTestEventHandler;
	gBatchTestEventHandler = nullptr;
}

// Should make these configurable
const u32 MAX_DLS = 10;
const u32 MAX_VBLS_WITHOUT_DL = 1000;
const f32 BATCH_TIME_LIMIT = 60.0f;

CBatchTestEventHandler::CBatchTestEventHandler()
:	mNumDisplayListsCompleted( 0 )
,	mNumVerticalBlanksSinceDisplayList( 0 )
,	mTerminationReason( TR_UNKNOWN )
{

}

void CBatchTestEventHandler::Reset()
{
	mNumDisplayListsCompleted = 0;
	mNumVerticalBlanksSinceDisplayList = 0;
	mTimer.Reset();
	mTerminationReason = TR_UNKNOWN;
	mAsserts.clear();
}

void CBatchTestEventHandler::Terminate( ETerminationReason reason )
{
	mTerminationReason = reason;
	CPU_Halt( "End of batch run" );
}

void CBatchTestEventHandler::OnDisplayListComplete()
{
	++mNumDisplayListsCompleted;
	mNumVerticalBlanksSinceDisplayList = 0;
	if( MAX_DLS != 0 && mNumDisplayListsCompleted >= MAX_DLS )
	{
		Terminate( TR_REACHED_DL_COUNT );
	}
}

void CBatchTestEventHandler::OnVerticalBlank()
{
	++mNumVerticalBlanksSinceDisplayList;
	if( mNumVerticalBlanksSinceDisplayList > MAX_VBLS_WITHOUT_DL )
	{
		Terminate( TR_TOO_MANY_VBLS_WITH_NO_DL );
	}

	if( mTimer.GetElapsedSecondsSinceReset() > BATCH_TIME_LIMIT )
	{
		Terminate( TR_TIME_LIMIT_REACHED );
	}
}

EAssertResult CBatchTestEventHandler::OnAssert( const char * expression, const char * file, unsigned int line, const char * formatted_msg )
{
	u32		assert_hash( murmur2_hash( (const u8 *)file, strlen( file ), line ) );

	std::vector<u32>::iterator	it( std::lower_bound( mAsserts.begin(), mAsserts.end(), assert_hash ) );
	if( it == mAsserts.end() || *it != assert_hash )
	{
		if( gRomLogFH )
		{
			fprintf( gRomLogFH, "! Assert Failed: Location: %s(%d), [%s] %s\n", file, line, expression, formatted_msg );
		}

		mAsserts.insert( it, assert_hash );
	}

	// Don't return AR_IGNORE as this prevents asserts firing for subsequent roms
	return AR_IGNORE_ONCE;
}

void CBatchTestEventHandler::OnDebugMessage( const char * msg )
{
	if( gRomLogFH )
	{
		fputs( msg, gRomLogFH );
	}
}

const char * CBatchTestEventHandler::GetTerminationReasonString( ETerminationReason reason )
{
	switch( reason )
	{
	case TR_UNKNOWN:						return "Unknown";
	case TR_REACHED_DL_COUNT:				return "Reached display list count";
	case TR_TIME_LIMIT_REACHED:				return "Time limit reached";
	case TR_TOO_MANY_VBLS_WITH_NO_DL:		return "Too many vertical blanks without a display list";
	}

	DAEDALUS_ERROR( "Unhandled reason" );
	return "Unknown";
}

void CBatchTestEventHandler::PrintSummary( FILE * fh )
{
	bool			success( mTerminationReason >= 0 );
	const char *	reason( GetTerminationReasonString( mTerminationReason ) );

	fprintf( fh, "\n\nSummary:\n--------\n\n" );
	fprintf( fh, "Termination Reason: [%s] - %s\n", success ? " OK " : "FAIL", reason );
	fprintf( fh, "Display Lists Completed: %d / %d\n", mNumDisplayListsCompleted, MAX_DLS );
}


#endif // DAEDALUS_BATCH_TEST_ENABLED
