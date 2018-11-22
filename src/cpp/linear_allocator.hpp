/*
 * Copyright © 2018 Denis Zyamaev. Email: (code4un@yandex.ru)
 * License: see "LICENSE" file
 * Author: Denis Zyamaev (code4un@yandex.ru)
 * API: C++ 11
*/

/* ALLOCATORS REQUIRED HEADERS */

#include <cstdlib> // malloc & free
#include <cstddef> // size_t
#include <new> // new, std::bad_alloc
#include <stdexcept> // std::length_error
#include <bitset> // bitset
#include <map> // map

#ifdef __linear_allocator_debug_enabled_ // DEBUG

#include <iostream> // cout, cin, cin.get
#include <cstdlib> // std
#include <string> // to_string

#endif // DEBUG

/* END OF ALLOCATORS REQUIRED HEADERS */

/*
 * linear_allocator - linear allocator with fixed size.
 * 
 * @config
 * - __linear_allocator_debug_enabled_ - enable log-output using STL cout & cin.
*/
template <typename T>
class linear_allocator
{

private:

	// -------------------------------------------------------- \\

	// ===========================================================
	// Config
	// ===========================================================

	/* Objects (items) limit (max.) */
	static constexpr std::size_t OBJECTS_LIMIT = 320;

	// -------------------------------------------------------- \\

public:

	// -------------------------------------------------------- \\

	// ===========================================================
	// Types
	// ===========================================================

	/* value_type type-alias for libstdc++ */
	using value_type = T;

	/* type-alias for ptrdiff_t, required by STL libstdc++ */
	using difference_type = std::ptrdiff_t;

	/* pointer type-alias for libstdc++ */
	using pointer = value_type * ;

	/* const pointer type-alias for libstdc++ */
	using const_pointer = const pointer;

	/* reference type-alias for libstdc++ */
	using reference = value_type & ;

	/* const reference type-alias for libstdc++ */
	using const_reference = const reference;

	/* size_type type-alias for libstdc++ */
	using size_type = std::size_t;

	// ===========================================================
	// Constructors
	// ===========================================================

	/*
	 * linear_allocator constructor.
	 * Used to support stateless allocator.
	 *
	 * @param pCount_ - objects (items, elements) limit.
	*/
	linear_allocator( const std::size_t & pCount_ = OBJECTS_LIMIT )
		: count_( pCount_ ),
		elementSize_( sizeof( T ) ),
		available_count_( count_ ),
		buffer_( nullptr ),
		blocks_status_( ),
		reserved_blocks_indices_( ),
		freedIndex_( 0 )
	{

#ifdef __linear_allocator_debug_enabled_ // DEBUG
		// Print message
		std::cout << "linear_allocator::constructor; elements: " << count_ << "; element_size=" << elementSize_ << "total_size=" << count_ * elementSize_ << std::endl;
#endif // DEBUG

		// Allocate buffer
		buffer_ = static_cast<unsigned char*>( std::malloc( ( elementSize_ * count_ ) * sizeof( unsigned char ) ) );

		// Check allocation
		if ( buffer_ == nullptr )
			throw std::bad_alloc( );

	}

	/*
	 * linear_allocator const copy constructor, required by STL.
	*/
	linear_allocator( const linear_allocator & pOther )
	{

#ifdef __linear_allocator_debug_enabled_ // DEBUG
		// Print message
		std::cout << "linear_allocator::const copy constructor" << std::endl;
#endif // DEBUG

	}

	// ===========================================================
	// Destructor
	// ===========================================================

	/* linear_allocator destructor */
	~linear_allocator( )
	{

#ifdef __linear_allocator_debug_enabled_ // DEBUG
		// Print message
		std::cout << "linear_allocator::destructor" << std::endl;
#endif // DEBUG

		// Release buffer
		std::free( buffer_ );

	}

	// ===========================================================
	// Methods
	// ===========================================================

	/* Returns address for reference */
	T * address( T & pRef ) const
	{ return( &pRef ); }

	/* Returns address for const reference */
	const T * address( const T & pRef ) const
	{ return( &pRef ); }

	/*
	 * Returns max available size (number, amount, count) of elements (items, blocks).
	 *
	 * (!) Result depends on platform. x86-32 Intel has 4-bytes per int, when x86-64 has 8
	 * bytes per int. Also pointers size will very.
	 *
	 * (?) Based on mallocator. Allows to avoid CPU (ABI) specific
	 * differences on calculating size.
	 *
	 * (?) Allows to avoid dependency from definition of size_t and to avoid signed/unsigned warnings.
	 *
	 * @thread_safety - thread-safe.
	*/
	const size_type max_size( ) const noexcept
	{ return( static_cast<std::size_t>( -1 ) / sizeof( T ) ); }

	/* Returns available blocks count */
	const size_type available_size( ) const noexcept
	{ return( available_count_ ); }

	/* Returns reserved blocks count */
	const size_type reserved_size( ) const noexcept
	{ return( count_ - available_count_ ); }

