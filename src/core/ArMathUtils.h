#ifndef ARMATH_UTILS_H
#define ARMATH_UTILS_H

#include "ArAurora.h"

#include "glm/common.hpp"
#include "glm/gtc/quaternion.hpp"

#include <ctime>
#include <iterator>

namespace Aurora
{
	typedef unsigned int uint;

	template <typename T>
	inline bool isNaN(const T x) { return std::isnan(x); }

	template <>
	inline bool isNaN(const int x) { return false; }

#define rndm 0x100000000LL
#define rndc 0xB16
#define rnda 0x5DEECE66DLL
	static unsigned long long seed = 1;
	inline Float drand48(void)
	{
		seed = (rnda * seed + rndc) & 0xFFFFFFFFFFFFLL;
		unsigned int x = seed >> 16;
		return  ((Float)x / (Float)rndm);
	}

	inline void srand48(unsigned int i)
	{
		seed = (((long long int)i) << 16) | rand();
	}

	inline Float radians(Float angle)
	{
		return angle * aPi / 180.0f;
	}

	inline Float angles(Float radians)
	{
		return radians * 180.0f / aPi;
	}

	inline bool equal(Float a, Float b)
	{
		return fabs(a - b) < aMachineEpsilon;
	}

	//-------------------------------------------Vector/Point/Normal-------------------------------------

	template<typename T>
	using AVector2 = glm::vec<2, T>;

	template<typename T>
	using AVector3 = glm::vec<3, T>;

	typedef AVector2<Float> AVector2f;
	typedef AVector2<int> AVector2i;
	typedef AVector3<Float> AVector3f;
	typedef AVector3<int> AVector3i;

	template <typename T>
	inline std::ostream &operator<<(std::ostream &os, const AVector2<T> &v)
	{
		os << "[ " << v.x << ", " << v.y << " ]";
		return os;
	}

	template <typename T>
	inline std::ostream &operator<<(std::ostream &os, const AVector3<T> &v)
	{
		os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
		return os;
	}


	//-------------------------------------------Matrix/Quaterion-------------------------------------

	using AMatrix4x4 = glm::mat<4, 4, Float>;
	using AQuaterion = glm::qua<Float>;

	inline AMatrix4x4 transpose(const AMatrix4x4 &m) { return glm::transpose(m); }
	inline AMatrix4x4 mul(const AMatrix4x4 &m1, const AMatrix4x4 &m2) { return m1 * m2; }
	inline AMatrix4x4 inverse(const AMatrix4x4 &m) { return glm::inverse(m); }

	inline AMatrix4x4 toMatrix4x4(const AQuaterion &q) { return glm::mat4_cast(q); }

	//-------------------------------------------ABounds2/ABounds3-------------------------------------

	template<typename T>
	class ABounds2
	{
	public:

		ABounds2()
		{
			T minNum = std::numeric_limits<T>::lowest();
			T maxNum = std::numeric_limits<T>::max();
			m_pMin = AVector2<T>(maxNum, maxNum);
			m_pMax = AVector2<T>(minNum, minNum);
		}

		explicit ABounds2(const AVector2<T> &p) : m_pMin(p), m_pMax(p) {}

		ABounds2(const AVector2<T> &p1, const AVector2<T> &p2)
		{
			m_pMin = AVector2<T>(glm::min(p1.x, p2.x), glm::min(p1.y, p2.y));
			m_pMax = AVector2<T>(glm::max(p1.x, p2.x), glm::max(p1.y, p2.y));
		}

		template <typename U>
		explicit operator ABounds2<U>() const
		{
			return ABounds2<U>((AVector2<U>)m_pMin, (AVector2<U>)m_pMax);
		}

		AVector2<T> diagonal() const { return m_pMax - m_pMin; }

		T area() const
		{
			AVector2<T> d = m_pMax - m_pMin;
			return (d.x * d.y);
		}

		int maximumExtent() const {
			AVector2<T> diag = diagonal();
			if (diag.x > diag.y)
				return 0;
			else
				return 1;
		}

