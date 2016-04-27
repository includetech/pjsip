
#include "vector_funcs.hpp"


GML_NAMESPACE_BEGIN


//-------------------------------------------------------------------------
// static members

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE typename Vector< N, T, O >::value_t Vector< N, T, O >::smallvalue ()
{
	return epsilon< T >();
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::zero ()
{
	Vector z;
	vzero< O, N >( z.a );
	return z;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::random ( value_t min, value_t max )
{
	Vector v;
	v.randomize( min, max );
	return v;
}


//-------------------------------------------------------------------------
// constructors / destructors

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O >::Vector ()
{
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O >::Vector ( const Vector &toCopy )
{
	set( toCopy );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O >::Vector ( value_t const * const toCopy )
{
	set( toCopy );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O >::Vector ( value_t x )
{
	set( x );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O >::Vector ( value_t x, value_t y, value_t z, value_t w ) 
{ 
	set( x, y, z, w ); 
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > & Vector< N, T, O >::operator = ( const Vector &toCopy )
{
//	if( this == &toCopy )  // forget the branch - not worth it
//		return *this;
	set( toCopy );
	return *this;
}


//-------------------------------------------------------------------------
// initalizers

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Vector< N, T, O >::set ( const Vector &toCopy )
{
	vcopy< O, N >( toCopy.a, this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Vector< N, T, O >::set ( value_t const * const toCopy )
{
	vcopy< O, N >( toCopy, this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Vector< N, T, O >::set ( value_t x )
{
	vset< O, N >( this->a, x );
}


// warning: conditional expression is constant
#pragma warning( disable : 4127 )

#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warray-bounds"
#endif

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Vector< N, T, O >::set ( value_t x, value_t y, value_t z, value_t w )
{
	if( N == 4 )
	{
		vset< O, 4 >( this->a, x, y, z, w );
	}
	else
	{
		if( N >= 1 )
			this->a[ 0 ] = x;
		if( N >= 2 )
			this->a[ 1 ] = y;
		if( N >= 3 )
			this->a[ 2 ] = z;
		if( N >= 4 )
			this->a[ 3 ] = w;
		for( int i = 4 ; i < N ; ++i )
			this->a[ i ] = T( 0 );
	}
}

#if __clang__
#pragma clang diagnostic pop
#endif


#pragma warning( default : 4127 )


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Vector< N, T, O >::setZero ()
{
	vzero< O, N >( this->a );
}


//-------------------------------------------------------------------------
// functions to convert one Vector to another

template < int N, typename T, STRATEGY O >
template < int M, typename S, STRATEGY P >
GML_FORCEINLINE Vector< N, T, O >::Vector ( const Vector< M, S, P > &toCopy )
{
	set( toCopy );
}


template < int N, typename T, STRATEGY O >
template < int M, typename S, STRATEGY P >
GML_FORCEINLINE Vector< N, T, O > & Vector< N, T, O >::operator = ( const Vector< M, S, P > &toCopy )
{
	set( toCopy );
	return *this;
}


template < int N, typename T, STRATEGY O >
template < int M, typename S, STRATEGY P >
GML_INLINE void Vector< N, T, O >::set ( const Vector< M, S, P > &toCopy )
{
	for( int i = 0 ; i < N && i < M ; ++i )
		this->a[ i ] = T( toCopy.a[ i ] );
	for( int i = M ; i < N ; ++i )
		this->a[ i ] = T( 0 );
}


//-------------------------------------------------------------------------
//

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE int Vector< N, T, O >::numRows () const
{
	return N;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE int Vector< N, T, O >::numColumns () const
{
	return 1;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE int Vector< N, T, O >::numElements () const
{
	return N;
}


//-------------------------------------------------------------------------
// conversion operators

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O >::operator value_t * ()
{
	return ( value_t * )this->a;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O >::operator value_t const * () const
{
	return ( const value_t * )this->a;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE typename Vector< N, T, O >::value_t & Vector< N, T, O >::operator () ( int i )
{
	GML_ASSERT( i >= 0 && i < N );
	return ( ( T * )this )[ i ];
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE const typename Vector< N, T, O >::value_t & Vector< N, T, O >::operator () ( int i ) const
{
	GML_ASSERT( i >= 0 && i < N );
	return ( ( T * )this )[ i ];
}


//-------------------------------------------------------------------------
// unary vector ops

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Vector< N, T, O >::randomize ( value_t min, value_t max )
{
	vrand< O, N >( this->a, min, max );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE bool Vector< N, T, O >::normalize ( const Vector &fallback, value_t eps )
{
	return vnorm< O, N >( this->a, this->a, fallback.a, eps );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::operator - () const
{
	Vector out;
	vneg< O, N >( this->a, out.a );
	return out;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::reciprocal () const
{
	Vector out;
	vrecip< O, N >( this->a, out.a );
	return out;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::abs () const
{
	Vector out;
	vabs< O, N >( this->a, out.a );
	return out;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::clamp ( value_t min, value_t max ) const
{
	Vector out;
	vclamp< O, N >( this->a, out.a, min, max );
	return out;
}


//-------------------------------------------------------------------------
// 

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE typename Vector< N, T, O >::value_t Vector< N, T, O >::lengthSqr () const
{
	return vlensqr< O, N >( this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE typename Vector< N, T, O >::value_t Vector< N, T, O >::length () const
{
	return vlen< O, N >( this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE typename Vector< N, T, O >::value_t Vector< N, T, O >::maxOf () const
{
	return vmax< O, N >( this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE typename Vector< N, T, O >::value_t Vector< N, T, O >::minOf () const
{
	return vmin< O, N >( this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > & Vector< N, T, O >::maxOf ( const Vector< N, T, O > &lhs, const Vector< N, T, O > &rhs )
{
	vmax< O, N >( lhs.a, rhs.a, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > & Vector< N, T, O >::minOf ( const Vector< N, T, O > &lhs, const Vector< N, T, O > &rhs )
{
	vmin< O, N >( lhs.a, rhs.a, this->a );
	return *this;
}


//-------------------------------------------------------------------------
// binary vector ops

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE typename Vector< N, T, O >::value_t Vector< N, T, O >::dot ( const Vector &rhs ) const
{
	return vdot< O, N >( this->a, rhs.a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::projection ( const Vector &onto, value_t eps ) const
{
	Vector out;
	vproj< O, N >( this->a, onto.a, out.a, eps );
	return out;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE typename Vector< N, T, O >::value_t Vector< N, T, O >::angle ( const Vector &onto ) const
{
	return vangle< O, N >( this->a, onto.a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE typename Vector< N, T, O >::value_t Vector< N, T, O >::component ( const Vector &onto, value_t eps ) const
{
	return vcomp< O, N >( this->a, onto.a, eps );
}


//-------------------------------------------------------------------------
// in-place vector algebra

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > & Vector< N, T, O >::operator += ( const Vector &rhs )
{
	vadd< O, N >( this->a, rhs.a, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > & Vector< N, T, O >::operator -= ( const Vector &rhs )
{
	vsub< O, N >( this->a, rhs.a, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > & Vector< N, T, O >::operator *= ( const Vector &rhs )
{
	vmul< O, N >( this->a, rhs.a, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > & Vector< N, T, O >::operator /= ( const Vector &rhs )
{
	vdiv< O, N >( this->a, rhs.a, this->a );
	return *this;
}


//-------------------------------------------------------------------------
// vector algebra

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::operator + ( const Vector &rhs ) const
{
	Vector out;
	vadd< O, N >( this->a, rhs.a, out.a );
	return out;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::operator - ( const Vector &rhs ) const
{
	Vector out;
	vsub< O, N >( this->a, rhs.a, out.a );
	return out;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::operator * ( const Vector &rhs ) const
{
	Vector out;
	vmul< O, N >( this->a, rhs.a, out.a );
	return out;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::operator / ( const Vector &rhs ) const
{
	Vector out;
	vdiv< O, N >( this->a, rhs.a, out.a );
	return out;
}


//-------------------------------------------------------------------------
// in-place scalar algebra

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > & Vector< N, T, O >::operator += ( T rhs )
{
	vadd< O, N >( this->a, rhs, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > & Vector< N, T, O >::operator -= ( T rhs )
{
	vsub< O, N >( this->a, rhs, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > & Vector< N, T, O >::operator *= ( T rhs )
{
	vmul< O, N >( this->a, rhs, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > & Vector< N, T, O >::operator /= ( T rhs )
{
	vdiv< O, N >( this->a, rhs, this->a );
	return *this;
}


//-------------------------------------------------------------------------
// scalar algebra

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::operator + ( T rhs ) const
{
	Vector out;
	vadd< O, N >( this->a, rhs, out.a );
	return out;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::operator - ( T rhs ) const
{
	Vector out;
	vsub< O, N >( this->a, rhs, out.a );
	return out;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::operator * ( T rhs ) const
{
	Vector out;
	vmul< O, N >( this->a, rhs, out.a );
	return out;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::operator / ( T rhs ) const
{
	Vector out;
	vdiv< O, N >( this->a, rhs, out.a );
	return out;
}


//-------------------------------------------------------------------------
// comparisons

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE bool Vector< N, T, O >::operator == ( const Vector &rhs ) const
{
	return veq< O, N >( this->a, rhs.a ); 
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE bool Vector< N, T, O >::operator != ( const Vector &rhs ) const
{
	return vneq< O, N >( this->a, rhs.a ); 
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE bool Vector< N, T, O >::operator < ( const Vector &rhs ) const
{
	return vlt< O, N >( this->a, rhs.a ); 
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE bool Vector< N, T, O >::operator > ( const Vector &rhs ) const
{
	return vgt< O, N >( this->a, rhs.a ); 
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE bool Vector< N, T, O >::operator <= ( const Vector &rhs ) const
{
	return vle< O, N >( this->a, rhs.a ); 
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE bool Vector< N, T, O >::operator >= ( const Vector &rhs ) const
{
	return vge< O, N >( this->a, rhs.a ); 
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::eq ( const Vector &rhs ) const
{
	Vector v;
	veq< O, N >( this->a, rhs.a, v.a );
	return v;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::neq ( const Vector &rhs ) const
{
	Vector v;
	vneq< O, N >( this->a, rhs.a, v.a );
	return v;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::lt ( const Vector &rhs ) const
{
	Vector v;
	vlt< O, N >( this->a, rhs.a, v.a );
	return v;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::gt ( const Vector &rhs ) const
{
	Vector v;
	vgt< O, N >( this->a, rhs.a, v.a );
	return v;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::le ( const Vector &rhs ) const
{
	Vector v;
	vle< O, N >( this->a, rhs.a, v.a );
	return v;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::ge ( const Vector &rhs ) const
{
	Vector v;
	vge< O, N >( this->a, rhs.a, v.a );
	return v;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE bool Vector< N, T, O >::realeq ( const Vector &rhs, value_t eps ) const
{
	return vrealeq< O, N >( this->a, rhs.a, eps ); 
}


//---------------------------------------------------------------------------
//

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE bool Vector< N, T, O >::isNan () const
{
	return vnan< O, N >( this->a ); 
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE bool Vector< N, T, O >::isFinite () const
{
	return vfinite< O, N >( this->a ); 
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE int Vector< N, T, O >::mask () const
{
	return vmask< O, N >( this->a ); 
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > & Vector< N, T, O >::select ( const Vector &mask, const Vector &lhs, const Vector &rhs )
{
	vselect< O, N >( mask.a, lhs.a, rhs.a, this->a );
	return *this;
}


//---------------------------------------------------------------------------
// bitwise operations

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::operator & ( const Vector &rhs ) const
{
	Vector< N, T, O > out;
	vand< O, N >( this->a, rhs.a, out.a );
	return out;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::operator | ( const Vector &rhs ) const
{
	Vector< N, T, O > out;
	vor< O, N >( this->a, rhs.a, out.a );
	return out;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::operator ^ ( const Vector &rhs ) const
{
	Vector< N, T, O > out;
	vxor< O, N >( this->a, rhs.a, out.a );
	return out;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::operator ~ () const
{
	Vector< N, T, O > out;
	vnot< O, N >( this->a, out.a );
	return out;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::andnot ( const Vector &rhs ) const
{
	Vector< N, T, O > out;
	vandnot< O, N >( this->a, rhs.a, out.a );
	return out;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > & Vector< N, T, O >::operator &= ( const Vector &rhs )
{
	vand< O, N >( this->a, rhs.a, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > & Vector< N, T, O >::operator |= ( const Vector &rhs )
{
	vor< O, N >( this->a, rhs.a, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > & Vector< N, T, O >::operator ^= ( const Vector &rhs )
{
	vxor< O, N >( this->a, rhs.a, this->a );
	return *this;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > & Vector< N, T, O >::setandnot ( const Vector &rhs )
{
	vandnot< O, N >( this->a, rhs.a, this->a );
	return *this;
}


//---------------------------------------------------------------------------
// only available for certain vector sizes

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > Vector< N, T, O >::cross ( const Vector &rhs ) const
{
	GML_COMPILE_ASSERT( N == 4 || N == 3 );

	Vector out;
	vcross< O, N >( this->a, rhs.a, out.a );
	return out;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE bool Vector< N, T, O >::homogenize ( const Vector &fallback, value_t eps )
{
	GML_COMPILE_ASSERT( N == 4 );

	return vhomog< O, N >( this->a, this->a, fallback.a, eps );
}


//---------------------------------------------------------------------------
// helper functions


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > max ( const Vector< N, T, O > &lhs, const Vector< N, T, O > &rhs )
{
	Vector< N, T, O > v;
	v.max( lhs, rhs );
	return v;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > min ( const Vector< N, T, O > &lhs, const Vector< N, T, O > &rhs )
{
	Vector< N, T, O > v;
	v.min( lhs, rhs );
	return v;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > select ( const Vector< N, T, O > &mask, const Vector< N, T, O > &lhs, const Vector< N, T, O > &rhs )
{
	Vector< N, T, O > v;
	v.select( mask, lhs, rhs );
	return v;
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE typename Vector< N, T, O >::value_t dot ( const Vector< N, T, O > &lhs, const Vector< N, T, O > &rhs )
{
	return lhs.dot( rhs );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE Vector< N, T, O > cross ( const Vector< N, T, O > &lhs, const Vector< N, T, O > &rhs )
{
	GML_COMPILE_ASSERT( N == 4 || N == 3 );
	return lhs.cross( rhs );
}


//-------------------------------------------------------------------------
// opengl support

#if GML_SUPPORT_OPENGL

template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Vector< N, T, O >::glGet ( GLenum which )
{
	glGetvTmpl( which, this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Vector< N, T, O >::glVertex () const
{
	glVertexTmpl< N >( this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Vector< N, T, O >::glNormal () const
{
	glNormalTmpl< N >( this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Vector< N, T, O >::glColor () const
{
	glColorTmpl< N >( this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Vector< N, T, O >::glTexCoord () const
{
	glTexCoordTmpl< N >( this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Vector< N, T, O >::glTranslate () const
{
	GML_COMPILE_ASSERT( N == 4 || N == 3 );
	glTranslateTmpl( this->a );
}


template < int N, typename T, STRATEGY O >
GML_FORCEINLINE void Vector< N, T, O >::glScale () const
{
	GML_COMPILE_ASSERT( N == 4 || N == 3 );
	glScaleTmpl( this->a );
}

#endif // GML_SUPPORT_OPENGL


//-------------------------------------------------------------------------

GML_NAMESPACE_END

//-------------------------------------------------------------------------
// external functions

template < int N, typename T, gml::STRATEGY O >
GML_FORCEINLINE gml::Vector< N, T, O > operator + ( const T &lhs, const gml::Vector< N, T, O > &rhs )
{
	return ( rhs + lhs );
}


template < int N, typename T, gml::STRATEGY O >
GML_FORCEINLINE gml::Vector< N, T, O > operator - ( const T &lhs, const gml::Vector< N, T, O > &rhs )
{
	return ( - rhs + lhs );
}


template < int N, typename T, gml::STRATEGY O >
GML_FORCEINLINE gml::Vector< N, T, O > operator * ( const T &lhs, const gml::Vector< N, T, O > &rhs )
{
	return ( rhs * lhs );
}


template < int N, typename T, gml::STRATEGY O >
GML_FORCEINLINE gml::Vector< N, T, O > operator / ( const T &lhs, const gml::Vector< N, T, O > &rhs )
{
	return ( rhs.reciprocal() * lhs );
}


#if GML_SUPPORT_STL

template < int N, typename T, gml::STRATEGY O >
GML_INLINE std::ostream & operator << ( std::ostream &out, const gml::Vector< N, T, O > &v )
{
	out << "[ ";
	for( int i = 0 ; i < N ; ++i )
		out << " " << v[ i ];
	out << "]'";
	return out;
}

#endif // GML_SUPPORT_STL
