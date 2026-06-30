// Copyright © 2026 CCP ehf.
#pragma once

// ASSERT_NO_THROW_MSG / EXPECT_NO_THROW_MSG — like gtest's ASSERT_NO_THROW / EXPECT_NO_THROW, but on a
// thrown C++ exception the failure names the exception's type and message instead of the bare
// "it throws". Handy for crash / regression tests where you want to know *what* was thrown.
//
//   ASSERT_NO_THROW_MSG( foo() );   // fatal: aborts the current test on throw — use inside a TEST body
//   EXPECT_NO_THROW_MSG( foo() );   // non-fatal: records and continues — also usable in helpers/callbacks
//                                   //   (the only form that compiles in a non-void function, e.g. a PyCFunction)
//
// Caveat: only C++ exceptions are catchable. A hard fault — access violation, or a fast-fail /
// heap-corruption abort (STATUS_STACK_BUFFER_OVERRUN) — still terminates the process; neither these
// macros nor gtest's own *_NO_THROW can intercept those.

#include "gtest/gtest.h"

#include <typeinfo>
#include <exception>

#define GTEST_NO_THROW_MSG_( statement, failureMacro )                                              \
	do                                                                                              \
	{                                                                                               \
		try                                                                                         \
		{                                                                                           \
			statement;                                                                              \
		}                                                                                           \
		catch( const std::exception& _ccpThrown )                                                   \
		{                                                                                           \
			failureMacro() << #statement " threw " << typeid( _ccpThrown ).name() << ": "           \
						   << _ccpThrown.what();                                                     \
		}                                                                                           \
		catch( ... )                                                                                \
		{                                                                                           \
			failureMacro() << #statement " threw a non-std::exception";                             \
		}                                                                                           \
	} while( 0 )

#define ASSERT_NO_THROW_MSG( statement ) GTEST_NO_THROW_MSG_( statement, FAIL )
#define EXPECT_NO_THROW_MSG( statement ) GTEST_NO_THROW_MSG_( statement, ADD_FAILURE )