		inline AVector2<T> &operator[](int i) { return (i == 0) ? m_pMin : m_pMax; }
		inline const AVector2<T> &operator[](int i) const { return (i == 0) ? m_pMin : m_pMax; }

		bool operator==(const ABounds2<T> &b) const { return b.m_pMin == pMin && b.m_pMax == pMax; }
		bool operator!=(const ABounds2<T> &b) const { return b.m_pMin != pMin || b.m_pMax != pMax; }

		AVector2<T> lerp(const AVector2f &t) const
		{
			return AVector2<T>(Aurora::lerp(t.x, m_pMin.x, m_pMax.x), Aurora::lerp(t.y, m_pMin.y, m_pMax.y));
		}

		AVector2<T> offset(const AVector2<T> &p) const
		{
			AVector2<T> o = p - m_pMin;
			if (m_pMax.x > m_pMin.x) o.x /= m_pMax.x - m_pMin.x;
			if (m_pMax.y > m_pMin.y) o.y /= m_pMax.y - m_pMin.y;
			return o;
		}

		void boundingSphere(AVector2<T> *c, Float *rad) const
		{
			*c = (m_pMin + m_pMax) / 2;
			*rad = Inside(*c, *this) ? Distance(*c, m_pMax) : 0;
		}

		friend std::ostream &operator<<(std::ostream &os, const ABounds2<T> &b)
		{
			os << "[ " << b.m_pMin << " - " << b.m_pMax << " ]";
			return os;
		}

	public:
		AVector2<T> m_pMin, m_pMax;

	};

	class ARay;
	template<typename T>
	class ABounds3
	{
	public:

		ABounds3()
		{
			T minNum = std::numeric_limits<T>::lowest();
			T maxNum = std::numeric_limits<T>::max();
			m_pMin = AVector3<T>(maxNum, maxNum, maxNum);
			m_pMax = AVector3<T>(minNum, minNum, minNum);
		}

		explicit ABounds3(const AVector3<T> &p) : m_pMin(p), m_pMax(p) {}

		ABounds3(const AVector3<T> &p1, const AVector3<T> &p2) :
			m_pMin(glm::min(p1.x, p2.x), glm::min(p1.y, p2.y), glm::min(p1.z, p2.z)),
			m_pMax(glm::max(p1.x, p2.x), glm::max(p1.y, p2.y), glm::max(p1.z, p2.z)) {}

		AVector3<T> &operator[](int i) { return (i == 0) ? m_pMin : m_pMax; }
		const AVector3<T> &operator[](int i) const { return (i == 0) ? m_pMin : m_pMax; }

		bool operator==(const ABounds3<T> &b) const { return b.m_pMin == m_pMin && b.m_pMax == m_pMax; }
		bool operator!=(const ABounds3<T> &b) const { return b.m_pMin != m_pMin || b.m_pMax != m_pMax; }

		AVector3<T> corner(int corner) const
		{
			return AVector3<T>((*this)[(corner & 1)].x, (*this)[(corner & 2) ? 1 : 0].y, (*this)[(corner & 4) ? 1 : 0].z);
		}

		AVector3<T> diagonal() const { return m_pMax - m_pMin; }

		T surfaceArea() const
		{
			AVector3<T> d = diagonal();
			return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
		}

		T volume() const
		{
			AVector3<T> d = diagonal();
			return d.x * d.y * d.z;
		}

		int maximumExtent() const
		{
			AVector3<T> d = diagonal();
			if (d.x > d.y && d.x > d.z)
				return 0;
			else if (d.y > d.z)
				return 1;
			else
				return 2;
		}

		AVector3<T> lerp(const AVector3f &t) const
		{
			return AVector3<T>(
				Aurora::lerp(t.x, m_pMin.x, m_pMax.x),
				Aurora::lerp(t.y, m_pMin.y, m_pMax.y),
				Aurora::lerp(t.z, m_pMin.z, m_pMax.z));
		}

