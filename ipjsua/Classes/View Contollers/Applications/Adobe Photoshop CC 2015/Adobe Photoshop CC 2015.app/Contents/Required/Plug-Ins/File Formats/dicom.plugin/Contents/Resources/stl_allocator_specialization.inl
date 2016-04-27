#if 0
namespace std
{

/*
 * specialization of g++ 4.0's std::allocator to use aligned malloc and free.
 */

 template<>
    class allocator< _Ty >
    {
   public:
      typedef size_t     size_type;
      typedef ptrdiff_t  difference_type;
      typedef _Ty*       pointer;
      typedef const _Ty* const_pointer;
      typedef _Ty&       reference;
      typedef const _Ty& const_reference;
      typedef _Ty        value_type;

      template<typename _Tp1>
        struct rebind
        { typedef allocator<_Tp1> other; };

      allocator() throw() { }

      allocator(const allocator&) throw() { }

      template<typename _Tp1>
        allocator(const allocator<_Tp1>&) throw() { }

      ~allocator() throw() { }

      pointer
      address(reference __x) const { return &__x; }

      const_pointer
      address(const_reference __x) const { return &__x; }

      // NB: __n is permitted to be 0.  The C++ standard says nothing
      // about what the return value is when __n == 0.
      pointer
      allocate(size_type __n, const void* = 0)
      { 
		if( __n <= 0 )
			__n = 0;
		else if( __n > this->max_size() )
			throw std::bad_alloc();

        return static_cast<pointer>( ::_aligned_malloc( __n * sizeof( value_type ), 16 ) );
      }

      // __p is not permitted to be a null pointer.
      void
      deallocate(pointer __p, size_type)
      { ::_aligned_free( __p ); }

      size_type
      max_size() const throw() 
      { return size_t(-1) / sizeof(value_type); }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 402. wrong new expression in [some_] allocator::construct
      void 
      construct(pointer __p, const_reference __val) 
      { ::new(__p) value_type(__val); }

// for some reason, the following function generates an warning that __p is an
// unreferenced formal parameter in some situations.  however, leaving it
// unnamed generates an error, and it clearly is referenced...so disable 
// the warning.
#pragma warning( disable : 4100 )

      void 
      destroy(pointer __p) { __p->~value_type(); }

#pragma warning( default : 4100 )

    };

}; // namespace std

#endif // #if 0