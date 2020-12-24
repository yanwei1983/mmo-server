#ifndef MATHDEF_H
#define MATHDEF_H

#include <array>
#include <cmath>
#include <cstring>
#include <valarray>

#include "RandomGet.h"
#include "export_lua.h"
using Real = float;
class Radian;
class Degree;
class Angle;

/** Wrapper class which indicates a given angle value is in Radians.
    @remarks
        Radian values are interchangeable with Degree values, and conversions
        will be done automatically between them.
*/
class Radian
{
    float mRad;

public:
    export_lua explicit Radian(float r = 0)
        : mRad(r)
    {
    }
    export_lua Radian(const Degree& d);
    export_lua Radian(const Radian& rhs)
        : mRad(rhs.mRad)
    {
    }
    export_lua Radian& operator=(const float& f)
    {
        mRad = f;
        return *this;
    }
    export_lua Radian& operator=(const Radian& r)
    {
        mRad = r.mRad;
        return *this;
    }
    export_lua Radian& operator=(const Degree& d);

    export_lua float valueDegrees() const; // see bottom of this file
    export_lua float valueRadians() const { return mRad; }
    export_lua float valueAngleUnits() const;

    export_lua const Radian& operator+() const { return *this; }
    export_lua Radian        operator+(const Radian& r) const { return Radian(mRad + r.mRad); }
    export_lua Radian        operator+(const Degree& d) const;
    Radian&                  operator+=(const Radian& r)
    {
        mRad += r.mRad;
        return *this;
    }
    Radian&           operator+=(const Degree& d);
    export_lua Radian operator-() const { return Radian(-mRad); }
    export_lua Radian operator-(const Radian& r) const { return Radian(mRad - r.mRad); }
    export_lua Radian operator-(const Degree& d) const;
    Radian&           operator-=(const Radian& r)
    {
        mRad -= r.mRad;
        return *this;
    }
    Radian&           operator-=(const Degree& d);
    export_lua Radian operator*(float f) const { return Radian(mRad * f); }
    export_lua Radian operator*(const Radian& f) const { return Radian(mRad * f.mRad); }
    Radian&           operator*=(float f)
    {
        mRad *= f;
        return *this;
    }
    export_lua Radian operator/(float f) const { return Radian(mRad / f); }
    Radian&           operator/=(float f)
    {
        mRad /= f;
        return *this;
    }

    export_lua bool operator<(const Radian& r) const { return mRad < r.mRad; }
    export_lua bool operator<=(const Radian& r) const { return mRad <= r.mRad; }
    export_lua bool operator==(const Radian& r) const { return mRad == r.mRad; }
    export_lua bool operator!=(const Radian& r) const { return mRad != r.mRad; }
    export_lua bool operator>=(const Radian& r) const { return mRad >= r.mRad; }
    export_lua bool operator>(const Radian& r) const { return mRad > r.mRad; }
};

/** Wrapper class which indicates a given angle value is in Degrees.
@remarks
    Degree values are interchangeable with Radian values, and conversions
    will be done automatically between them.
*/
export_lua class Degree
{
    float mDeg; // if you get an error here - make sure to define/typedef 'float' first

public:
    export_lua explicit Degree(float d = 0)
        : mDeg(d)
    {
    }
    export_lua Degree(const Radian& r)
        : mDeg(r.valueDegrees())
    {
    }
    export_lua Degree(const Degree& rhs)
        : mDeg(rhs.mDeg)
    {
    }
    export_lua Degree& operator=(const float& f)
    {
        mDeg = f;
        return *this;
    }
    export_lua Degree& operator=(const Degree& d)
    {
        mDeg = d.mDeg;
        return *this;
    }
    export_lua Degree& operator=(const Radian& r)
    {
        mDeg = r.valueDegrees();
        return *this;
    }

    export_lua float valueDegrees() const { return mDeg; }
    export_lua float valueRadians() const; // see bottom of this file
    export_lua float valueAngleUnits() const;

    export_lua const Degree& operator+() const { return *this; }
    export_lua Degree        operator+(const Degree& d) const { return Degree(mDeg + d.mDeg); }
    export_lua Degree        operator+(const Radian& r) const { return Degree(mDeg + r.valueDegrees()); }
    Degree&                  operator+=(const Degree& d)
    {
        mDeg += d.mDeg;
        return *this;
    }
    Degree& operator+=(const Radian& r)
    {
        mDeg += r.valueDegrees();
        return *this;
    }
    export_lua Degree operator-() const { return Degree(-mDeg); }
    export_lua Degree operator-(const Degree& d) const { return Degree(mDeg - d.mDeg); }
    export_lua Degree operator-(const Radian& r) const { return Degree(mDeg - r.valueDegrees()); }
    Degree&           operator-=(const Degree& d)
    {
        mDeg -= d.mDeg;
        return *this;
    }
    Degree& operator-=(const Radian& r)
    {
        mDeg -= r.valueDegrees();
        return *this;
    }
    export_lua Degree operator*(float f) const { return Degree(mDeg * f); }
    export_lua Degree operator*(const Degree& f) const { return Degree(mDeg * f.mDeg); }
    Degree&           operator*=(float f)
    {
        mDeg *= f;
        return *this;
    }
    export_lua Degree operator/(float f) const { return Degree(mDeg / f); }
    Degree&           operator/=(float f)
    {
        mDeg /= f;
        return *this;
    }

    export_lua bool operator<(const Degree& d) const { return mDeg < d.mDeg; }
    export_lua bool operator<=(const Degree& d) const { return mDeg <= d.mDeg; }
    export_lua bool operator==(const Degree& d) const { return mDeg == d.mDeg; }
    export_lua bool operator!=(const Degree& d) const { return mDeg != d.mDeg; }
    export_lua bool operator>=(const Degree& d) const { return mDeg >= d.mDeg; }
    export_lua bool operator>(const Degree& d) const { return mDeg > d.mDeg; }
};

