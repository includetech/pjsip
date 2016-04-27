

GML_NAMESPACE_BEGIN

//-------------------------------------------------------------------------
// Static members

template < typename T, STRATEGY O >
GML_FORCEINLINE Transform< T, O > Transform< T, O >::identity ()
{
	Transform t;
	t.setIdentity();
	return t;
}


//-------------------------------------------------------------------------
// Constructors

template < typename T, STRATEGY O >
GML_FORCEINLINE Transform< T, O >::Transform ()
{
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Transform< T, O >::Transform ( const Transform &toCopy )
{
	set( toCopy );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Transform< T, O >::Transform ( const matrix_t &_m )
{
	set( _m );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Transform< T, O > & Transform< T, O >::operator = ( const Transform &toCopy )
{
	set( toCopy );
	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Transform< T, O > & Transform< T, O >::operator = ( const matrix_t &_m )
{
	set( _m );
	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Transform< T, O > & Transform< T, O >::set ( const Transform &toCopy )
{
	this->m = toCopy.m;
	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Transform< T, O > & Transform< T, O >::set ( const matrix_t &_m )
{
	this->m = _m;
	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Transform< T, O > & Transform< T, O >::setIdentity ()
{
	this->m.setIdentity();
	return *this;
}


//-------------------------------------------------------------------------
//

template < typename T, STRATEGY O >
GML_FORCEINLINE Matrix< 4, T, O > & Transform< T, O >::get ()
{
	return this->m;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE const Matrix< 4, T, O > & Transform< T, O >::get () const
{
	return this->m;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Transform< T, O >::operator matrix_t & ()
{
	return this->m;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Transform< T, O >::operator matrix_t const & () const
{
	return this->m;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Transform< T, O >::operator value_t * ()
{
	return ( value_t * )this->m;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Transform< T, O >::operator value_t const * () const
{
	return ( const value_t * )this->m;
}


//-------------------------------------------------------------------------
//

template < typename T, STRATEGY O >
GML_INLINE Transform< T, O > & Transform< T, O >::setFrustum (
	T left, T right, T bottom, T top, T nearplane, T farplane )
{
	this->m( 0, 0 ) = 2 * nearplane / ( right - left );
	this->m( 0, 1 ) = 0;
	this->m( 0, 2 ) = ( right + left ) / ( right - left );
	this->m( 0, 3 ) = 0;
	this->m( 1, 0 ) = 0;
	this->m( 1, 1 ) = 2 * nearplane / ( top - bottom );
	this->m( 1, 2 ) = ( top + bottom ) / ( top - bottom );
	this->m( 1, 3 ) = 0;
	this->m( 2, 0 ) = 0;
	this->m( 2, 1 ) = 0;
	this->m( 2, 2 ) = - ( farplane + nearplane ) / ( farplane - nearplane );
	this->m( 2, 3 ) = - 2 * farplane * nearplane / ( farplane - nearplane );
	this->m( 3, 0 ) = 0;
	this->m( 3, 1 ) = 0;
	this->m( 3, 2 ) = -1;
	this->m( 3, 3 ) = 0;

	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Transform< T, O > & Transform< T, O >::frustum (
	T left, T right, T bottom, T top, T nearplane, T farplane )
{
	Transform< T, O > t;
	t.setFrustum( left, right, bottom, top, nearplane, farplane );
	matrix_t m0 = this->m;
	this->m.mul( m0, t.m );

	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Transform< T, O > & Transform< T, O >::setPerspective (
	T fovy, T aspect, T nearplane, T farplane )
{
	T ymax = nearplane * tan( ( T )( deg2rad( fovy ) / 2.0 ) );
	T ymin = -ymax;
	T xmin = ymin * aspect;
	T xmax = ymax * aspect;

	setFrustum( xmin, xmax, ymin, ymax, nearplane, farplane );

	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Transform< T, O > & Transform< T, O >::perspective (
	T fovy, T aspect, T nearplane, T farplane )
{
	Transform< T, O > t;
	t.setPerspective( fovy, aspect, nearplane, farplane );
	matrix_t m0 = this->m;
	this->m.mul( m0, t.m );

	return *this;
}


template < typename T, STRATEGY O >
GML_INLINE Transform< T, O > & Transform< T, O >::setOrtho (
	T left, T right, T bottom, T top, T nearplane, T farplane )
{
	this->m( 0, 0 ) = 2 / ( right - left );
	this->m( 0, 1 ) = 0;
	this->m( 0, 2 ) = 0;
	this->m( 0, 3 ) = - ( right + left ) / ( right - left );
	this->m( 1, 0 ) = 0;
	this->m( 1, 1 ) = 2 / ( top - bottom );
	this->m( 1, 2 ) = 0;
	this->m( 1, 3 ) = - ( top + bottom ) / ( top - bottom );
	this->m( 2, 0 ) = 0;
	this->m( 2, 1 ) = 0;
	this->m( 2, 2 ) = - 2 / ( farplane - nearplane );
	this->m( 2, 3 ) = - ( farplane + nearplane ) / ( farplane - nearplane );
	this->m( 3, 0 ) = 0;
	this->m( 3, 1 ) = 0;
	this->m( 3, 2 ) = 0;
	this->m( 3, 3 ) = 1;

	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Transform< T, O > & Transform< T, O >::ortho (
	T left, T right, T bottom, T top, T nearplane, T farplane )
{
	Transform< T, O > t;
	t.setOrtho( left, right, bottom, top, nearplane, farplane );
	matrix_t m0 = this->m;
	this->m.mul( m0, t.m );

	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Transform< T, O > & Transform< T, O >::setOrtho2D (
	T left, T right, T bottom, T top )
{
	setOrtho( left, right, bottom, top, -1.0, 1.0 );

	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Transform< T, O > & Transform< T, O >::ortho2D (
	T left, T right, T bottom, T top )
{
	Transform< T, O > t;
	t.setOrtho( left, right, bottom, top, -1.0, 1.0  );
	matrix_t m0 = this->m;
	this->m.mul( m0, t.m );

	return *this;
}


template < typename T, STRATEGY O >
GML_INLINE Transform< T, O > & Transform< T, O >::setLookAt (
	T eyex, T eyey, T eyez, 
	T targetx, T targety, T targetz, 
	T upx, T upy, T upz)
{
	// first determine camera's x, y, and z vectors.  then use them
	// to generate the transformation matrix's rotation component.
	// translation component is specified by eye parameter.

	Vector< 4, T, O > z = ( Vector< 4, T, O >( eyex, eyey, eyez, 0 ) 
	  - Vector< 4, T, O >( targetx, targety, targetz, 0 ) );
	z.normalize();

	Vector< 4, T, O > y = Vector< 4, T, O >( upx, upy, upz );

	Vector< 4, T, O > x = y.cross( z );
	y = z.cross( x );

	x.normalize();
	y.normalize();

	Vector< 4, T, O > t( -eyex, -eyey, -eyez, 0 );

	this->m( 0, 0 ) = x.x;
	this->m( 0, 1 ) = x.y;
	this->m( 0, 2 ) = x.z;
	this->m( 0, 3 ) = t.x * x.x + t.y * x.y + t.z * x.z;
	this->m( 1, 0 ) = y.x;
	this->m( 1, 1 ) = y.y;
	this->m( 1, 2 ) = y.z;
	this->m( 1, 3 ) = t.x * y.x + t.y * y.y + t.z * y.z;
	this->m( 2, 0 ) = z.x;
	this->m( 2, 1 ) = z.y;
	this->m( 2, 2 ) = z.z;
	this->m( 2, 3 ) = t.x * z.x + t.y * z.y + t.z * z.z;	
	this->m( 3, 0 ) = 0;
	this->m( 3, 1 ) = 0;
	this->m( 3, 2 ) = 0;
	this->m( 3, 3 ) = 1;

	return *this;
}


template < typename T, STRATEGY O >
GML_FORCEINLINE Transform< T, O > & Transform< T, O >::lookAt (
	T eyex, T eyey, T eyez, 
	T targetx, T targety, T targetz, 
	T upx, T upy, T upz )
{
	Transform< T, O > t;
	t.setLookAt( eyex, eyey, eyez, targetx, targety, targetz, upx, upy, upz );
	matrix_t m0 = this->m;
	this->m.mul( m0, t.m );

	return *this;
}


//-------------------------------------------------------------------------
//

template < typename T, STRATEGY O >
GML_INLINE Transform< T, O > & Transform< T, O >::setTranslate (
	T x, T y, T z )
{
	this->m( 0, 0 ) = 1;
	this->m( 0, 1 ) = 0;
	this->m( 0, 2 ) = 0;
	this->m( 0, 3 ) = x;
	this->m( 1, 0 ) = 0;
	this->m( 1, 1 ) = 1;
	this->m( 1, 2 ) = 0;
	this->m( 1, 3 ) = y;
	this->m( 2, 0 ) = 0;
	this->m( 2, 1 ) = 0;
	this->m( 2, 2 ) = 1;
	this->m( 2, 3 ) = z;
	this->m( 3, 0 ) = 0;
	this->m( 3, 1 ) = 0;
	this->m( 3, 2 ) = 0;
	this->m( 3, 3 ) = 1;

	return *this;
}


template < typename T, STRATEGY O >
GML_INLINE Transform< T, O > & Transform< T, O >::translate (
	T x, T y, T z )
{
	//TODO can be faster - no need for a temporary!
	Transform< T, O > t;
	t.setTranslate( x, y, z );
	matrix_t m0 = this->m;
	this->m.mul( m0, t.m );

	return *this;
}


template < typename T, STRATEGY O >
GML_INLINE Transform< T, O > & Transform< T, O >::setScale (
	T x, T y, T z )
{
	this->m( 0, 0 ) = x;
	this->m( 0, 1 ) = 0;
	this->m( 0, 2 ) = 0;
	this->m( 0, 3 ) = 0;
	this->m( 1, 0 ) = 0;
	this->m( 1, 1 ) = y;
	this->m( 1, 2 ) = 0;
	this->m( 1, 3 ) = 0;
	this->m( 2, 0 ) = 0;
	this->m( 2, 1 ) = 0;
	this->m( 2, 2 ) = z;
	this->m( 2, 3 ) = 0;
	this->m( 3, 0 ) = 0;
	this->m( 3, 1 ) = 0;
	this->m( 3, 2 ) = 0;
	this->m( 3, 3 ) = 1;

	return *this;
}


template < typename T, STRATEGY O >
GML_INLINE Transform< T, O > & Transform< T, O >::scale (
	T x, T y, T z )
{
	//TODO can be faster - no need for a temporary!
	Transform< T, O > t;
	t.setScale( x, y, z );
	matrix_t m0 = this->m;
	this->m.mul( m0, t.m );

	return *this;
}


template < typename T, STRATEGY O >
GML_INLINE Transform< T, O > & Transform< T, O >::setRotate (
	T angle, T x, T y, T z )
{
	T len = Vector< 4, T, O >( x, y, z, 0 ).length();
	x /= len;
	y /= len;
	z /= len;

	T c = cos< T >( deg2rad( angle ) );
	T s = sin< T >( deg2rad( angle ) );
	T C = ( T )( 1.0 - c );

	this->m( 0, 0 ) = x * x * C + c;
	this->m( 0, 1 ) = x * y * C - z * s;
	this->m( 0, 2 ) = x * z * C + y * s;
	this->m( 0, 3 ) = 0;
	this->m( 1, 0 ) = y * x * C + z * s;
	this->m( 1, 1 ) = y * y * C + c;
	this->m( 1, 2 ) = y * z * C - x * s;
	this->m( 1, 3 ) = 0;
	this->m( 2, 0 ) = z * x * C - y * s;
	this->m( 2, 1 ) = z * y * C + x * s;
	this->m( 2, 2 ) = z * z * C + c;
	this->m( 2, 3 ) = 0;
	this->m( 3, 0 ) = 0;
	this->m( 3, 1 ) = 0;
	this->m( 3, 2 ) = 0;
	this->m( 3, 3 ) = 1;

	return *this;
}


template < typename T, STRATEGY O >
GML_INLINE Transform< T, O > & Transform< T, O >::rotate (
	T angle, T x, T y, T z )
{
	//TODO can be faster - no need for a temporary!
	Transform< T, O > t;
	t.setRotate( angle, x, y, z );
	matrix_t m0 = this->m;
	this->m.mul( m0, t.m );

	return *this;
}


template < typename T, STRATEGY O >
GML_INLINE Transform< T, O > & Transform< T, O >::setRotateX ( T angle )
{
	T st = sin< T >( deg2rad( angle ) );
	T ct = cos< T >( deg2rad( angle ) );

	this->m( 0, 0 ) = 1;
	this->m( 0, 1 ) = 0;
	this->m( 0, 2 ) = 0;
	this->m( 0, 3 ) = 0;
	this->m( 1, 0 ) = 0;
	this->m( 1, 1 ) = ct;
	this->m( 1, 2 ) = -st;
	this->m( 1, 3 ) = 0;
	this->m( 2, 0 ) = 0;
	this->m( 2, 1 ) = st;
	this->m( 2, 2 ) = ct;
	this->m( 2, 3 ) = 0;
	this->m( 3, 0 ) = 0;
	this->m( 3, 1 ) = 0;
	this->m( 3, 2 ) = 0;
	this->m( 3, 3 ) = 1;

	return *this;
}


template < typename T, STRATEGY O >
GML_INLINE Transform< T, O > & Transform< T, O >::rotateX ( T angle )
{
	//TODO can be faster - no need for a temporary!
	Transform< T, O > t;
	t.setRotateX( angle );
	matrix_t m0 = this->m;
	this->m.mul( m0, t.m );

	return *this;
}


template < typename T, STRATEGY O >
GML_INLINE Transform< T, O > & Transform< T, O >::setRotateY ( T angle )
{
	T st = sin< T >( deg2rad( angle ) );
	T ct = cos< T >( deg2rad( angle ) );

	this->m( 0, 0 ) = ct;
	this->m( 0, 1 ) = 0;
	this->m( 0, 2 ) = st;
	this->m( 0, 3 ) = 0;
	this->m( 1, 0 ) = 0;
	this->m( 1, 1 ) = 1;
	this->m( 1, 2 ) = 0;
	this->m( 1, 3 ) = 0;
	this->m( 2, 0 ) = -st;
	this->m( 2, 1 ) = 0;
	this->m( 2, 2 ) = ct;
	this->m( 2, 3 ) = 0;
	this->m( 3, 0 ) = 0;
	this->m( 3, 1 ) = 0;
	this->m( 3, 2 ) = 0;
	this->m( 3, 3 ) = 1;

	return *this;
}


template < typename T, STRATEGY O >
GML_INLINE Transform< T, O > & Transform< T, O >::rotateY ( T angle )
{
	//TODO can be faster - no need for a temporary!
	Transform< T, O > t;
	t.setRotateY( angle );
	matrix_t m0 = this->m;
	this->m.mul( m0, t.m );

	return *this;
}


template < typename T, STRATEGY O >
GML_INLINE Transform< T, O > & Transform< T, O >::setRotateZ ( T angle )
{
	T st = sin< T >( deg2rad( angle ) );
	T ct = cos< T >( deg2rad( angle ) );

	this->m( 0, 0 ) = ct;
	this->m( 0, 1 ) = -st;
	this->m( 0, 2 ) = 0;
	this->m( 0, 3 ) = 0;
	this->m( 1, 0 ) = st;
	this->m( 1, 1 ) = ct;
	this->m( 1, 2 ) = 0;
	this->m( 1, 3 ) = 0;
	this->m( 2, 0 ) = 0;
	this->m( 2, 1 ) = 0;
	this->m( 2, 2 ) = 1;
	this->m( 2, 3 ) = 0;
	this->m( 3, 0 ) = 0;
	this->m( 3, 1 ) = 0;
	this->m( 3, 2 ) = 0;
	this->m( 3, 3 ) = 1;

	return *this;
}


template < typename T, STRATEGY O >
GML_INLINE Transform< T, O > & Transform< T, O >::rotateZ ( T angle )
{
	//TODO can be faster - no need for a temporary!
	Transform< T, O > t;
	t.setRotateZ( angle );
	matrix_t m0 = this->m;
	this->m.mul( m0, t.m );

	return *this;
}


template < typename T, STRATEGY O >
GML_INLINE Transform< T, O > & Transform< T, O >::setRotate (
	const rotation_t &r )
{
	r.toMatrix( this->m );

	return *this;
}


template < typename T, STRATEGY O >
GML_INLINE Transform< T, O > & Transform< T, O >::rotate (
	const rotation_t &r )
{
	//TODO can be faster - no need for a temporary!
	Transform< T, O > t;
	t.setRotate( r );
	matrix_t m0 = this->m;
	this->m.mul( m0, t.m );

	return *this;
}


//-------------------------------------------------------------------------
//

template < typename T, STRATEGY O >
GML_FORCEINLINE Transform< T, O > & Transform< T, O >::invert ()
{
	matrix_t tmp( this->m );
	this->m.invert( tmp );
	return *this;
}


//-------------------------------------------------------------------------
// opengl support

#if GML_SUPPORT_OPENGL

template < typename T, STRATEGY O >
GML_FORCEINLINE void Transform< T, O >::glGet ( GLenum which )
{
	glGetvTmpl< T >( which, this->m );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE void Transform< T, O >::glLoadMatrix () const
{
	glLoadMatrixTmpl< T >( this->m );
}


template < typename T, STRATEGY O >
GML_FORCEINLINE void Transform< T, O >::glMultMatrix () const
{
	glMultMatrixTmpl< T >( this->m );
}

#endif // GML_SUPPORT_OPENGL


//-------------------------------------------------------------------------
//

template < typename T, STRATEGY O >
GML_INLINE void project ( T objx, T objy, T objz,
					  const Transform< T, O > &modelview, 
					  const Transform< T, O > &proj, 
					  const int4 &viewport, 
					  T &winx, T &winy, T &winz )
{
	Vector< 4, T, O > obj( objx, objy, objz, 1 );

	Vector< 4, T, O > clip = proj.get() * ( modelview.get() * obj );
	if( clip.w == 0 )
		return;

	clip.x /= clip.w;
	clip.y /= clip.w;
	clip.z /= clip.w;

	winx = viewport.x + ( 1 + clip.x ) * viewport.z / 2;
	winy = viewport.y + ( 1 + clip.y ) * viewport.w / 2;
	winz = ( 1 + clip.z ) / 2;
}


template < typename T, STRATEGY O >
GML_INLINE void unproject ( T winx, T winy, T winz,
						const Transform< T, O > &modelview, 
						const Transform< T, O > &proj,
						const int4 &viewport,
						T &objx, T &objy, T &objz )
{
	Vector< 4, T, O > clip;
	clip.x = ( winx - viewport.x ) * 2 / viewport.z - 1;
	clip.y = ( winy - viewport.y ) * 2 / viewport.w - 1;
	clip.z = 2 * winz - 1;
	clip.w = 1;

	Matrix< 4, T, O > mvp;
	mvp.mul( proj.get(), modelview.get() );

	Matrix< 4, T, O > mvpinv;
	if( ! mvpinv.invert( mvp ) )
		return;

	Vector< 4, T, O > obj = mvpinv * clip;
	if( obj.w == 0 )
		return;

	objx = obj.x / obj.w;
	objy = obj.y / obj.w;
	objz = obj.z / obj.w;
}


template < typename T, STRATEGY O >
GML_INLINE void unproject ( T winx, T winy, T winz,
						const Transform< T, O > &mvpinv, 
						const int4 &viewport,
						T &objx, T &objy, T &objz )
{
	Vector< 4, T, O > clip;
	clip.x = ( winx - viewport.x ) * 2 / viewport.z - 1;
	clip.y = ( winy - viewport.y ) * 2 / viewport.w - 1;
	clip.z = 2 * winz - 1;
	clip.w = 1;

	Vector< 4, T, O > obj = mvpinv.get() * clip;
	if( obj.w == 0 )
		return;

	objx = obj.x / obj.w;
	objy = obj.y / obj.w;
	objz = obj.z / obj.w;
}


//-------------------------------------------------------------------------

GML_NAMESPACE_END

//-------------------------------------------------------------------------
// external functions

#if GML_SUPPORT_STL

template < typename T, gml::STRATEGY O >
GML_INLINE std::ostream & operator << ( std::ostream &out, const gml::Transform< T, O > &xform )
{
	out << xform.m;
	return out;
}

#endif // GML_SUPPORT_STL