		AVector3<T> offset(const AVector3<T> &p) const
		{
			AVector3<T> o = p - m_pMin;
			if (m_pMax.x > m_pMin.x) o.x /= m_pMax.x - m_pMin.x;
			if (m_pMax.y > m_pMin.y) o.y /= m_pMax.y - m_pMin.y;
			if (m_pMax.z > m_pMin.z) o.z /= m_pMax.z - m_pMin.z;
			return o;
		}

		void boundingSphere(AVector3<T> *center, Float *radius) const
		{
			*center = (m_pMin + m_pMax) / 2;
			*radius = Inside(*center, *this) ? Distance(*center, m_pMax) : 0;
		}

		template <typename U>
		explicit operator ABounds3<U>() const
		{
			return ABounds3<U>((AVector3<U>)m_pMin, (AVector3<U>)m_pMax);
		}

		bool hit(const ARay &ray, Float &hitt0, Float &hitt1) const;
		inline bool hit(const ARay &ray, const AVector3f &invDir, const int dirIsNeg[3]) const;

		friend std::ostream &operator<<(std::ostream &os, const ABounds3<T> &b)
		{
			os << "[ " << b.m_pMin << " - " << b.m_pMax << " ]";
			return os;
		}

	public:
		AVector3<T> m_pMin, m_pMax;
	};

	typedef ABounds2<Float> ABounds2f;
	typedef ABounds2<int> ABounds2i;
	typedef ABounds3<Float> ABounds3f;
	typedef ABounds3<int> ABounds3i;

	class ABounds2iIterator : public std::forward_iterator_tag 
	{
	public:
		ABounds2iIterator(const ABounds2i &b, const AVector2i &pt)
			: p(pt), bounds(&b) {}

		ABounds2iIterator operator++() 
		{
			advance();
			return *this;
		}

		ABounds2iIterator operator++(int) 
		{
			ABounds2iIterator old = *this;
			advance();
			return old;
		}

		bool operator==(const ABounds2iIterator &bi) const 
		{
			return p == bi.p && bounds == bi.bounds;
		}

		bool operator!=(const ABounds2iIterator &bi) const 
		{
			return p != bi.p || bounds != bi.bounds;
		}

		AVector2i operator*() const { return p; }

	private:
		void advance() 
		{
			++p.x;
			if (p.x == bounds->m_pMax.x) 
			{
				p.x = bounds->m_pMin.x;
				++p.y;
			}
		}

		AVector2i p;
		const ABounds2i *bounds;
	};

	inline ABounds2iIterator begin(const ABounds2i &b) 
	{
		return ABounds2iIterator(b, b.m_pMin);
	}

	inline ABounds2iIterator end(const ABounds2i &b) 
	{
		// Normally, the ending point is at the minimum x value and one past
		// the last valid y value.
		AVector2i pEnd(b.m_pMin.x, b.m_pMax.y);
		// However, if the bounds are degenerate, override the end point to
		// equal the start point so that any attempt to iterate over the bounds
		// exits out immediately.
		if (b.m_pMin.x >= b.m_pMax.x || b.m_pMin.y >= b.m_pMax.y)
			pEnd = b.m_pMin;
		return ABounds2iIterator(b, pEnd);
	}

	//-------------------------------------------ARay-------------------------------------

	class ARay
	{
	public:

		ARay() : m_tMax(aInfinity) {}

		ARay(const AVector3f &o, const AVector3f &d, Float tMax = aInfinity)
			: m_origin(o), m_dir(normalize(d)), m_tMax(tMax) {}

		const AVector3f &origin() const { return m_origin; }
		const AVector3f &direction() const { return m_dir; }

		AVector3f operator()(Float t) const { return m_origin + m_dir * t; }

		friend std::ostream &operator<<(std::ostream &os, const ARay &r)
		{
			os << "[o=" << r.m_origin << ", d=" << r.m_dir << ", tMax=" << r.m_tMax << "]";
			return os;
		}