/** Wrapper class which identifies a value as the currently default angle
    type, as defined by Math::setAngleUnit.
@remarks
    Angle values will be automatically converted between radians and degrees,
    as appropriate.
*/
export_lua class Angle
{
    float mAngle;

public:
    explicit Angle(float angle)
        : mAngle(angle)
    {
    }
    operator Radian() const;
    operator Degree() const;
};

// these functions could not be defined within the class definition of class
// Radian because they required class Degree to be defined
inline Radian::Radian(const Degree& d)
    : mRad(d.valueRadians())
{
}
inline Radian& Radian::operator=(const Degree& d)
{
    mRad = d.valueRadians();
    return *this;
}
inline Radian Radian::operator+(const Degree& d) const
{
    return Radian(mRad + d.valueRadians());
}
inline Radian& Radian::operator+=(const Degree& d)
{
    mRad += d.valueRadians();
    return *this;
}
inline Radian Radian::operator-(const Degree& d) const
{
    return Radian(mRad - d.valueRadians());
}
inline Radian& Radian::operator-=(const Degree& d)
{
    mRad -= d.valueRadians();
    return *this;
}

export_lua class Math
{
public:
    enum AngleUnit
    {
        AU_DEGREE,
        AU_RADIAN,
    };
    export_lua static constexpr float PI           = 3.1415926f;
    export_lua static constexpr float TWO_PI       = 2.0f * PI;
    export_lua static constexpr float HALF_PI      = 0.5f * PI;
    static constexpr float            POS_INFINITY = std::numeric_limits<float>::infinity();
    static constexpr float            NEG_INFINITY = -std::numeric_limits<float>::infinity();
    static constexpr float            fDeg2Rad     = PI / 180.0f;
    static constexpr float            fRad2Deg     = 180.0f / PI;
    static constexpr AngleUnit        msAngleUnit  = AU_DEGREE;

    export_lua static inline int32_t IAbs(int32_t iValue) { return (iValue >= 0 ? iValue : -iValue); }
    export_lua static inline int32_t ICeil(float fValue) { return int32_t(std::ceil(fValue)); }
    export_lua static inline int32_t IFloor(float fValue) { return int32_t(std::floor(fValue)); }
    export_lua static inline int32_t ISign(int32_t iValue) { return (iValue > 0 ? +1 : (iValue < 0 ? -1 : 0)); }

    export_lua static inline float  Abs(float fValue) { return std::abs(fValue); }
    export_lua static inline Degree Abs(const Degree& dValue) { return Degree(std::abs(dValue.valueDegrees())); }
    export_lua static inline Radian Abs(const Radian& rValue) { return Radian(std::abs(rValue.valueRadians())); }

    export_lua static inline Radian ACos(float fValue)
    {
        if(-1.0 < fValue)
        {
            if(fValue < 1.0)
                return Radian(std::acos(fValue));
            else
                return Radian(0.0);
        }
        else
        {
            return Radian(PI);
        }
    }
    export_lua static inline Radian ASin(float fValue)
    {
        if(-1.0 < fValue)
        {
            if(fValue < 1.0)
                return Radian(std::asin(fValue));
            else
                return Radian(HALF_PI);
        }
        else
        {
            return Radian(-HALF_PI);
        }
    }
    export_lua static inline Radian ATan(float fValue) { return Radian(std::atan(fValue)); }
    export_lua static inline Radian ATan2(float fY, float fX) { return Radian(std::atan2(fY, fX)); }
    export_lua static inline float  Ceil(float fValue) { return std::ceil(fValue); }

    export_lua static inline bool isNaN(float f)
    {
        // std::isnan() is C99, not supported by all compilers
        // However NaN always fails this next test, no other number does.
        return f != f;
    }

    export_lua static inline bool is2Pow(uint32_t target) { return (!(target & (target - 1))); }

    export_lua static inline int32_t get2Pow(uint32_t target)
    {
        int32_t result = -1;
        if(is2Pow(target) == true)
        {
            while(target != 0)
            {
                target >>= 1;
                result++;
            }
        }
        return result;

        // return log10(target) / log10(2);
    }

    export_lua static inline uint32_t nextPow2(uint32_t v)
    {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;
        return v;
    }

    export_lua static inline uint32_t ilog2(uint32_t v)
    {
        uint32_t r;
        uint32_t shift;
        r = (v > 0xffff) << 4;
        v >>= r;
        shift = (v > 0xff) << 3;
        v >>= shift;
        r |= shift;
        shift = (v > 0xf) << 2;
        v >>= shift;
        r |= shift;
        shift = (v > 0x3) << 1;
        v >>= shift;
        r |= shift;
        r |= (v >> 1);
        return r;
    }

    export_lua static inline bool FloatEqual(float a, float b, float tolerance = std::numeric_limits<float>::epsilon())
    {
        if(std::fabs(b - a) <= tolerance)
            return true;
        else
            return false;
    }

    export_lua static inline float Cos(const Radian& fValue, bool useTables = false)
    {
        return (!useTables) ? std::cos(fValue.valueRadians()) : TRIG_TABLE().SinTable(fValue.valueRadians() + HALF_PI);
    }
    export_lua static inline float Cos(float fValue, bool useTables = false)
    {
        return (!useTables) ? std::cos(fValue) : TRIG_TABLE().SinTable(fValue + HALF_PI);
    }

    export_lua static inline float Exp(float fValue) { return std::exp(fValue); }

    /** Floor function
    Returns the largest previous integer. (example: Floor(1.9) = 1)

    @param fValue
        The value to round down to the nearest integer.
    */
    export_lua static inline float Floor(float fValue) { return std::floor(fValue); }
    export_lua static inline float Log(float fValue) { return std::log(fValue); }
    export_lua static inline float Log2(float fValue)
    {
        static const float LOG2 = std::log(2.0f);
        return std::log(fValue) / LOG2;
    }
    export_lua static inline float LogN(float base, float fValue) { return std::log(fValue) / std::log(base); }
    export_lua static inline float Pow(float fBase, float fExponent) { return std::pow(fBase, fExponent); }
    export_lua static float        Sign(float fValue)
    {
        if(fValue > 0.0)
            return 1.0;
        if(fValue < 0.0)
            return -1.0;
        return 0.0;
    }

    export_lua static inline Radian Sign(const Radian& rValue) { return Radian(Sign(rValue.valueRadians())); }
    export_lua static inline Degree Sign(const Degree& dValue) { return Degree(Sign(dValue.valueDegrees())); }

    /// Simulate the shader function saturate that clamps a parameter value between 0 and 1
    export_lua static inline float  saturate(float t) { return (t < 0) ? 0 : ((t > 1) ? 1 : t); }
    export_lua static inline double saturate(double t) { return (t < 0) ? 0 : ((t > 1) ? 1 : t); }

    /** Simulate the shader function lerp which performers linear interpolation
         given 3 parameters v0, v1 and t the function returns the value of (1 - t)* v0 + t * v1.
        where v0 and v1 are matching vector or scalar types and t can be either a scalar or a
        vector of the same type as a and b.
    */
    template<typename V, typename T>
    static V lerp(const V& v0, const V& v1, const T& t)
    {
        return v0 * (1 - t) + v1 * t;
    }

    export_lua static inline float Sin(const Radian& fValue, bool useTables = false)
    {
        return (!useTables) ? std::sin(fValue.valueRadians()) : TRIG_TABLE().SinTable(fValue.valueRadians());
    }
    export_lua static inline float Sin(float fValue, bool useTables = false)
    {
        return (!useTables) ? std::sin(fValue) : TRIG_TABLE().SinTable(fValue);
    }

    export_lua static inline float  Sqr(float fValue) { return fValue * fValue; }
    export_lua static inline float  Sqrt(float fValue) { return std::sqrt(fValue); }
    export_lua static inline Radian Sqrt(const Radian& fValue) { return Radian(std::sqrt(fValue.valueRadians())); }
    export_lua static inline Degree Sqrt(const Degree& fValue) { return Degree(std::sqrt(fValue.valueDegrees())); }

    /** Inverse square root i.e. 1 / Sqrt(x), good for vector normalisation. */
    export_lua static float InvSqrt(float fValue) { return float(1.) / std::sqrt(fValue); }

    /** Clamp a value within an inclusive range. */
    template<typename T>
    static T Clamp(T val, T minval, T maxval)
    {
        assert(minval < maxval && "Invalid clamp range");
        return std::max<T>(std::min<T>(val, maxval), minval);
    }

    export_lua static inline float Tan(const Radian& fValue, bool useTables = false)
    {
        return (!useTables) ? std::tan(fValue.valueRadians()) : TRIG_TABLE().TanTable(fValue.valueRadians());
    }

    export_lua static inline float Tan(float fValue, bool useTables = false)
    {
        return (!useTables) ? std::tan(fValue) : TRIG_TABLE().TanTable(fValue);
    }

    export_lua static inline float DegreesToRadians(float degrees) { return degrees * fDeg2Rad; }
    export_lua static inline float RadiansToDegrees(float radians) { return radians * fRad2Deg; }

    //-----------------------------------------------------------------------
    export_lua static inline Math::AngleUnit getAngleUnit(void) { return msAngleUnit; }
    //-----------------------------------------------------------------------
    export_lua static inline float AngleUnitsToRadians(float angleunits)
    {
        if(msAngleUnit == AU_DEGREE)
            return angleunits * fDeg2Rad;
        else
            return angleunits;
    }

    //-----------------------------------------------------------------------
    export_lua static inline float RadiansToAngleUnits(float radians)
    {
        if(msAngleUnit == AU_DEGREE)
            return radians * fRad2Deg;
        else
            return radians;
    }

    //-----------------------------------------------------------------------
    export_lua static inline float AngleUnitsToDegrees(float angleunits)
    {
        if(msAngleUnit == AU_RADIAN)
            return angleunits * fRad2Deg;
        else
            return angleunits;
    }

    //-----------------------------------------------------------------------
    export_lua static inline float DegreesToAngleUnits(float degrees)
    {
        if(msAngleUnit == AU_RADIAN)
            return degrees * fDeg2Rad;
        else
            return degrees;
    }

    template<size_t nTrigTableSize>
    class CTrigTable
    {
    public:
        CTrigTable()
        {
            for(int32_t i = 0; i < nTrigTableSize; ++i)
            {
                float angle   = TWO_PI * i / nTrigTableSize;
                m_SinTable[i] = std::sin(angle);
                m_TanTable[i] = std::tan(angle);
            }
        }

        float SinTable(float fValue) const
        {
            int32_t idx;
            if(fValue >= 0)
            {
                idx = int32_t(fValue * m_TrigTableFactor) % nTrigTableSize;
            }
            else
            {
                idx = nTrigTableSize - (int32_t(-fValue * m_TrigTableFactor) % nTrigTableSize) - 1;
            }

            return m_SinTable[idx];
        }

        float TanTable(float fValue) const
        {
            // Convert range to index values, wrap if required
            int32_t idx = int32_t(fValue *= m_TrigTableFactor) % nTrigTableSize;
            return m_TanTable[idx];
        }

    private:
        static constexpr float m_TrigTableFactor = nTrigTableSize / TWO_PI;
        std::array<float, nTrigTableSize> m_SinTable        = {};
        std::array<float, nTrigTableSize> m_TanTable        = {};
    };
    using TRIG_TABLE_t = CTrigTable<4096>;
    static inline const TRIG_TABLE_t& TRIG_TABLE()
    {
        static const TRIG_TABLE_t s_TRIG_TABLE;
        return s_TRIG_TABLE;
    }
};

// these functions must be defined down here, because they rely on the
// angle unit conversion functions in class Math:

inline float Radian::valueDegrees() const
{
    return Math::RadiansToDegrees(mRad);
}

inline float Radian::valueAngleUnits() const
{
    return Math::RadiansToAngleUnits(mRad);
}

inline float Degree::valueRadians() const
{
    return Math::DegreesToRadians(mDeg);
}

inline float Degree::valueAngleUnits() const
{
    return Math::DegreesToAngleUnits(mDeg);
}

inline Angle::operator Radian() const
{
    return Radian(Math::AngleUnitsToRadians(mAngle));
}

inline Angle::operator Degree() const
{
    return Degree(Math::AngleUnitsToDegrees(mAngle));
}

inline Radian operator*(float a, const Radian& b)
{
    return Radian(a * b.valueRadians());
}

inline Radian operator/(float a, const Radian& b)
{
    return Radian(a / b.valueRadians());
}

inline Degree operator*(float a, const Degree& b)
{
    return Degree(a * b.valueDegrees());
}

inline Degree operator/(float a, const Degree& b)
{
    return Degree(a / b.valueDegrees());
}

#endif /* MATHDEF_H */
