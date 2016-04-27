
#include "vector_funcs.hpp"
#include "quaternion_funcs.hpp"


GML_NAMESPACE_BEGIN


//-------------------------------------------------------------------------
// static members

template < typename T, STRATEGY O >
GML_FORCEINLINE typename Quaternion< T, O >::value_t Quaternion< T, O >::smallvalue ()
{
	return epsilon< T >();
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O > Quaternion< T, O >::zero ()
{
	return Quaternion( T( 0 ), T( 0 ), T( 0 ), T( 0 ) );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O > Quaternion< T, O >::identity ()
{
	return Quaternion( T( 0 ), T( 0 ), T( 0 ), T( 1 ) );
}


//-------------------------------------------------------------------------
// constructors / destructors

template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O >::Quaternion ()
{
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O >::Quaternion ( const Quaternion &toCopy )
{
	set( toCopy );
}

/*
template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O >::Quaternion ( const vector_t &toCopy )
{
	set( toCopy );
}
*/

template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O >::Quaternion ( value_t const * const toCopy )
{
	set( toCopy );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O >::Quaternion ( value_t x, value_t y, value_t z, value_t w )
{
	set( x, y, z, w );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O > & Quaternion< T, O >::operator = ( const Quaternion &toCopy )
{
//	if( this == &toCopy )  // forget the branch - not worth it
//		return *this;
	set( toCopy );
	return *this;
}


//-------------------------------------------------------------------------
// initalizers

template < typename T, STRATEGY O >
GML_FORCEINLINE void Quaternion< T, O >::set ( const Quaternion &toCopy )
{
	qcopy< O >( toCopy.a, this->a );
}

/*
template < typename T, STRATEGY O >
GML_FORCEINLINE void Quaternion< T, O >::set ( const vector_t &toCopy )
{
	qcopy< O >( toCopy.a, this->a );
}
*/

template < typename T, STRATEGY O >
GML_FORCEINLINE void Quaternion< T, O >::set ( value_t const * const toCopy )
{
	qcopy< O >( toCopy, this->a );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE void Quaternion< T, O >::set ( value_t x, value_t y, value_t z, value_t w )
{
	qset< O >( this->a, x, y, z, w );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE void Quaternion< T, O >::setZero ()
{
	qset< O >( this->a, T( 0 ), T( 0 ), T( 0 ), T( 0 ) );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE void Quaternion< T, O >::setIdentity ()
{
	qset< O >( this->a, T( 0 ), T( 0 ), T( 0 ), T( 1 ) );
}


//-------------------------------------------------------------------------
// functions to convert one Quaternion to another

template < typename T, STRATEGY O >
template < typename S, STRATEGY P >
GML_FORCEINLINE Quaternion< T, O >::Quaternion ( const Quaternion< S, P > &toCopy )
{
	set( toCopy );
}


template < typename T, STRATEGY O >
template < typename S, STRATEGY P >
GML_FORCEINLINE Quaternion< T, O > & Quaternion< T, O >::operator = ( const Quaternion< S, P > &toCopy )
{
	set( toCopy );
	return *this;
}


template < typename T, STRATEGY O >
template < typename S, STRATEGY P >
GML_INLINE void Quaternion< T, O >::set ( const Quaternion< S, P > &toCopy )
{
	for( int i = 0 ; i < 4 ; ++i )
		this->a[ i ] = ( T )toCopy.a[ i ];
}


//-------------------------------------------------------------------------
// conversion operators

template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O >::operator value_t * ()
{
	return ( value_t * )this->a;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O >::operator value_t const * () const
{
	return ( const value_t * )this->a;
}

/*
template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O >::operator vector_t & ()
{
	return *( vector_t * )this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O >::operator vector_t const & () const
{
	return *( const vector_t * )this;
}
*/

//-------------------------------------------------------------------------
// unary quaternion ops

template < typename T, STRATEGY O >
GML_FORCEINLINE bool Quaternion< T, O >::normalize ( const Quaternion &fallback, value_t eps )
{
	return qnorm< O >( this->a, this->a, fallback.a, eps );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O > Quaternion< T, O >::operator - () const
{
	Quaternion out;
	qneg< O >( this->a, out.a );
	return out;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O > Quaternion< T, O >::conjugate () const
{
	Quaternion out;
	qconj< O >( this->a, out.a );
	return out;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE typename Quaternion< T, O >::value_t Quaternion< T, O >::lengthSqr () const
{
	return qlensqr< O >( this->a );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE typename Quaternion< T, O >::value_t Quaternion< T, O >::length () const
{
	return qlen< O >( this->a );
}


//-------------------------------------------------------------------------
// binary quaternion ops

template <  typename T, STRATEGY O >
GML_FORCEINLINE typename Quaternion< T, O >::value_t Quaternion< T, O >::dot ( const Quaternion &rhs ) const
{
	return qdot< O >( this->a, rhs.a );
}


//-------------------------------------------------------------------------
// in-place quaternion algebra

template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O > & Quaternion< T, O >::operator += ( const Quaternion &rhs )
{
	qadd< O >( this->a, rhs.a, this->a );
	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O > & Quaternion< T, O >::operator -= ( const Quaternion &rhs )
{
	qsub< O >( this->a, rhs.a, this->a );
	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O > & Quaternion< T, O >::operator *= ( const Quaternion &rhs )
{
	Quaternion tmp( *this );
	qmul< O >( tmp.a, rhs.a, this->a );
	return *this;
}


//-------------------------------------------------------------------------
// quaternion algebra

template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O > Quaternion< T, O >::operator + ( const Quaternion &rhs ) const
{
	Quaternion out;
	qadd< O >( this->a, rhs.a, out.a );
	return out;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O > Quaternion< T, O >::operator - ( const Quaternion &rhs ) const
{
	Quaternion out;
	qsub< O >( this->a, rhs.a, out.a );
	return out;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O > Quaternion< T, O >::operator * ( const Quaternion &rhs ) const
{
	Quaternion out;
	qmul< O >( this->a, rhs.a, out.a );
	return out;
}


//-------------------------------------------------------------------------
// in-place scalar algebra

template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O > & Quaternion< T, O >::operator += ( T rhs )
{
	qadd< O >( this->a, rhs, this->a );
	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O > & Quaternion< T, O >::operator -= ( T rhs )
{
	qsub< O >( this->a, rhs, this->a );
	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O > & Quaternion< T, O >::operator *= ( T rhs )
{
	qmul< O >( this->a, rhs, this->a );
	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O > & Quaternion< T, O >::operator /= ( T rhs )
{
	qdiv< O >( this->a, rhs, this->a );
	return *this;
}


//-------------------------------------------------------------------------
// scalar algebra

template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O > Quaternion< T, O >::operator + ( T rhs ) const
{
	Quaternion out;
	qadd< O >( this->a, rhs, out.a );
	return out;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O > Quaternion< T, O >::operator - ( T rhs ) const
{
	Quaternion out;
	qsub< O >( this->a, rhs, out.a );
	return out;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O > Quaternion< T, O >::operator * ( T rhs ) const
{
	Quaternion out;
	qmul< O >( this->a, rhs, out.a );
	return out;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O > Quaternion< T, O >::operator / ( T rhs ) const
{
	Quaternion out;
	qdiv< O >( this->a, rhs, out.a );
	return out;
}


//-------------------------------------------------------------------------
// comparisons

template < typename T, STRATEGY O >
GML_FORCEINLINE bool Quaternion< T, O >::operator == ( const Quaternion &rhs ) const
{
	return qeq< O >( this->a, rhs.a ); 
}


template < typename T, STRATEGY O >
GML_FORCEINLINE bool Quaternion< T, O >::operator != ( const Quaternion &rhs ) const
{
	return qneq< O >( this->a, rhs.a ); 
}


template < typename T, STRATEGY O >
GML_FORCEINLINE bool Quaternion< T, O >::operator <  ( const Quaternion &rhs ) const
{
	return qlt< O >( this->a, rhs.a ); 
}


template < typename T, STRATEGY O >
GML_FORCEINLINE bool Quaternion< T, O >::operator >  ( const Quaternion &rhs ) const
{
	return qgt< O >( this->a, rhs.a ); 
}


template < typename T, STRATEGY O >
GML_FORCEINLINE bool Quaternion< T, O >::operator <= ( const Quaternion &rhs ) const
{
	return qle< O >( this->a, rhs.a ); 
}


template < typename T, STRATEGY O >
GML_FORCEINLINE bool Quaternion< T, O >::operator >= ( const Quaternion &rhs ) const
{
	return qge< O >( this->a, rhs.a ); 
}


template < typename T, STRATEGY O >
GML_FORCEINLINE bool Quaternion< T, O >::realeq ( const Quaternion &rhs, value_t eps ) const
{
	return qrealeq< O >( this->a, rhs.a, eps ); 
}


//---------------------------------------------------------------------------
//

template < typename T, STRATEGY O >
GML_FORCEINLINE bool Quaternion< T, O >::isNan () const
{
	return qnan< O >( this->a ); 
}


template < typename T, STRATEGY O >
GML_FORCEINLINE bool Quaternion< T, O >::isFinite () const
{
	return qfinite< O >( this->a ); 
}


//---------------------------------------------------------------------------
// helper functions


template < typename T, STRATEGY O >
GML_FORCEINLINE typename Quaternion< T, O >::value_t dot ( const Quaternion< T, O > &lhs, const Quaternion< T, O > &rhs )
{
	return lhs.dot( rhs );
}


//-------------------------------------------------------------------------

GML_NAMESPACE_END

//-------------------------------------------------------------------------
// external functions

template < typename T, gml::STRATEGY O >
GML_FORCEINLINE gml::Quaternion< T, O > operator + ( const T &lhs, const gml::Quaternion< T, O > &rhs )
{
	return ( rhs + lhs );
}


template < typename T, gml::STRATEGY O >
GML_FORCEINLINE gml::Quaternion< T, O > operator * ( const T &lhs, const gml::Quaternion< T, O > &rhs )
{
	return ( rhs * lhs );
}


#if GML_SUPPORT_STL

template < typename T, gml::STRATEGY O >
GML_INLINE std::ostream & operator << ( std::ostream &out, const gml::Quaternion< T, O > &q )
{
	out << "[ ";
	for( int i = 0 ; i < 3 ; ++i )
		out << " " << q[ i ];
	out << "]";
	return out;
}

#endif // GML_SUPPORT_STL