	public:
		AVector3f m_origin;
		AVector3f m_dir;
		mutable Float m_tMax;
	};

	//-------------------------------------------Defnition-------------------------------------

	template <typename T>
	inline Float dot(const AVector2<T> &v1, const AVector2<T> &v2) { return glm::dot(v1, v2); }

	template <typename T>
	inline Float absDot(const AVector2<T> &v1, const AVector2<T> &v2) { return glm::abs(dot(v1, v2)); }

	template <typename T>
	inline AVector2<T> normalize(const AVector2<T> &v) { return glm::normalize(v); }

	template <typename T>
	AVector2<T> abs(const AVector2<T> &v) { return glm::abs(v); }

	template <typename T>
	inline Float length(const AVector3<T> &p) { return glm::length(p); }

	template <typename T>
	inline Float lengthSquared(const AVector3<T> &p) { return glm::dot(p, p); }

	template <typename T>
	inline Float distance(const AVector3<T> &p1, const AVector3<T> &p2) { return glm::length(p1 - p2); }

	template <typename T>
	inline Float distanceSquared(const AVector3<T> &p1, const AVector3<T> &p2) { return glm::dot(p1 - p2, p1 - p2); }

	template <typename T>
	inline AVector3<T> lerp(Float t, const AVector3<T> &p0, const AVector3<T> &p1) { return (1 - t) * p0 + t * p1; }

	template <typename T>
	inline AVector3<T> min(const AVector3<T> &p1, const AVector3<T> &p2)
	{
		return AVector3<T>(glm::min(p1.x, p2.x), glm::min(p1.y, p2.y), glm::min(p1.z, p2.z));
	}

	template <typename T>
	inline AVector3<T> max(const AVector3<T> &p1, const AVector3<T> &p2)
	{
		return AVector3<T>(glm::max(p1.x, p2.x), glm::max(p1.y, p2.y), glm::max(p1.z, p2.z));
	}

	template <typename T>
	inline AVector3<T> floor(const AVector3<T> &p)
	{
		return AVector3<T>(glm::floor(p.x), glm::floor(p.y), glm::floor(p.z));
	}

	template <typename T>
	inline AVector3<T> ceil(const AVector3<T> &p)
	{
		return AVector3<T>(glm::ceil(p.x), glm::ceil(p.y), glm::ceil(p.z));
	}

	template <typename T>
	inline AVector3<T> abs(const AVector3<T> &p)
	{
		return AVector3<T>(glm::abs(p.x), glm::abs(p.y), glm::abs(p.z));
	}

	template <typename T>
	inline Float distance(const AVector2<T> &p1, const AVector2<T> &p2) { return glm::length(p1, p2); }

	template <typename T>
	inline Float distanceSquared(const AVector2<T> &p1, const AVector2<T> &p2) { return glm::dot(p1 - p2, p1 - p2); }

	template <typename T>
	inline AVector2<T> floor(const AVector2<T> &p) { return AVector2<T>(glm::floor(p.x), glm::floor(p.y)); }

	template <typename T>
	inline AVector2<T> ceil(const AVector2<T> &p) { return AVector2<T>(glm::ceil(p.x), glm::ceil(p.y)); }

	template <typename T>
	inline AVector2<T> lerp(Float t, const AVector2<T> &v0, const AVector2<T> &v1) { return (1 - t) * v0 + t * v1; }

	template <typename T>
	inline AVector2<T> min(const AVector2<T> &pa, const AVector2<T> &pb) { return AVector2<T>(glm::min(pa.x, pb.x), glm::min(pa.y, pb.y)); }

	template <typename T>
	inline AVector2<T> max(const AVector2<T> &pa, const AVector2<T> &pb) { return AVector2<T>(glm::max(pa.x, pb.x), glm::max(pa.y, pb.y)); }

	template <typename T>
	inline T dot(const AVector3<T> &n1, const AVector3<T> &v2) { return glm::dot(n1, v2); }

	template <typename T>
	inline T absDot(const AVector3<T> &n1, const AVector3<T> &v2) { return glm::abs(glm::dot(n1, v2)); }

