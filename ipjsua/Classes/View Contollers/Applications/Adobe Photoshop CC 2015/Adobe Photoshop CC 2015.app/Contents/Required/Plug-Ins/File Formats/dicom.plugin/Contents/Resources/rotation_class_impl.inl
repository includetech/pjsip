

GML_NAMESPACE_BEGIN

//-------------------------------------------------------------------------
// static functions

template < typename T, STRATEGY O >
GML_FORCEINLINE typename Rotation< T, O >::value_t Rotation< T, O >::smallvalue ()
{
	return epsilon< T >();
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > Rotation< T, O >::identity ()
{
	Rotation r;
	r.setIdentity();
	return r;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > Rotation< T, O >::random ()
{
	Rotation r;
	r.randomize();
	return r;
}


//-------------------------------------------------------------------------
// Constructors

template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O >::Rotation ()
{
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O >::Rotation ( const Rotation &toCopy )
{
	set( toCopy );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O >::Rotation ( const quaternion_t &_q )
{
	set( _q );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O >::Rotation ( const matrix_t &m )
{
	set( m );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O >::Rotation ( const vector3_t &axis, value_t angle )
{
	set( axis, angle );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O >::Rotation ( const vector4_t &axis, value_t angle )
{
	set( axis, angle );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O >::Rotation ( value_t yaw, value_t pitch, value_t roll )
{
	set( yaw, pitch, roll );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > & Rotation< T, O >::operator = ( const Rotation &toCopy )
{
	set( toCopy );
	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > & Rotation< T, O >::operator = ( const quaternion_t &_q )
{
	set( _q );
	return *this;
}


//-------------------------------------------------------------------------
// initializers

template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > & Rotation< T, O >::set ( const Rotation &toCopy )
{
	this->q = toCopy.q;
	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > & Rotation< T, O >::set ( const quaternion_t &_q )
{
	this->q = _q;
	normalize();
	return *this;
}


/*
 * from the matrix and quaternion faq
 * http://skal.planet-d.net/demo/matrixfaq.htm
 */
template < typename T, STRATEGY O >
GML_INLINE Rotation< T, O > & Rotation< T, O >::set ( const matrix_t &m )
{
	value_t tr = ( T )( 1.0 + m( 0, 0 ) + m( 1, 1 ) + m( 2, 2 ) );
	value_t s, x, y, z, w;

	if( tr > 0.00001 )
	{
		s = sqrt< T >( tr ) * 2;
		x = ( m( 1, 2 ) - m( 2, 1 ) ) / s;
		y = ( m( 2, 0 ) - m( 0, 2 ) ) / s;
		z = ( m( 0, 1 ) - m( 1, 0 ) ) / s;
		w = ( T )( 0.25 * s );
	}
	else if( m( 0, 0 ) > m( 1, 1 ) && m( 0, 0 ) > m( 2, 2 ) )
	{
		s = sqrt< T >( 1 + m( 0, 0 ) - m( 1, 1 ) - m( 2, 2 ) ) * 2;
		x = ( T )( 0.25 * s );
		y = ( m( 0, 1 ) + m( 1, 0 ) ) / s;
		z = ( m( 2, 0 ) + m( 0, 2 ) ) / s;
		w = ( m( 1, 2 ) - m( 2, 1 ) ) / s;
	}
	else if( m( 1, 1 ) > m( 2, 2 ) )
	{
		s = sqrt< T >( 1 + m( 1, 1 ) - m( 0, 0 ) - m( 2, 2 ) ) * 2;
		x = ( m( 0, 1 ) + m( 1, 0 ) ) / s;
		y = ( T )( 0.25 * s );
		z = ( m( 1, 2 ) + m( 2, 1 ) ) / s;
		w = ( m( 2, 0 ) - m( 0, 2 ) ) / s;
	}
	else
	{
		s = sqrt< T >( 1 + m( 2, 2 ) - m( 0, 0 ) - m( 1, 1 ) ) * 2;
		x = ( m( 2, 0 ) + m( 0, 2 ) ) / s;
		y = ( m( 1, 2 ) + m( 2, 1 ) ) / s;
		z = ( T )( 0.25 * s );
		w = ( m( 0, 1 ) - m( 1, 0 ) ) / s;
	}

	if( w < 0 )
		this->q = Quaternion< T, O >( x, y, z, -w );
	else
		this->q = Quaternion< T, O >( -x, -y, -z, w );
	normalize();

	return *this;
}


template < typename T, STRATEGY O >
GML_INLINE Rotation< T, O > & Rotation< T, O >::set ( const vector3_t &axis, value_t angle )
{
	T l = axis.length();
	T c = cos( angle / 2 );
	T s = sin( angle / 2 );
	T scale = s / l;
	this->q.set( scale * axis.x, scale * axis.y, scale * axis.z, c );
	// normalize shouldn't be necessary here...

	return *this;
}


template < typename T, STRATEGY O >
GML_INLINE Rotation< T, O > & Rotation< T, O >::set ( const vector4_t &axis, value_t angle )
{
	T l = sqrt< T >( axis.x*axis.x + axis.y*axis.y + axis.z*axis.z );
	T c = cos( angle / 2 );
	T s = sin( angle / 2 );
	T scale = s / l;
	this->q.set( scale * axis.x, scale * axis.y, scale * axis.z, c );
	// normalize shouldn't be necessary here...

	return *this;
}


template < typename T, STRATEGY O >
GML_INLINE Rotation< T, O > & Rotation< T, O >::set ( value_t yaw, value_t pitch, value_t roll )
{
	Quaternion< T, O > yawq( 0, sin( yaw / 2 ), 0, cos( yaw / 2 ) ),
		pitchq( sin( pitch / 2 ), 0, 0, cos( pitch / 2 ) ),
		rollq( 0, 0, sin( roll / 2 ), cos( roll / 2 ) );
	this->q = rollq * pitchq * yawq;
	normalize();

	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > & Rotation< T, O >::setIdentity ()
{
	this->q = Quaternion< T, O >::identity();
	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > & Rotation< T, O >::randomize ()
{
	vector3_t axis = vector3_t::random();
	value_t angle = ( T )( rand() / ( value_t )RAND_MAX * M_PI * 2.0 );
	set( axis, angle );
	return *this;
}


//-------------------------------------------------------------------------
// functions to convert one Rotation to another

template < typename T, STRATEGY O >
template < typename S, STRATEGY P >
GML_FORCEINLINE Rotation< T, O >::Rotation ( const Rotation< S, P > &toCopy )
{
	set( toCopy );
}


template < typename T, STRATEGY O >
template < typename S, STRATEGY P >
GML_FORCEINLINE Rotation< T, O > & Rotation< T, O >::operator = ( const Rotation< S, P > &toCopy )
{
	set( toCopy );
	return *this;
}


template < typename T, STRATEGY O >
template < typename S, STRATEGY P >
GML_INLINE void Rotation< T, O >::set ( const Rotation< S, P > &toCopy )
{
	this->q = toCopy.q;
}


//-------------------------------------------------------------------------
// conversion

template < typename T, STRATEGY O >
GML_FORCEINLINE Quaternion< T, O > & Rotation< T, O >::get ()
{
	return this->q;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE const Quaternion< T, O > & Rotation< T, O >::get () const
{
	return this->q;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O >::operator quaternion_t & ()
{
	return this->q;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O >::operator quaternion_t const & () const
{
	return this->q;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE void Rotation< T, O >::toQuaternion ( quaternion_t &_q ) const
{
	_q = this->q;
}


/*
 * matrix formulation from
 * http://www.gamasutra.com/features/19980703/quaternions_01.htm
 * CORRECTED: term (1,1) is given as 1-2y^2-2x^2, but should be
 * 1-2y^2-2z^2 so i've corrected it here.
 * CORRECTED: term (2,3) is given as 2yz-2wx, but should be
 * 2yz+2wx so i've corrected it here.
 * better source is the quaternion and matrix faq
 * http://skal.planet-d.net/demo/matrixfaq.htm

  1-2y^2-2z^2    2xy+2wz     2xz-2wy      0
  2xy-2wz      1-2x^2-2z^2   2yz+2wx      0
  2xz+2wy        2yz-2wx    1-2x^2-2y^2   0
     0              0           0         1

* hm, for some reason need the tranpose of this to match the
* behavior of the Transform class.
*/
template < typename T, STRATEGY O >
GML_INLINE void Rotation< T, O >::toMatrix ( matrix_t &m ) const
{
	T len = this->q.length();
	T x = this->q.x / len;
	T y = this->q.y / len;
	T z = this->q.z / len;
	T w = this->q.w / len;

	T xx = 2 * x * x;
	T xy = 2 * x * y;
	T xz = 2 * x * z;
	T xw = 2 * x * w;
	T yy = 2 * y * y;
	T yz = 2 * y * z;
	T yw = 2 * y * w;
	T zz = 2 * z * z;
	T zw = 2 * z * w;

	m( 0, 0 ) = 1 - yy - zz;
	m( 0, 1 ) = xy - zw;
	m( 0, 2 ) = xz + yw;
	m( 0, 3 ) = 0;
	m( 1, 0 ) = xy + zw;
	m( 1, 1 ) = 1 - xx - zz;
	m( 1, 2 ) = yz - xw;
	m( 1, 3 ) = 0;
	m( 2, 0 ) = xz - yw;
	m( 2, 1 ) = yz + xw;
	m( 2, 2 ) = 1 - xx - yy;
	m( 2, 3 ) = 0;
	m( 3, 0 ) = 0;
	m( 3, 1 ) = 0;
	m( 3, 2 ) = 0;
	m( 3, 3 ) = 1;
}


template < typename T, STRATEGY O >
GML_INLINE void Rotation< T, O >::toAxisAngle ( vector3_t &axis, value_t &angle ) const
{
	T qlensqr = this->q.lengthSqr();
	if( qlensqr == 0 )
	{
		axis = vector3_t( 1, 0, 0 );
		angle = 0;
		return;
	}

	axis = vector3_t(this-> q.x, this->q.y, this->q.z );
	T lensqr = axis.lengthSqr();
	if( lensqr == 0 )
	{
		axis = vector3_t( 1, 0, 0 );
		angle = 0;
	}
	else
	{
		value_t len = sqrt< T >( lensqr );
		axis /= len;
		value_t qlen = sqrt< T >( qlensqr );
		angle = ( T )( 2.0 * acos< T >( clamp( this->q.w / qlen, -1.0, 1.0 ) ) );

		if( angle > M_PI )
		{
			angle = ( T )( M_PI * 2.0 - angle );
			axis *= -1;
		}
	}
}


template < typename T, STRATEGY O >
GML_INLINE void Rotation< T, O >::toAxisAngle ( vector4_t &axis, value_t &angle ) const
{
	T qlensqr = this->q.lengthSqr();
	if( qlensqr == 0 )
	{
		axis = vector4_t( 1, 0, 0, 0 );
		angle = 0;
		return;
	}

	axis = vector4_t( this->q.x, this->q.y, this->q.z, 0 );
	T lensqr = axis.lengthSqr();
	if( lensqr == 0 )
	{
		axis = vector4_t( 1, 0, 0, 0 );
		angle = 0;
	}
	else
	{
		value_t len = sqrt< T >( lensqr );
		axis /= len;
		value_t qlen = sqrt< T >( qlensqr );
		angle = ( T )( 2.0 * acos< T >( clamp( this->q.w / qlen, -1.0, 1.0 ) ) );

		if( angle > M_PI )
		{
			angle = ( T )( M_PI * 2.0 - angle );
			axis *= -1;
		}
	}
}


/*
 * http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm

  yaw   = atan2(2*qy*qw-2*qx*qz , 1 - 2*qy2 - 2*qz2)
  pitch = asin(2*qx*qy + 2*qz*qw)
  roll  = atan2(2*qx*qw-2*qy*qz , 1 - 2*qx2 - 2*qz2)

  except when qx*qy + qz*qw = 0.5 (north pole) which gives:
    yaw = 2 * atan2(x,w)
    roll = 0
  and when qx*qy + qz*qw = -0.5 (south pole) which gives:
    yaw = -2 * atan2(x,w)
    roll = 0

 * CORRECTED: had to swap x and z quat terms, because math above
 * uses coord system where x is roll axis and z is pitch axis.
 */
template < typename T, STRATEGY O >
GML_INLINE void Rotation< T, O >::toEuler ( value_t &yaw, value_t &pitch, value_t &roll ) const
{
	T len = this->q.length();
	T x = this->q.x / len;
	T y = this->q.y / len;
	T z = this->q.z / len;
	T w = this->q.w / len;

	T singularity = z * y + x * w;
	if( singularity >= 0.5 )
	{
		yaw   = ( T )( 2.0 * atan2( z, w ) );
		pitch = ( T )( M_PI / 2.0 );
		roll  = 0.0;
	}
	else if( singularity <= -0.5 )
	{
		yaw   = ( T )( - 2.0 * atan2( z, w ) );
		pitch = ( T )( - M_PI / 2.0 );
		roll  = 0.0;
	}
	else
	{
		yaw   = ( T )atan2( 2.0 * y * w - 2.0 * x * z,
			1.0 - 2.0 * y * y - 2.0 * x * x );
		pitch = ( T )asin( 2.0 * z * y + 2.0 * x * w );
		roll  = ( T )atan2( 2.0 * z * w - 2.0 * y * x,
			1.0 - 2.0 * x * x - 2.0 * z * z );
	}
}


//-------------------------------------------------------------------------
// binary rotation ops

template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > Rotation< T, O >::operator * ( const Rotation &rhs ) const
{
	return Rotation< T, O >( this->q * rhs.q );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > & Rotation< T, O >::operator *= ( const Rotation &rhs )
{
	this->q *= rhs.q;
	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > Rotation< T, O >::operator / ( const Rotation &rhs ) const
{
	return Rotation< T, O >( this->q * rhs.q.conjugate() );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > & Rotation< T, O >::operator /= ( const Rotation &rhs )
{
	this->q *= rhs.q.conjugate();
	return *this;
}


//-------------------------------------------------------------------------
// scalar ops

template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > Rotation< T, O >::operator + ( const value_t &rhs ) const
{
	vector3_t axis;
	value_t angle;
	toAxisAngle( axis, angle );
	return Rotation( axis, angle + rhs );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > Rotation< T, O >::operator - ( const value_t &rhs ) const
{
	vector3_t axis;
	value_t angle;
	toAxisAngle( axis, angle );
	return Rotation( axis, angle - rhs );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > Rotation< T, O >::operator * ( const value_t &rhs ) const
{
	vector3_t axis;
	value_t angle;
	toAxisAngle( axis, angle );
	return Rotation( axis, angle * rhs );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > Rotation< T, O >::operator / ( const value_t &rhs ) const
{
	vector3_t axis;
	value_t angle;
	toAxisAngle( axis, angle );
	return Rotation( axis, angle / rhs );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > & Rotation< T, O >::operator += ( const value_t &rhs )
{
	vector3_t axis;
	value_t angle;
	toAxisAngle( axis, angle );
	set( axis, angle + rhs );
	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > & Rotation< T, O >::operator -= ( const value_t &rhs )
{
	vector3_t axis;
	value_t angle;
	toAxisAngle( axis, angle );
	set( axis, angle - rhs );
	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > & Rotation< T, O >::operator *= ( const value_t &rhs )
{
	vector3_t axis;
	value_t angle;
	toAxisAngle( axis, angle );
	set( axis, angle * rhs );
	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > & Rotation< T, O >::operator /= ( const value_t &rhs )
{
	vector3_t axis;
	value_t angle;
	toAxisAngle( axis, angle );
	set( axis, angle / rhs );
	return *this;
}


//-------------------------------------------------------------------------
// unary

template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > Rotation< T, O >::operator - () const
{
	return inverse();
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > Rotation< T, O >::inverse () const
{
	Rotation out;
	out.q = this->q.conjugate();
	return out;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE void Rotation< T, O >::normalize ()
{
	this->q /= this->q.length();
}


//-------------------------------------------------------------------------
// apply rotation

template < typename T, STRATEGY O >
GML_FORCEINLINE Vector< 3, T, O > Rotation< T, O >::transform ( const Vector< 3, T, O > &v ) const
{
	Quaternion< T, O > vq( v.x, v.y, v.z, 0 );
	Quaternion< T, O > rq = this->q * vq * this->q.conjugate();
//	Vector< 4, T, O > r = rq;
	return Vector< 3, T, O >( rq.x, rq.y, rq.z );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Vector< 4, T, O > Rotation< T, O >::transform ( const Vector< 4, T, O > &v ) const
{
	Quaternion< T, O > vq( v.x, v.y, v.z, 0 );
	Quaternion< T, O > rq = this->q * vq * this->q.conjugate();
	Vector< 4, T, O > r( rq );
	r.w = v.w;
	return r;
}


//-------------------------------------------------------------------------
// comparisons

template < typename T, STRATEGY O >
GML_FORCEINLINE bool Rotation< T, O >::operator == ( const Rotation &rhs ) const
{
	return ( this->q == rhs.q );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE bool Rotation< T, O >::operator != ( const Rotation &rhs ) const
{
	return ( this->q != rhs.q );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE bool Rotation< T, O >::operator <  ( const Rotation &rhs ) const
{
	return ( this->q < rhs.q );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE bool Rotation< T, O >::operator >  ( const Rotation &rhs ) const
{
	return ( this->q > rhs.q );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE bool Rotation< T, O >::operator <= ( const Rotation &rhs ) const
{
	return ( this->q <= rhs.q );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE bool Rotation< T, O >::operator >= ( const Rotation &rhs ) const
{
	return ( this->q >= rhs.q );
}


template < typename T, STRATEGY O >
GML_INLINE bool Rotation< T, O >::realeq ( const Rotation &rhs, value_t eps ) const
{
	vector4_t axis;
	value_t angle;
	Rotation< T, O > diff = inverse() * rhs;
	diff.toAxisAngle( axis, angle );
	return ( abs< T >( angle ) < eps );

//	return this->q.realeq( rhs.q, eps );
}



//-------------------------------------------------------------------------
// opengl support

#if GML_SUPPORT_OPENGL

template < typename T, STRATEGY O >
GML_FORCEINLINE void Rotation< T, O >::glRotate () const
{
	vector3_t axis;
	value_t angle;
	toAxisAngle( axis, angle );
	glRotateTmpl( rad2deg( angle ), axis.x, axis.y, axis.z );
}


#endif // GML_SUPPORT_OPENGL


//-------------------------------------------------------------------------
// helpers

template < typename T, STRATEGY O >
GML_FORCEINLINE Rotation< T, O > match ( const Vector< 3, T, O > &u, 
						const Vector< 3, T, O > &v,
						T eps )
{
	Vector< 4, T, O > u4( u ), v4( v );
	return match( u4, v4, eps );
}


template < typename T, STRATEGY O >
GML_INLINE Rotation< T, O > match ( const Vector< 4, T, O > &u, 
						const Vector< 4, T, O > &v,
						T eps )
{
	Vector< 4, T, O > axis;
	T angle;

	T dp = dot( u, v );
	if( dp >= ( 1 - eps ) )
	{
		// vectors are already aligned
		angle = 0;
		axis = Vector< 4, T, O >( 1, 0, 0, 0 );
	}
	else if( dp <= ( -1 + eps ) )
	{
		// vectors are opposed (anti-parallel).
		// construct any random perpendicular vector
		if( fabs( v.x ) > fabs( v.y ) && fabs( v.x ) > fabs( v.z ) )
			axis = Vector< 4, T, O >( v.y, - v.x, 0, 0 );
		else
			axis = Vector< 4, T, O >( 0, - v.z, v.y, 0 );
		axis.normalize();
		angle = ( T )M_PI;
	}
	else
	{
		angle = acos( dp );
		axis = cross( u, v );
		axis.normalize();
	}

	return Rotation< T, O >( axis, angle );
}


template < typename T, STRATEGY O >
GML_INLINE Rotation< T, O > trackball ( T p0x, T p0y, T p1x, T p1y )
{
	// restrict to unit circle
	T len0 = sqrt< T >( p0x*p0x + p0y*p0y );
	if( len0 > 1.0 )
	{
		p0x /= len0;
		p0y /= len0;
	}
	T len1 = sqrt< T >( p1x*p1x + p1y*p1y );
	if( len1 > 1.0 )
	{
		p1x /= len1;
		p1y /= len1;
	}

	if( p0x == p1x && p0y == p1y )
		return Rotation< T, O >::identity();

	// project to sphere
	Vector< 4, T, O > s0, s1;
	if( len0 >= 1.0 )
		s0 = Vector< 4, T, O >( p0x, p0y, 0.0, 0.0 );
	else
		s0 = Vector< 4, T, O >( p0x, p0y,
			sqrt< T >( ( T )1.0 - p0x*p0x - p0y*p0y ), 0.0 );
	if( len1 >= 1.0 )
		s1 = Vector< 4, T, O >( p1x, p1y, 0.0, 0.0 );
	else
		s1 = Vector< 4, T, O >( p1x, p1y,
			sqrt< T >( ( T )1.0 - p1x*p1x - p1y*p1y ), 0.0 );

	// cross prod for vec
	Vector< 4, T, O > axis = cross( s0, s1 );
	axis.normalize();

	// dot prod for theta
	T angle = acos< T >( clamp( dot( s0, s1 ), 0, 1 ) );

	return Rotation< T, O >( axis, angle );
}


/*
 * from http://www.gamedev.net/community/forums/topic.asp?topic_id=25314
 * another useful reference:
 * http://number-none.com/product/Understanding Slerp, Then Not Using It/
 */
template < typename T, STRATEGY O >
GML_INLINE Rotation< T, O > slerp ( const Rotation< T, O > &lhs, 
						const Rotation< T, O > &rhs,
						T alpha )
{
	T costheta = dot( lhs.q, rhs.q );
	Rotation< T, O > r;

	T sign = ( T )( ( costheta > 0.0 ) ? 1 : -1 );
	costheta *= sign;

	/*
	 * can't slerp with vectors are coincident, causes divide
	 * by zero error.  instead use nlerp in that case.
	 */
	static const float DOT_THRESHOLD = 0.995f;
	if( costheta > DOT_THRESHOLD )
		return nlerp( lhs, rhs, alpha );

	T theta = acos< T >( costheta );
	T sintheta = sqrt< T >( ( T )1.0 - costheta*costheta );

	T sin_t_theta = sin( alpha * theta ) / sintheta;
	T sin_oneminust_theta = sin( ( ( T )1.0 - alpha ) * theta ) / sintheta;

	r.q = lhs.q * sin_oneminust_theta;
	r.q += rhs.q * sin_t_theta * sign;

	return r;
}


template < typename T, STRATEGY O >
GML_INLINE Rotation< T, O > nlerp ( const Rotation< T, O > &lhs, 
						const Rotation< T, O > &rhs,
						T alpha )
{
	T dp = dot( lhs.q, rhs.q );
	Rotation< T, O > r;

	if( dp < 0.0 )
		r.q = lhs.q - ( rhs.q + lhs.q ) * alpha;
	else
		r.q = lhs.q + ( rhs.q - lhs.q ) * alpha;
	r.normalize();

	return r;
}


/*
 * q1 = q0 + t * 1/2 * ( w * q0 )
 */
template < typename T, STRATEGY O >
GML_INLINE Rotation< T, O > integrate ( const Rotation< T, O > &r0,
							T wx, T wy, T wz, T dt )
{
	Quaternion< T, O > wq( wx, wy, wz, 0 );
	return Rotation< T, O >( r0.q + ( wq * r0.q ) * ( dt * ( T )0.5 ) );
}


/*
 * w = ( 2 / dt ) * ( q1 - q0 ) * q0^-1
 */
template < typename T, STRATEGY O >
GML_INLINE void omega ( const Rotation< T, O > &r0,
			const Rotation< T, O > &r1,
			T dt, T &wx, T &wy, T &wz )
{
	Quaternion< T, O > wq = ( ( T )2.0 / dt ) * ( ( r1.q - r0.q ) * r0.q.conjugate() );

	wx = wq.x;
	wy = wq.y;
	wz = wq.z;
}


//-------------------------------------------------------------------------

GML_NAMESPACE_END

//-------------------------------------------------------------------------
// external functions

#if GML_SUPPORT_STL

template < typename T, gml::STRATEGY O >
GML_INLINE std::ostream & operator << ( std::ostream &out, const gml::Rotation< T, O > &r )
{
	gml::Vector< 3, T, O > axis;
	T angle;
	r.toAxisAngle( axis, angle );

	out << "[ " << rad2deg( angle ) << " @ " << axis << " ]";
	return out;
}

#endif // GML_SUPPORT_STL