	/*
	 * Allocates given amount of objects (elements)
	 * & returns pointer to first element.
	 * 
	 * @thread_safety - not thread-safe.
	 * @param pCount - number of elements (size, count).
	 * @throws - can throw std::bad_alloc
	*/
	T * allocate( const size_type pCount = 1, const void *const = 0 )
	{
		
#ifdef __linear_allocator_debug_enabled_ // DEBUG
		// Print message
		std::cout << "linear_allocator::allocate - allocating " << pCount << " objects, already allocated:" << count_ << " objects." << std::endl;
#endif // DEBUG

		// Check if exceeded
		if ( available_count_ < 1 )
			throw std::length_error( "linear_allocator::allocate - maximum objects exceeded" );

		// Check allocation-size
		if ( pCount > 1 )
			throw std::length_error( "linear_allocator::allocate - this allocator supports only one object allocation at once" );
		else if ( pCount < 1 )
			return( nullptr );

		// Decrease available blocks counter
		available_count_ -= pCount;
		
		// Search Available block
		if ( freedIndex_ > 0 )
		{

			// Check if last freed block still available
			if ( !blocks_status_.test( freedIndex_ ) )
			{// Available

				// Pointer (address, offset) to the block
				void *const ptr_( buffer_ + ( freedIndex_ * elementSize_ ) );

#ifdef __linear_allocator_debug_enabled_ // DEBUG
		// Print message
				std::cout << "linear_allocator::allocate - reserving again, lately freed block #" << std::to_string( freedIndex_ ) << " ; address=" << ptr_ << std::endl;
#endif // DEBUG

				// Reserve
				blocks_status_.set( freedIndex_, true );

				// Add index to the reserved blocks map
				reserved_blocks_indices_[static_cast<const void *const>( ptr_ )] = freedIndex_;

				// Reset last freed block
				freedIndex_ = 0;

				// Return pointer to the offset-address
				return( static_cast<pointer>( ptr_ ) );

			}
			else // Somehow block got reserved.
				freedIndex_ = 0;
		}

		// Block index
		size_type i = 0;
		
		// Search available block
		while ( i < count_ )
		{

			// Check block status
			if ( !blocks_status_.test( i ) )
			{// Available

				// Pointer (address, offset) to the block
				void *const ptr_( buffer_ + ( freedIndex_ * elementSize_ ) );

#ifdef __linear_allocator_debug_enabled_ // DEBUG
		// Print message
				std::cout << "linear_allocator::allocate - reserving block #" << std::to_string( i ) << " ; address=" << ptr_ << std::endl;
#endif // DEBUG

				// Reserve
				blocks_status_.set( i, true );

				// Add index to the reserved blocks map
				reserved_blocks_indices_[static_cast<const void *const>( ptr_ )] = i;

				// Return pointer to the offset-address
				return( static_cast<pointer>( ptr_ ) );

			}

			// Next
			i++;

		}

		// Throw bad_alloc
		throw std::bad_alloc( );

	}

	/*
	 * Deallocate.
	 *
	 * (!) This method calls destructor. Don't use the given object.
	 *
	 * (!) This allocator supports only one block (address, pointer) allocation/deallocation
	 * at one time, to provide alignment of memory & fast access.
	 *
	 * @param ptr_ - pointer/offset to the block of memory.
	 * @param size_ - number of objects (blocks) to deallocate from
	 * the given offset (pointer, address).
	*/
	void deallocate( pointer ptr_, const size_type size_ = 1 )
	{

		// Destroy
		destroy( ptr_ );

		// Get block index
		const size_type & index_ = reserved_blocks_indices_[static_cast<const void *const>( ptr_ )];

#ifdef __linear_allocator_debug_enabled_ // DEBUG
		// Print message
		std::cout << "linear_allocator::deallocate - freeing block #" << std::to_string( index_ ) << std::endl;
#endif // DEBUG

		// Mark block as available
		blocks_status_.set( index_, false );
		freedIndex_ = index_;

		// Remove block index from reserved map
		reserved_blocks_indices_.erase( static_cast<const void *const>( ptr_ ) );

		// Increase available blocks counter
		available_count_ += size_;

	}

	template <typename... _Args>
	void construct( const_pointer ptr_, _Args&&... args_ )
	{
		new( (void*) ptr_ ) T( std::forward<_Args>( args_ )... );
	}
	
	void destroy( pointer ptr_ )
	{
		ptr_->~T( );
	}

	// ===========================================================
	// Operators
	// ===========================================================

	/* Compare linear_allocators */
	const bool operator!=( const linear_allocator & pOther ) const noexcept
	{ return( *this != pOther ); }

	/*
	 * Returns 'TRUE' if this storage allocator can be deallocated
	 * from the other allocator, and other-way also (vise versa).
	 * 
	 * @return - 'TRUE', because this is stateless allocator.
	*/
	const bool operator==( const linear_allocator & pOther ) const noexcept
	{ return( true ); }

	// -------------------------------------------------------- \\

private:

	// -------------------------------------------------------- \\

	// ===========================================================
	// Constants
	// ===========================================================

	/* Elements (items, blocks) count */
	const std::size_t count_;

	/* Max. (limit) number of objects */
	//const std::size_t objectsLimit_;

	/* Size (length) in bytes of the element (item, object) */
	const std::size_t elementSize_;

	/* Total size in bytes [count * size]. Can't be exceeded. */
	//const std::size_t sizeLimit_;

	// ===========================================================
	// Fields
	// ===========================================================

	/* Number of available blocks (items, objects). */
	std::size_t available_count_;

	/* Buffer */
	unsigned char * buffer_;

	/*
	 * Cache to store blocks status.
	*/
	std::bitset<OBJECTS_LIMIT> blocks_status_;

	/*
	 * Last freed block index.
	*/
	size_type freedIndex_;

	/*
	 * Map to store reserved blocks indices.
	 *
	 * Allows to release specific block fast, knowing
	 * only pointer (address of the block, offset).
	*/
	std::map<const void*const, size_type> reserved_blocks_indices_;

	// ===========================================================
	// Deleted
	// ===========================================================

	/* @deleted linear_allocator const copy assignment operator */
	linear_allocator & operator=( const linear_allocator & ) = delete;

	/* @deleted linear_allocator move assignment operator */
	linear_allocator & operator=( linear_allocator && ) = delete;

	// -------------------------------------------------------- \\

};