	template <typename T>
	inline AVector3<T> cross(const AVector3<T> &v1, const AVector3<T> &v2) { return glm::cross(v1, v2); }

	template <typename T>
	inline AVector3<T> normalize(const AVector3<T> &v) { return glm::normalize(v); }

	template <typename T>
	inline T minComponent(const AVector3<T> &v) { return glm::min(v.x, glm::min(v.y, v.z)); }

	template <typename T>
	inline T maxComponent(const AVector3<T> &v) { return glm::max(v.x, glm::max(v.y, v.z)); }

	template <typename T>
	inline int maxDimension(const AVector3<T> &v) { return (v.x > v.y) ? ((v.x > v.z) ? 0 : 2) : ((v.y > v.z) ? 1 : 2); }

	template <typename T>
	inline AVector3<T> permute(const AVector3<T> &v, int x, int y, int z) { return AVector3<T>(v[x], v[y], v[z]); }

	template <typename T>
	inline AVector3<T> faceforward(const AVector3<T> &n, const AVector3<T> &v) { return (dot(n, v) < 0.f) ? -n : n; }

	template <typename T>
	inline void coordinateSystem(const AVector3<T> &v1, AVector3<T> &v2, AVector3<T> &v3)
	{
		if (glm::abs(v1.x) > glm::abs(v1.y))
			v2 = AVector3<T>(-v1.z, 0, v1.x) / glm::sqrt(v1.x * v1.x + v1.z * v1.z);
		else
			v2 = AVector3<T>(0, v1.z, -v1.y) / glm::sqrt(v1.y * v1.y + v1.z * v1.z);
		v3 = cross(v1, v2);
	}

	inline AVector3f sphericalDirection(Float sinTheta, Float cosTheta, Float phi)
	{
		return AVector3f(sinTheta * glm::cos(phi), sinTheta * glm::sin(phi), cosTheta);
	}

	inline AVector3f sphericalDirection(Float sinTheta, Float cosTheta, Float phi,
		const AVector3f &x, const AVector3f &y, const AVector3f &z)
	{
		return sinTheta * glm::cos(phi) * x + sinTheta * glm::sin(phi) * y + cosTheta * z;
	}

	template <typename T>
	inline ABounds3<T> unionBounds(const ABounds3<T> &b, const AVector3<T> &p)
	{
		ABounds3<T> ret;
		ret.m_pMin = min(b.m_pMin, p);
		ret.m_pMax = max(b.m_pMax, p);
		return ret;
	}

	template <typename T>
	inline ABounds3<T> unionBounds(const ABounds3<T> &b1, const ABounds3<T> &b2)
	{
		ABounds3<T> ret;
		ret.m_pMin = min(b1.m_pMin, b2.m_pMin);
		ret.m_pMax = max(b1.m_pMax, b2.m_pMax);
		return ret;
	}

	template <typename T>
	inline ABounds3<T> intersect(const ABounds3<T> &b1, const ABounds3<T> &b2)
	{
		// Important: assign to pMin/pMax directly and don't run the ABounds2()
		// constructor, since it takes min/max of the points passed to it.  In
		// turn, that breaks returning an invalid bound for the case where we
		// intersect non-overlapping bounds (as we'd like to happen).
		ABounds3<T> ret;
		ret.m_pMin = max(b1.m_pMin, b2.m_pMin);
		ret.m_pMax = Min(b1.m_pMax, b2.m_pMax);
		return ret;
	}

	template <typename T>
	inline bool overlaps(const ABounds3<T> &b1, const ABounds3<T> &b2)
	{
		bool x = (b1.m_pMax.x >= b2.m_pMin.x) && (b1.m_pMin.x <= b2.m_pMax.x);
		bool y = (b1.m_pMax.y >= b2.m_pMin.y) && (b1.m_pMin.y <= b2.m_pMax.y);
		bool z = (b1.m_pMax.z >= b2.m_pMin.z) && (b1.m_pMin.z <= b2.m_pMax.z);
		return (x && y && z);
	}

