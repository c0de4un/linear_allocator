/*
 * Copyright © 2018 Denis Zyamaev. Email: (code4un@yandex.ru)
 * License: see "LICENSE" file
 * Author: Denis Zyamaev (code4un@yandex.ru)
 * API: C++ 11
*/

// DEBUG
#ifdef DEBUG
#define __linear_allocator_debug_enabled_
#endif

// DEBUG
// Include STL
#include <iostream> // cout, cin, cin.get
#include <cstdlib> // std

// Include linear_allocator
#include "linear_allocator.hpp"

/*
 * Linear-Allocator tests.
*/
static void linear_allocator_test( )
{

	// Create linear_allocator instance
	linear_allocator<double> allocator_( 16 );

	// Print available blocks count
	std::cout << "linear allocator available block=" << allocator_.available_size( ) << std::endl;

	// Allocate 1 object
	double *const n_ = allocator_.allocate( );

	// Construct double
	allocator_.construct( n_, 16 );

	// Change value
	*n_ = 777.7;

	// Print available blocks count
	std::cout << "linear allocator available block=" << allocator_.available_size( ) << " after allocation of 1 object" << std::endl;

	// Deallocate & destroy
	allocator_.deallocate( n_ );

	// Print available blocks count
	std::cout << "linear allocator available block=" << allocator_.available_size( ) << " after deallocation of 1 object" << std::endl;

}

/* MAIN */
int main( int argC, char** argV )
{

	// Print 'Hello Linear Allocator' to the console
	std::cout << "Hello Linear Allocator" << std::endl;
	
	// Run linear_allocator tests
	linear_allocator_test( );

	// Print 'Linear Allocator Test Complete' to the console
	std::cout << "Linear Allocator Test Complete, press any key to exit" << std::endl;

	// Pause Console-Window
	std::cin.get( );

	// Return OK
	return( 0 );

}