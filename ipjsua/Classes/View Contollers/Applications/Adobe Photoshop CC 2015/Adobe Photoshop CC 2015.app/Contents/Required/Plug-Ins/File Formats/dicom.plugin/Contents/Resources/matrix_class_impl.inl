
#include "matrix_funcs.hpp"


GML_NAMESPACE_BEGIN

// column major storage
#define IDX(N,R,C) ( ( C ) * ( N ) + ( R ) )


//-------------------------------------------------------------------------
// static members

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE typename Matrix< N, T, O >::value_t Matrix< N, T, O >::smallvalue ()
{
	return epsilon< T >();
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > Matrix< N, T, O >::zero ()
{
	return Matrix( T( 0 ) );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > Matrix< N, T, O >::identity ()
{
	Matrix m;
	m.setDiagonal( 1 );
	return m;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > Matrix< N, T, O >::random ( value_t min, value_t max )
{
	Matrix m;
	m.randomize( min, max );
	return m;
}


//-------------------------------------------------------------------------
// constructors / destructors


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O >::Matrix ()
{
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O >::Matrix ( const Matrix &toCopy )
{
	set( toCopy );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O >::Matrix ( value_t const * const toCopy )
{
	set( toCopy );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O >::Matrix ( value_t const * const toCopy, bool rowMajor )
{
	set( toCopy, rowMajor );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O >::Matrix ( const value_t toCopy[ N ][ N ] )
{
	set( toCopy );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O >::Matrix ( const value_t toCopy[ N ][ N ], bool rowMajor )
{
	set( toCopy, rowMajor );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O >::Matrix ( value_t x )
{
	set( x );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > & Matrix< N, T, O >::operator = ( const Matrix &toCopy )
{
//	if( this == &toCopy ) // forget the branch - not worth it.
//		return *this;

	set( toCopy );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O >::Matrix ( 
	value_t _m00, value_t _m01,
	value_t _m10, value_t _m11 )
{
	set( _m00, _m01,
		 _m10, _m11 );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O >::Matrix ( 
	value_t _m00, value_t _m01, value_t _m02,
	value_t _m10, value_t _m11, value_t _m12,
	value_t _m20, value_t _m21, value_t _m22 )
{
	set( _m00, _m01, _m02,
		 _m10, _m11, _m12,
		 _m20, _m21, _m22 );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O >::Matrix ( 
	value_t _m00, value_t _m01, value_t _m02, value_t _m03,
	value_t _m10, value_t _m11, value_t _m12, value_t _m13,
	value_t _m20, value_t _m21, value_t _m22, value_t _m23,
	value_t _m30, value_t _m31, value_t _m32, value_t _m33 )
{
	set( _m00, _m01, _m02, _m03,
		 _m10, _m11, _m12, _m13,
		 _m20, _m21, _m22, _m23,
		 _m30, _m31, _m32, _m33 );
}


//-------------------------------------------------------------------------
// initializers

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Matrix< N, T, O >::set ( const Matrix &toCopy )
{
	mcopy< O, N >( toCopy.a, this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Matrix< N, T, O >::set ( value_t const * const toCopy )
{
	GML_ASSERT( this->a );
	mcopy< O, N >( toCopy, this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Matrix< N, T, O >::set ( value_t const * const toCopy, bool rowMajor )
{
	GML_ASSERT( this->a );
	if( rowMajor )
		mtrans< O, N >( toCopy, this->a );
	else
		mcopy< O, N >( toCopy, this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Matrix< N, T, O >::set ( const value_t toCopy[ N ][ N ] )
{
	mtrans< O, N >( ( T const * const )toCopy, this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Matrix< N, T, O >::set ( const value_t toCopy[ N ][ N ], bool rowMajor )
{
	if( ! rowMajor )
		mcopy< O, N >( ( T const * const )toCopy, this->a );
	else
		mtrans< O, N >( ( T const * const )toCopy, this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Matrix< N, T, O >::set ( value_t x )
{
	mset< O, N >( this->a, x );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Matrix< N, T, O >::setZero ()
{
	mset< O, N >( this->a, T( 0 ) );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Matrix< N, T, O >::setIdentity ()
{
	miden< O, N >( this->a );
}


// warning: conditional expression is constant
#pragma warning( disable : 4127 )

template < int N, typename T, STRATEGY O >
GML_INLINE void Matrix< N, T, O >::set ( 
	value_t _m00, value_t _m01,
	value_t _m10, value_t _m11 )
{
	Matrix< N, T, O > &M = *this;

	M( 0, 0 ) = _m00;
	if( N >= 2 )
	{
		M( 1, 0 ) = _m10;
		M( 0, 1 ) = _m01;
		M( 1, 1 ) = _m11;
	}
	if( N > 2 )
	{
		for( int c = 0 ; c < 2 ; ++c )
			for( int r = 2 ; r < N ; ++r )
				M( r, c ) = 0;
		for( int c = 2 ; c < N ; ++c )
			for( int r = 0 ; r < N ; ++r )
				M( r, c ) = 0;
	}
}


template < int N, typename T, STRATEGY O >
GML_INLINE void Matrix< N, T, O >::set ( 
	value_t _m00, value_t _m01, value_t _m02,
	value_t _m10, value_t _m11, value_t _m12,
	value_t _m20, value_t _m21, value_t _m22 )
{
	Matrix< N, T, O > &M = *this;

	M( 0, 0 ) = _m00;
	if( N >= 2 )
	{
		M( 1, 0 ) = _m10;
		M( 0, 1 ) = _m01;
		M( 1, 1 ) = _m11;
	}
	if( N >= 3 )
	{
		M( 0, 2 ) = _m02;
		M( 1, 2 ) = _m12;
		M( 2, 0 ) = _m20;
		M( 2, 1 ) = _m21;
		M( 2, 2 ) = _m22;
	}
	if( N > 3 )
	{
		for( int c = 0 ; c < 3 ; ++c )
			for( int r = 3 ; r < N ; ++r )
				M( r, c ) = 0;
		for( int c = 3 ; c < N ; ++c )
			for( int r = 0 ; r < N ; ++r )
				M( r, c ) = 0;
	}
}


template < int N, typename T, STRATEGY O >
GML_INLINE void Matrix< N, T, O >::set ( 
	value_t _m00, value_t _m01, value_t _m02, value_t _m03,
	value_t _m10, value_t _m11, value_t _m12, value_t _m13,
	value_t _m20, value_t _m21, value_t _m22, value_t _m23,
	value_t _m30, value_t _m31, value_t _m32, value_t _m33 )
{
	Matrix< N, T, O > &M = *this;

	M( 0, 0 ) = _m00;
	if( N >= 2 )
	{
		M( 1, 0 ) = _m10;
		M( 0, 1 ) = _m01;
		M( 1, 1 ) = _m11;
	}
	if( N >= 3 )
	{
		M( 0, 2 ) = _m02;
		M( 1, 2 ) = _m12;
		M( 2, 0 ) = _m20;
		M( 2, 1 ) = _m21;
		M( 2, 2 ) = _m22;
	}
	if( N >= 4 )
	{
		M( 0, 3 ) = _m03;
		M( 1, 3 ) = _m13;
		M( 2, 3 ) = _m23;
		M( 3, 0 ) = _m30;
		M( 3, 1 ) = _m31;
		M( 3, 2 ) = _m32;
		M( 3, 3 ) = _m33;
	}
	if( N > 4 )
	{
		for( int c = 0 ; c < 4 ; ++c )
			for( int r = 4 ; r < N ; ++r )
				M( r, c ) = 0;
		for( int c = 4 ; c < N ; ++c )
			for( int r = 0 ; r < N ; ++r )
				M( r, c ) = 0;
	}
}

#pragma warning( default : 4127 )


template < int N, typename T, STRATEGY O >
GML_INLINE void Matrix< N, T, O >::setRow ( int i, const vector_t &r )
{
	GML_ASSERT( i >= 0 && i < N );

	for( int c = 0 ; c < N ; ++c )
		( *this )( i, c ) = r[ c ];
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Matrix< N, T, O >::setColumn ( int i, const vector_t &_c )
{
	GML_ASSERT( i >= 0 && i < N );

	vcopy< O, N >( ( T const * const )_c, ( T * const )( this->a + i*N ) );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Matrix< N, T, O >::setDiagonal ( value_t x )
{
	mdiag< O, N >( this->a, x );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Matrix< N, T, O >::setDiagonal ( const vector_t &d )
{
	mdiag< O, N >( this->a, ( T const * const )d );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Matrix< N, T, O >::randomize ( value_t min, value_t max )
{
	mrand< O, N >( this->a, min, max );
}


//-------------------------------------------------------------------------
// matrix conversions

template < int N, typename T, STRATEGY O >
template < int M, typename S, STRATEGY P >
GML_FORCEINLINE Matrix< N, T, O >::Matrix ( const Matrix< M, S, P > &toCopy )
{
	set( toCopy );
}


template < int N, typename T, STRATEGY O >
template < int M, typename S, STRATEGY P >
GML_FORCEINLINE Matrix< N, T, O > & Matrix< N, T, O >::operator = ( const Matrix< M, S, P > &toCopy )
{
	set( toCopy );
	return *this;
}


template < int N, typename T, STRATEGY O >
template < int M, typename S, STRATEGY P >
GML_INLINE void Matrix< N, T, O >::set ( const Matrix< M, S, P > &toCopy )
{
	for( int c = 0 ; c < N && c < M ; ++c )
		for( int r = 0 ; r < N && r < M ; ++r )
			( *this )( r, c ) = T( toCopy( r, c ) );

	// fill the rest w/ zeros
	for( int c = 0 ; c < M ; ++c )
		for( int r = M ; r < N ; ++r )
			( *this )( r, c ) = 0;
	for( int c = M ; c < N ; ++c )
		for( int r = 0 ; r < N ; ++r )
			( *this )( r, c ) = 0;
}


//-------------------------------------------------------------------------
// accessors

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE T & Matrix< N, T, O >::operator () ( int row, int col )
{
	return this->a[ IDX( N, row, col ) ];
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE const T & Matrix< N, T, O >::operator () ( int row, int col ) const
{
	return this->a[ IDX( N, row, col ) ];
}


template < int N, typename T, STRATEGY O >
GML_INLINE Vector< N, T, O > Matrix< N, T, O >::row ( int r ) const
{
	vector_t v;
	for( int i = 0 ; i < N ; ++i )
		v[ i ] = this->a[ IDX( N, r, i ) ];
	return v;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Matrix< N, T, O >::column ( int c ) const
{
	vector_t v;
	for( int i = 0 ; i < N ; ++i )
		v[ i ] = this->a[ IDX( N, i, c ) ];
	return v;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE int Matrix< N, T, O >::numRows () const
{
	return N;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE int Matrix< N, T, O >::numColumns () const
{
	return N;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE int Matrix< N, T, O >::numElements () const
{
	return N * N;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE T Matrix< N, T, O >::maxOf () const
{
	return mmax< O, N >( this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE T Matrix< N, T, O >::minOf () const
{
	return mmin< O, N >( this->a );
}


//-------------------------------------------------------------------------
// conversion operators

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O >::operator value_t * ()
{
	return ( value_t * )this->a;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O >::operator value_t const * () const
{
	return ( const value_t * )this->a;
}


//-------------------------------------------------------------------------
// matrix ops

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE T Matrix< N, T, O >::cofactor ( const Matrix &m )
{
	return mcofactor< O, N >( m.a, this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE T Matrix< N, T, O >::adjoint ( const Matrix &m )
{
	return madjoint< O, N >( m.a, this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Matrix< N, T, O >::transpose ( const Matrix &m )
{
	mtrans< O, N >( m.a, this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE bool Matrix< N, T, O >::invert ( const Matrix &m, value_t eps )
{
	return minv< O, N >( m.a, this->a, eps );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE T Matrix< N, T, O >::determinant () const
{
	return mdet< O, N >( this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE T Matrix< N, T, O >::trace () const
{
	return mtrace< O, N >( this->a );
}


template < int N, typename T, STRATEGY O >
template < int M >
GML_FORCEINLINE void Matrix< N, T, O >::minorMat ( Matrix< M, T, O > &m, int r, int c ) const
{
	mminor< O, N >( this->a, m.a, r, c );
}


//-------------------------------------------------------------------------
// unary ops

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > & Matrix< N, T, O >::negate ()
{
	mneg< O, N >( this->a, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > & Matrix< N, T, O >::reciprocal ()
{
	mrecip< O, N >( this->a, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > & Matrix< N, T, O >::abs ()
{
	mabs< O, N >( this->a, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > & Matrix< N, T, O >::clamp ( value_t min, value_t max )
{
	mclamp< O, N >( this->a, this->a, min, max );
	return *this;
}


//-------------------------------------------------------------------------
// matrix algebra

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > & Matrix< N, T, O >::operator += ( value_t rhs )
{
	madd< O, N >( this->a, rhs, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > & Matrix< N, T, O >::operator -= ( value_t rhs )
{
	msub< O, N >( this->a, rhs, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > & Matrix< N, T, O >::operator *= ( value_t rhs )
{
	mmul< O, N >( this->a, rhs, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > & Matrix< N, T, O >::operator /= ( value_t rhs )
{
	mdiv< O, N >( this->a, rhs, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > & Matrix< N, T, O >::operator += ( const Matrix &rhs )
{
	madd< O, N >( this->a, rhs.a, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > & Matrix< N, T, O >::operator -= ( const Matrix &rhs )
{
	msub< O, N >( this->a, rhs.a, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > & Matrix< N, T, O >::add ( const Matrix &lhs, const Matrix &rhs )
{
	madd< O, N >( lhs.a, rhs.a, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > & Matrix< N, T, O >::sub ( const Matrix &lhs, const Matrix &rhs )
{
	msub< O, N >( lhs.a, rhs.a, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > & Matrix< N, T, O >::mul ( const Matrix &lhs, const Matrix &rhs )
{
	mmmul< O, N >( lhs.a, rhs.a, this->a );
	return *this;
}


//-------------------------------------------------------------------------
// matrix vector multiply

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Matrix< N, T, O >::operator * ( const vector_t &rhs ) const
{
	vector_t v;
	mvmul< O, N >( this->a, rhs.a, v.a, 1 );
	return v;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Matrix< N, T, O >::mul ( vector_t const * const rhs, vector_t * const dst ) const
{
	mvmul< O, N >( this->a, ( T const * const )rhs, ( T * const )dst );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Matrix< N, T, O >::mul ( vector_t const * const rhs, vector_t * const dst, int n ) const
{
	mvmul< O, N >( this->a, ( T const * const )rhs, ( T * const )dst, n );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Matrix< N, T, O >::muls ( vector_t const * const rhs, vector_t * const dst, int n ) const
{
	mvmuls< O, N >( this->a, ( T const * const )rhs, ( T * const )dst, n );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE T Matrix< N, T, O >::qform ( const vector_t &lhs, const vector_t &rhs ) const
{
	return mqform< O, N >( lhs.a, this->a, rhs.a );
}


//-------------------------------------------------------------------------
// comparisons

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE bool Matrix< N, T, O >::operator == ( const Matrix &rhs ) const
{
	return meq< O, N >( this->a, rhs.a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE bool Matrix< N, T, O >::operator != ( const Matrix &rhs ) const
{
	return mneq< O, N >( this->a, rhs.a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE bool Matrix< N, T, O >::operator < ( const Matrix &rhs ) const
{
	return mlt< O, N >( this->a, rhs.a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE bool Matrix< N, T, O >::operator > ( const Matrix &rhs ) const
{
	return mgt< O, N >( this->a, rhs.a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE bool Matrix< N, T, O >::operator <= ( const Matrix &rhs ) const
{
	return mle< O, N >( this->a, rhs.a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE bool Matrix< N, T, O >::operator >= ( const Matrix &rhs ) const
{
	return mge< O, N >( this->a, rhs.a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > & Matrix< N, T, O >::eq ( const Matrix &lhs, const Matrix &rhs )
{
	meq< O, N >( lhs.a, rhs.a, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > & Matrix< N, T, O >::neq ( const Matrix &lhs, const Matrix &rhs )
{
	mneq< O, N >( lhs.a, rhs.a, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > & Matrix< N, T, O >::lt ( const Matrix &lhs, const Matrix &rhs )
{
	mlt< O, N >( lhs.a, rhs.a, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > & Matrix< N, T, O >::gt ( const Matrix &lhs, const Matrix &rhs )
{
	mgt< O, N >( lhs.a, rhs.a, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > & Matrix< N, T, O >::le ( const Matrix &lhs, const Matrix &rhs )
{
	mle< O, N >( lhs.a, rhs.a, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Matrix< N, T, O > & Matrix< N, T, O >::ge ( const Matrix &lhs, const Matrix &rhs )
{
	mge< O, N >( lhs.a, rhs.a, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE bool Matrix< N, T, O >::realeq ( const Matrix &rhs, value_t eps ) const
{
	return mrealeq< O, N >( this->a, rhs.a, eps );
}


//---------------------------------------------------------------------------
//

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE bool Matrix< N, T, O >::isNan () const
{
	return mnan< O, N >( this->a ); 
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE bool Matrix< N, T, O >::isFinite () const
{
	return mfinite< O, N >( this->a ); 
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE long long Matrix< N, T, O >::mask () const
{
	return mmask< O, N >( this->a ); 
}


//-------------------------------------------------------------------------
// opengl support

#if GML_SUPPORT_OPENGL

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Matrix< N, T, O >::glGet ( GLenum which )
{
	glGetvTmpl( which, this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Matrix< N, T, O >::glLoadMatrix () const
{
	glLoadMatrixTmpl( this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Matrix< N, T, O >::glMultMatrix () const
{
	glMultMatrixTmpl( this->a );
}

#endif // GML_SUPPORT_OPENGL


//-------------------------------------------------------------------------

#undef IDX

GML_NAMESPACE_END

//-------------------------------------------------------------------------
// external functions

template < int N, typename T, gml::STRATEGY O >
GML_FORCEINLINE gml::Matrix< N, T, O > operator + ( const T &lhs, const gml::Matrix< N, T, O > &rhs )
{
	return ( rhs + lhs );
}


template < int N, typename T, gml::STRATEGY O >
GML_FORCEINLINE gml::Matrix< N, T, O > operator - ( const T &lhs, const gml::Matrix< N, T, O > &rhs )
{
	return ( - rhs + lhs );
}


template < int N, typename T, gml::STRATEGY O >
GML_FORCEINLINE gml::Matrix< N, T, O > operator * ( const T &lhs, const gml::Matrix< N, T, O > &rhs )
{
	return ( rhs * lhs );
}


template < int N, typename T, gml::STRATEGY O >
GML_FORCEINLINE gml::Matrix< N, T, O > operator / ( const T &lhs, const gml::Matrix< N, T, O > &rhs )
{
	return ( rhs.reciprocal() * lhs );
}


#if GML_SUPPORT_STL

template < int N, typename T, gml::STRATEGY O >
GML_INLINE std::ostream & operator << ( std::ostream &out, const gml::Matrix< N, T, O > &m )
{
	out << "[ ";
	for( int r = 0 ; r < N ; ++r )
	{
		for( int c = 0 ; c < N ; ++c )
			out << " " << m( r, c );
		out << "; ";
	}
	out << "]";
	return out;
}

#endif // GML_SUPPORT_STL