	template <typename T>
	inline bool inside(const AVector3<T> &p, const ABounds3<T> &b)
	{
		return (p.x >= b.m_pMin.x && p.x <= b.m_pMax.x && p.y >= b.m_pMin.y &&
			p.y <= b.m_pMax.y && p.z >= b.m_pMin.z && p.z <= b.m_pMax.z);
	}

	template <typename T>
	inline ABounds2<T> unionBounds(const ABounds2<T> &b, const AVector2<T> &p)
	{
		ABounds2<T> ret;
		ret.m_pMin = min(b.m_pMin, p);
		ret.m_pMax = max(b.m_pMax, p);
		return ret;
	}

	template <typename T>
	inline ABounds2<T> unionBounds(const ABounds2<T> &b, const ABounds2<T> &b2)
	{
		ABounds2<T> ret;
		ret.m_pMin = min(b.m_pMin, b2.m_pMin);
		ret.m_pMax = max(b.m_pMax, b2.m_pMax);
		return ret;
	}

	template <typename T>
	inline ABounds2<T> intersect(const ABounds2<T> &b1, const ABounds2<T> &b2)
	{
		// Important: assign to pMin/pMax directly and don't run the Bounds2()
		// constructor, since it takes min/max of the points passed to it.  In
		// turn, that breaks returning an invalid bound for the case where we
		// intersect non-overlapping bounds (as we'd like to happen).
		ABounds2<T> ret;
		ret.m_pMin = max(b1.m_pMin, b2.m_pMin);
		ret.m_pMax = min(b1.m_pMax, b2.m_pMax);
		return ret;
	}

	template <typename T>
	inline bool overlaps(const ABounds2<T> &ba, const ABounds2<T> &bb)
	{
		bool x = (ba.m_pMax.x >= bb.m_pMin.x) && (ba.m_pMin.x <= bb.m_pMax.x);
		bool y = (ba.m_pMax.y >= bb.m_pMin.y) && (ba.m_pMin.y <= bb.m_pMax.y);
		return (x && y);
	}

	template <typename T>
	inline bool inside(const AVector2<T> &pt, const ABounds2<T> &b)
	{
		return (pt.x >= b.m_pMin.x && pt.x <= b.m_pMax.x && pt.y >= b.m_pMin.y && pt.y <= b.m_pMax.y);
	}

	template <typename T>
	bool insideExclusive(const AVector2<T> &pt, const ABounds2<T> &b) 
	{
		return (pt.x >= b.m_pMin.x && pt.x < b.m_pMax.x && pt.y >= b.m_pMin.y && pt.y < b.m_pMax.y);
	}

	template <typename T>
	inline bool ABounds3<T>::hit(const ARay &ray, Float &hitt0, Float &hitt1) const
	{
		Float t0 = 0, t1 = ray.m_tMax;
		for (int i = 0; i < 3; ++i)
		{
			// Update interval for _i_th bounding box slab
			Float invRayDir = 1 / ray.m_dir[i];
			Float tNear = (m_pMin[i] - ray.m_origin[i]) * invRayDir;
			Float tFar = (m_pMax[i] - ray.m_origin[i]) * invRayDir;

			// Update parametric interval from slab intersection $t$ values
			if (tNear > tFar)
				std::swap(tNear, tFar);

			// Update _tFar_ to ensure robust ray--bounds intersection
			tFar *= 1 + 2 * gamma(3);
			t0 = tNear > t0 ? tNear : t0;
			t1 = tFar < t1 ? tFar : t1;
			if (t0 > t1)
				return false;
		}
		hitt0 = t0;
		hitt1 = t1;
		return true;
	}

