#pragma once

#include <format>

#include "boost/container_hash/hash.hpp"

namespace Vector2D
{
	struct Vector2D
	{
		double x, y;

		Vector2D() = default;

		Vector2D(double a, double b) : x(a), y(b) {}

		double& operator[](int i)
		{
			return ((&x)[i]);
		}

		const double& operator[](int i) const
		{
			return ((&x)[i]);
		}

		Vector2D& operator *=(double scalar)
		{
			x *= scalar;
			y *= scalar;
			return (*this);
		}

		Vector2D& operator /=(double scalar)
		{
			scalar = 1.0f / scalar;
			x *= scalar;
			y *= scalar;
			return (*this);
		}

		Vector2D& operator +=(const Vector2D& v)
		{
			x += v.x;
			y += v.y;
			return (*this);
		}

		Vector2D& operator -=(const Vector2D& v)
		{
			x -= v.x;
			y -= v.y;
			return *this;
		}

		friend size_t hash_value(const Vector2D& v)
		{
			size_t seed = 0;

			boost::hash_combine(seed, v.x);
			boost::hash_combine(seed, v.y);

			return seed;
		}

		std::string ToString() const
		{
			return std::format("({},{})", x, y);
		}
	};

	inline Vector2D operator *(const Vector2D& v, double scalar)
	{
		return Vector2D(v.x * scalar, v.y * scalar);
	}

	inline Vector2D operator /(const Vector2D& v, double scalar)
	{
		return Vector2D(v.x / scalar, v.y / scalar);
	}

	inline Vector2D operator -(const Vector2D& v)
	{
		return Vector2D(-v.x, -v.y);
	}

	inline Vector2D operator +(const Vector2D& v, const Vector2D& w)
	{
		return Vector2D(
			v.x + w.x,
			v.y + w.y
		);
	}

	inline Vector2D operator -(const Vector2D& v, const Vector2D& w)
	{
		return Vector2D(
			v.x - w.x,
			v.y - w.y
		);
	}

	inline double Magnitude(const Vector2D& v)
	{
		return sqrt(v.x * v.x + v.y * v.y);
	}

	inline Vector2D Normalize(const Vector2D& v)
	{
		return v / Magnitude(v);
	}
		
	inline bool operator ==(const Vector2D& v, const Vector2D& w)
	{
		return v.x == w.x && v.y == w.y;
	};
}