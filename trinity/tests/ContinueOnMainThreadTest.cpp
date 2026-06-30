#include "StdAfx.h"
#include "gtest/gtest.h"

#include "ContinueOnMainThread.h"

#include "TestExceptionMacros.h"

namespace
{
int g_peerReentrantDrains = 0;
bool g_peerRanDuringDrain = false;

PyObject* PeerReentrantDrain( PyObject*, PyObject* )
{
	g_peerReentrantDrains++;
	EXPECT_NO_THROW_MSG( ExecuteMainThreadActions() );
	Py_RETURN_NONE;
}

PyObject* MarkPeerRan( PyObject*, PyObject* )
{
	g_peerRanDuringDrain = true;
	Py_RETURN_NONE;
}

PyMethodDef g_yieldTestMethods[] = {
	{ "_peer_reentrant_drain", PeerReentrantDrain, METH_NOARGS, "Re-enter the drain from a peer tasklet." },
	{ "_mark_peer_ran", MarkPeerRan, METH_NOARGS, "Record that a peer tasklet ran." },
	{ nullptr, nullptr, 0, nullptr },
};
}

TEST( ContinueOnMainThread, Recursive )
{
	int firstRuns = 0;
	int secondRuns = 0;
	bool reentered = false;

	ContinueOnMainThread( [&] {
		firstRuns++;
		if( !reentered )
		{
			reentered = true;
			ExecuteMainThreadActions();
		}
	} );
	ContinueOnMainThread( [&] { secondRuns++; } );

	ExecuteMainThreadActions();
	ExecuteMainThreadActions();

	EXPECT_EQ( 1, firstRuns );
	EXPECT_EQ( 1, secondRuns );
}

TEST( ContinueOnMainThread, NonRecursive )
{
	int runs = 0;
	ContinueOnMainThread( [&] { runs++; } );
	ContinueOnMainThread( [&] { runs++; } );

	ExecuteMainThreadActions();
	ExecuteMainThreadActions();

	EXPECT_EQ( 2, runs );
}

TEST( ContinueOnMainThread, CrashRecursive )
{
	static int total = 0;
	bool reentered = false;

	ContinueOnMainThread( [&] {
		total++;
		if( reentered )
		{
			return;
		}
		reentered = true;

		ContinueOnMainThread( [] {
			for( int i = 0; i < 1000; i++ )
			{
				ContinueOnMainThread( [] { total++; } );
			}
		} );

		ExecuteMainThreadActions();
	} );
	ContinueOnMainThread( [] { total++; } );

	ASSERT_NO_THROW_MSG( ExecuteMainThreadActions() );
	ASSERT_NO_THROW_MSG( ExecuteMainThreadActions() );

	EXPECT_EQ( 1002, total );
}

TEST( ContinueOnMainThread, TaskletYield )
{
	g_peerReentrantDrains = 0;
	int firstRuns = 0;
	int secondRuns = 0;

	Py_InitModule( "_cmt_yieldtest", g_yieldTestMethods );

	ContinueOnMainThread( [&] {
		firstRuns++;
		PyRun_SimpleString(
			"import stackless, _cmt_yieldtest\n"
			"stackless.tasklet( _cmt_yieldtest._peer_reentrant_drain )()\n"
			"stackless.schedule()\n" );
	} );
	ContinueOnMainThread( [&] { secondRuns++; } );

	ExecuteMainThreadActions();
	ExecuteMainThreadActions();

	EXPECT_EQ( 1, firstRuns );
	EXPECT_EQ( 1, secondRuns );
	EXPECT_GE( g_peerReentrantDrains, 1 );
}

TEST( ContinueOnMainThread, CrashTaskletYield )
{
	static int total = 0;
	total = 0;

	Py_InitModule( "_cmt_yieldtest", g_yieldTestMethods );

	ContinueOnMainThread( [] {
		total++;
		ContinueOnMainThread( [] {
			for( int i = 0; i < 1000; i++ )
			{
				ContinueOnMainThread( [] { total++; } );
			}
		} );
		PyRun_SimpleString(
			"import stackless, _cmt_yieldtest\n"
			"stackless.tasklet( _cmt_yieldtest._peer_reentrant_drain )()\n"
			"stackless.schedule()\n" );
	} );
	ContinueOnMainThread( [] { total++; } );

	ASSERT_NO_THROW_MSG( ExecuteMainThreadActions() );
	ASSERT_NO_THROW_MSG( ExecuteMainThreadActions() );

	EXPECT_EQ( 1002, total );
}

TEST( ContinueOnMainThread, BlockingYieldIsTrapped )
{
	g_peerRanDuringDrain = false;
	Py_InitModule( "_cmt_yieldtest", g_yieldTestMethods );

	ContinueOnMainThread( [] {
		PyRun_SimpleString(
			"import stackless, _cmt_yieldtest\n"
			"ch = stackless.channel()\n"
			"def _peer():\n"
			"    _cmt_yieldtest._mark_peer_ran()\n"
			"    if ch.balance < 0:\n"
			"        ch.send( 1 )\n"
			"stackless.tasklet( _peer )()\n"
			"try:\n"
			"    ch.receive()\n"
			"except RuntimeError:\n"
			"    pass\n" );
	} );

	ExecuteMainThreadActions();

	EXPECT_FALSE( g_peerRanDuringDrain )
		<< "a peer tasklet ran mid-drain — a blocking yield was not trapped (ScopedBlockTrap missing?)";
}