	template <typename T>
	inline bool ABounds3<T>::hit(const ARay &ray, const AVector3f &invDir, const int dirIsNeg[3]) const
	{
		const ABounds3f &bounds = *this;
		// Check for ray intersection against $x$ and $y$ slabs
		Float tMin = (bounds[dirIsNeg[0]].x - ray.m_origin.x) * invDir.x;
		Float tMax = (bounds[1 - dirIsNeg[0]].x - ray.m_origin.x) * invDir.x;
		Float tyMin = (bounds[dirIsNeg[1]].y - ray.m_origin.y) * invDir.y;
		Float tyMax = (bounds[1 - dirIsNeg[1]].y - ray.m_origin.y) * invDir.y;

		// Update _tMax_ and _tyMax_ to ensure robust bounds intersection
		tMax *= 1 + 2 * gamma(3);
		tyMax *= 1 + 2 * gamma(3);
		if (tMin > tyMax || tyMin > tMax)
			return false;
		if (tyMin > tMin)
			tMin = tyMin;
		if (tyMax < tMax)
			tMax = tyMax;

		// Check for ray intersection against $z$ slab
		Float tzMin = (bounds[dirIsNeg[2]].z - ray.m_origin.z) * invDir.z;
		Float tzMax = (bounds[1 - dirIsNeg[2]].z - ray.om_origin.z) * invDir.z;

		// Update _tzMax_ to ensure robust bounds intersection
		tzMax *= 1 + 2 * gamma(3);
		if (tMin > tzMax || tzMin > tMax)
			return false;
		if (tzMin > tMin)
			tMin = tzMin;
		if (tzMax < tMax)
			tMax = tzMax;
		return (tMin < ray.m_tMax) && (tMax > 0);
	}

	template <typename Predicate>
	int findInterval(int size, const Predicate &pred) 
	{
		int first = 0, len = size;
		while (len > 0) 
		{
			int half = len >> 1, middle = first + half;
			// Bisect range based on value of _pred_ at _middle_
			if (pred(middle)) 
			{
				first = middle + 1;
				len -= half + 1;
			}
			else
			{
				len = half;
			}
		}
		return clamp(first - 1, 0, size - 2);
	}

	inline AVector3f reflect(const AVector3f &wo, const AVector3f &n) { return -wo + 2 * dot(wo, n) * n; }

	inline bool refract(const AVector3f &wi, const AVector3f &n, Float eta, AVector3f &wt)
	{
		// Compute $\cos \theta_\roman{t}$ using Snell's law
		Float cosThetaI = dot(n, wi);
		Float sin2ThetaI = glm::max(Float(0), Float(1 - cosThetaI * cosThetaI));
		Float sin2ThetaT = eta * eta * sin2ThetaI;

		// Handle total internal reflection for transmission
		if (sin2ThetaT >= 1) 
			return false;
		Float cosThetaT = glm::sqrt(1 - sin2ThetaT);
		wt = eta * -wi + (eta * cosThetaI - cosThetaT) * AVector3f(n);
		return true;
	}

	inline AVector3f randomInUnitSphere()
	{
		AVector3f pos;
		do
		{
			pos = AVector3f(drand48(), drand48(), drand48()) * 2.0f - AVector3f(1.0, 1.0, 1.0);
		} while (lengthSquared(pos) >= 1.0);
		return pos;
	}

	inline AVector3f randomToSphere(float radius, float distance_squared)
	{
		float r1 = drand48();
		float r2 = drand48();
		float z = 1 + r2 * (glm::sqrt(1 - radius * radius / distance_squared) - 1);
		float phi = 2 * aPi * r1;
		float x = glm::cos(phi) * glm::sqrt(1 - z * z);
		float y = glm::sin(phi) * glm::sqrt(1 - z * z);
		return AVector3f(x, y, z);
	}

	inline AVector3f randomCosineDir()
	{
		float r1 = drand48();
		float r2 = drand48();
		float z = glm::sqrt(1 - r2);
		float phi = 2 * aPi * r1;
		float x = glm::cos(phi) * 2 * glm::sqrt(r2);
		float y = glm::sin(phi) * 2 * glm::sqrt(r2);
		return AVector3f(x, y, z);
	}
}

#endif