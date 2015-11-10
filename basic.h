#ifndef _4DSPA_BASIC_H
#define _4DSPA_BASIC_H

#include <cmath>
#include <iostream>

class VectorCoordinate
{
public:
	VectorCoordinate(double i, double j, double k): x(i), y(j), z(k){};
	VectorCoordinate():x(0.0), y(0.0), z(0.0){};
	VectorCoordinate(const VectorCoordinate& rhs)
	{
		x = rhs.x; y = rhs.y; z = rhs.z;
	}

	double x, y, z;

	friend bool operator == (const VectorCoordinate& lhs, const VectorCoordinate& rhs)
	{
		if( lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z)
			return false;
		else 
			return true;
		
	}

	friend bool operator != (const VectorCoordinate& lhs, const VectorCoordinate& rhs)
	{
		return !( lhs == rhs );
	}

	inline VectorCoordinate& operator /= (double s) 
	{
		if( s != 0 )
		{
			x /= s; y /= s; z /= s;
		}
		else
			std::cerr<<"divide by zero!\n";

		return *this;
	}

	inline VectorCoordinate& operator *= (double s )
	{
		x *= s; y *= s; z *= s;
		return *this;
	}

	inline VectorCoordinate& operator* ( double s )
	{
		x *= s; y *= s; z *= s;
		return *this;
	}

	inline  VectorCoordinate& operator+= ( const VectorCoordinate& rhs )
	{
		x += rhs.x; y += rhs.y; z += rhs.z;
		return *this;
	}

	friend  VectorCoordinate operator+ ( VectorCoordinate lhs, const VectorCoordinate& rhs)
	{
		lhs += rhs;
		return lhs; 
	}

	inline  VectorCoordinate& operator-= ( const VectorCoordinate& rhs )
	{
		x -= rhs.x; y -= rhs.y; z -= rhs.z;
		return *this;
	}

	friend  VectorCoordinate operator- ( VectorCoordinate lhs, const VectorCoordinate& rhs)
	{
		lhs -= rhs;
		return lhs; 
	}

    VectorCoordinate& operator= (const VectorCoordinate& rhs)
	{
		x = rhs.x; 
		y = rhs.y;
		z = rhs.z;

		return *this;
	}

	friend std::istream& operator>> ( std::istream& is, VectorCoordinate& Vec)
	{
		is>>Vec.x>>Vec.y>>Vec.z;
		return is;
	}

	friend std::ostream& operator<< (std::ostream& os, const VectorCoordinate& Vec)
	{
		os<<Vec.x<<'\t'<<Vec.y<<'\t'<<Vec.z;
		return os;
	}

	void Normalize()
	{
		double L = sqrt( x*x + y*y + z*z );
		if( L > 0 )
		{
			x /= L; y/=L; z/=L;
		}
	}

	double Length()
	{
		return sqrt( x*x + y*y + z*z );
	}
};


#endif
