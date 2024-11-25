#pragma once
#include <cmath>

template <typename T>
class Vec3
{
	public:
		T x, y, z;

		Vec3() : x(0), y(0), z(0) {}
		Vec3(T x, T y, T z) : x(x), y(y), z(z) {}
		Vec3(const Vec3& copy) : x(copy.x), y(copy.y), z(copy.z) {}

		Vec3 operator+(const Vec3& other) const {
			return Vec3(x + other.x, y + other.y, z + other.z);
		}

		void Normalize() {
			T magnitude = std::sqrt(x * x + y * y + z * z);
			if (magnitude > 0) {
				x /= magnitude;
				y /= magnitude;
				z /= magnitude;
			}
		}

		T distance_to(const Vec3& other) const {
			return std::sqrt((x - other.x) * (x - other.x) +
							 (y - other.y) * (y - other.y) +
							 (z - other.z) * (z - other.z));
		}

		T dot_product(const Vec3& other) const {
			return (x * other.x + y * other.y + z * other.z);
		}

		Vec3 cross_product(const Vec3& other) const {
			return Vec3(
				y * other.z - z * other.y,
				z * other.x - x * other.z,
				x * other.y - y * other.x
			);
		}

		T angle_between(const Vec3& other) const {
			T dot = dot_product(other);
			T firstMag = std::sqrt(x*x + y*y + z*z);
			T secondMag = std::sqrt(other.x * other.x + other.y * other.y + other.z * other.z);
			return (std::acos(dot / firstMag * secondMag));
		}
};