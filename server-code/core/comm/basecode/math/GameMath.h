#ifndef __GAMEMATH_H_
#define __GAMEMATH_H_

#include <functional>
#include <vector>

#include <assert.h>

#include "AxisAlignedBox.h"
#include "MathDef.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "RandomGet.h"
#include "Ray.h"
#include "Vector2.h"
#include "Vector3.h"
#include "export_lua.h"

export_lua struct Rect
{
    export_lua long left   = 0;
    export_lua long top    = 0;
    export_lua long right  = 0;
    export_lua long bottom = 0;

    export_lua Rect() {}
    export_lua Rect(long l, long t, long r, long b)
        : left(l)
        , top(t)
        , right(r)
        , bottom(b)
    {
    }
    export_lua Rect(Rect const& o)
        : left(o.left)
        , top(o.top)
        , right(o.right)
        , bottom(o.bottom)
    {
    }
    export_lua Rect& operator=(Rect const& o)
    {
        left   = o.left;
        top    = o.top;
        right  = o.right;
        bottom = o.bottom;
        return *this;
    }
    export_lua long        width() const { return right - left; }
    export_lua long        height() const { return bottom - top; }
    export_lua inline bool isIntersect(const Vector2& pos, float vDis)
    {
        return (left - vDis <= pos.x && pos.x <= right + vDis && top - vDis <= pos.y && pos.y <= bottom + vDis);
    }
};

export_lua struct FloatRect
{
    export_lua float left   = 0.0f;
    export_lua float top    = 0.0f;
    export_lua float right  = 0.0f;
    export_lua float bottom = 0.0f;
    export_lua       FloatRect() {}
    export_lua       FloatRect(float l, float t, float r, float b)
        : left(l)
        , top(t)
        , right(r)
        , bottom(b)
    {
    }
    export_lua FloatRect(const FloatRect& o)
        : left(o.left)
        , top(o.top)
        , right(o.right)
        , bottom(o.bottom)
    {
    }
    FloatRect& operator=(const FloatRect& o)
    {
        left   = o.left;
        top    = o.top;
        right  = o.right;
        bottom = o.bottom;
        return *this;
    }
    export_lua float width() const { return right - left; }
    export_lua float height() const { return bottom - top; }

    export_lua inline bool isIntersect(const Vector2& pos, float vDis)
    {
        return (left - vDis <= pos.x && pos.x <= right + vDis && top - vDis <= pos.y && pos.y <= bottom + vDis);
    }
};

export_lua namespace GameMath
{
    export_lua inline bool isIntersect(const FloatRect& rect, const Vector2& pos, float vDis)
    {
        return (rect.left - vDis <= pos.x && pos.x <= rect.right + vDis && rect.top - vDis <= pos.y && pos.y <= rect.bottom + vDis);
    }

    export_lua inline float simpleDistance(const Vector2& src, const Vector2& dest)
    {
        float x = src.x - dest.x;
        float y = src.y - dest.y;
        return x * x + y * y;
    }

    export_lua inline float distance(const Vector2& src, const Vector2& dest) { return std::sqrt(simpleDistance(src, dest)); }

    export_lua inline float distance2D(const Vector3& src, const Vector3& dest) { return distance(Vector2(src.x, src.z), Vector2(dest.x, dest.z)); }

    export_lua inline float manhattanDistance(const Vector2& src, const Vector2& dest)
    {
        return std::fabs(src.x - dest.x) + std::fabs(src.y - dest.y);
    }

    export_lua inline Vector2 random_vector2(float _min, float _max) { return Vector2::UNIT_X().randomDeviant(1.0f) * random_float(_min, _max); }

    export_lua inline Vector2 rotateByRadian(const Vector2& vec, float radian)
    {
        float cosa = std::cos(radian);
        float sina = std::sin(radian);
        return Vector2(cosa * vec.x - sina * vec.y, sina * vec.x + cosa * vec.y);
    }

    export_lua inline Vector2 rotate(const Vector2& vec, float angle)
    {
        float radian = angle / 180.0f * Math::PI;
        return rotateByRadian(vec, radian);
    }

    export_lua const Vector2 VECTOR2_NORTH = Vector2(0, -1);
    export_lua const Vector2 VECTOR2_SOUTH = VECTOR2_NORTH * -1;
    export_lua const Vector2 VECTOR2_EAST  = VECTOR2_NORTH.perpendicular();
    export_lua const Vector2 VECTOR2_WEST  = VECTOR2_SOUTH.perpendicular();

    export_lua inline float getRadianFrom(const Vector2& vec, const Vector2& Dir = GameMath::VECTOR2_NORTH)
    {
        if(vec.isNaN() || vec == Vector2::ZERO())
            return 0.0f;
        return std::acos(vec.dotProduct(Dir));
    }
    export_lua inline float getAngleFrom(const Vector2& vec, const Vector2& Dir = GameMath::VECTOR2_NORTH)
    {
        if(vec.isNaN() || vec == Vector2::ZERO())
            return 0.0f;
        return std::acos(vec.dotProduct(Dir)) / Math::PI * 180.0f;
    }

    export_lua inline float getDirectAngleFrom(const Vector2& vec, const Vector2& Dir = GameMath::VECTOR2_NORTH, bool clockwise = true)
    {
        if(vec.isNaN() || vec == Vector2::ZERO())
            return 0.0f;
        float angle = getAngleFrom(vec, Dir);
        if(clockwise == true)
            return (Dir.crossProduct(vec) > 0) ? angle : -angle;
        else
            return (vec.crossProduct(Dir) > 0) ? angle : -angle;
    }

    export_lua inline float getDirectRadianFrom(const Vector2& vec, const Vector2& Dir = GameMath::VECTOR2_NORTH, bool clockwise = true)
    {
        if(vec.isNaN() || vec == Vector2::ZERO())
            return 0.0f;
        float radian = getRadianFrom(vec, Dir);
        if(clockwise == true)
            return (Dir.crossProduct(vec) > 0) ? radian : -radian;
        else
            return (vec.crossProduct(Dir) > 0) ? radian : -radian;
    }

    export_lua inline void correctRect(Rect & rect, long width, long height)
    {
        if(rect.left < 0)
            rect.left = 0;
        else if(rect.left > width)
            rect.left = width;

        if(rect.right < 0)
            rect.right = 0;
        else if(rect.right > width)
            rect.right = width;

        if(rect.top < 0)
            rect.top = 0;
        else if(rect.top > height)
            rect.top = height;

        if(rect.bottom < 0)
            rect.bottom = 0;
        else if(rect.bottom > height)
            rect.bottom = height;

        if(rect.left > rect.right)
            rect.left = rect.right;

        if(rect.right < rect.left)
            rect.right = rect.left;

        if(rect.top > rect.bottom)
            rect.top = rect.bottom;

        if(rect.bottom < rect.top)
            rect.bottom = rect.top;
    }

    //////////////////////////////////////////////////////////////////////
    export_lua inline void DDALineForeach(int32_t x0, int32_t y0, int32_t x1, int32_t y1, std::function<bool(int32_t, int32_t)> func)
    {
        if(x0 == x1 && y0 == y1)
            return;

        int32_t dx     = x1 - x0;
        int32_t dy     = y1 - y0;
        int32_t abs_dx = abs(dx);
        int32_t abs_dy = abs(dy);
        if(abs_dx > abs_dy)
        {
            int32_t _0_5        = abs_dx * (dy > 0 ? 1 : -1);
            int32_t numerator   = dy * 2;
            int32_t denominator = abs_dx * 2;
            // x 增益
            if(dx > 0)
            {
                // x0 ++
                for(int32_t i = 1; i <= abs_dx; i++)
                {
                    if(func(x0 + i, y0 + ((numerator * i + _0_5) / denominator)) == false)
                        return;
                }
            }
            else if(dx < 0)
            {
                // x0 --
                for(int32_t i = 1; i <= abs_dx; i++)
                {
                    if(func(x0 - i, y0 + ((numerator * i + _0_5) / denominator)) == false)
                        return;
                }
            }
        }
        else
        {
            int32_t _0_5        = abs_dy * (dx > 0 ? 1 : -1);
            int32_t numerator   = dx * 2;
            int32_t denominator = abs_dy * 2;
            // y 增益
            if(dy > 0)
            {
                // y0 ++
                for(int32_t i = 1; i <= abs_dy; i++)
                {
                    if(func(x0 + ((numerator * i + _0_5) / denominator), y0 + i) == false)
                        return;
                }
            }
            else if(dy < 0)
            {
                // y0 --
                for(int32_t i = 1; i <= abs_dy; i++)
                {
                    if(func(x0 + ((numerator * i + _0_5) / denominator), y0 - i) == false)
                        return;
                }
            }
        }
    }

    export_lua inline void DDALineEx(int32_t x0, int32_t y0, int32_t x1, int32_t y1, std::vector<CPos2D> & setPos)
    {
        setPos.clear();
        if(x0 == x1 && y0 == y1)
            return;

        setPos.push_back(CPos2D(x0, y0)); // 先把初始点加入

        DDALineForeach(x0, y0, x1, y1, [&setPos](int32_t x, int32_t y) -> bool {
            setPos.push_back(CPos2D(x, y));
            return true;
        });
    }
    //相交判定
    export_lua namespace Intersection2D
    {
        /** Checks whether a given point is inside a triangle, in a
            2-dimensional (Cartesian) space.
            @remarks
                The vertices of the triangle must be given in either
                trigonometrical (anticlockwise) or inverse trigonometrical
                (clockwise) order.
            @param p
                The point.
            @param a
                The triangle's first vertex.
            @param b
                The triangle's second vertex.
            @param c
                The triangle's third vertex.
            @return
                If the point resides in the triangle, <b>true</b> is
                returned.
            @par
                If the point is outside the triangle, <b>false</b> is
                returned.
        */
        static bool pointInTri2D(const Vector2& p, const Vector2& a, const Vector2& b, const Vector2& c);

        export_lua inline bool isInFOV(const Vector2& posFirst, const Vector2& facingFirst, const Vector2& posSecond, float fov)
        {
            Vector2 toTarget = Vector2(posSecond - posFirst).normalisedCopy();

            float dp = facingFirst.dotProduct(toTarget);
            float cs = std::cos(fov * Math::PI);

            // return dp >= cs;
            return (dp > cs) || (Math::FloatEqual(dp, cs));
        }

        export_lua inline bool isInABBox(const Vector2& posFirst,
                                         const Vector2& facingFirst,
                                         const Vector2& posSecond,
                                         const Vector2& facingNormal,
                                         float          halfWidth,
                                         float          height)
        {
            Vector2 toTarget = posSecond - posFirst;
            float   dp       = facingFirst.dotProduct(toTarget);
            if(dp > height || dp < 0.0f)
                return false;

            float dp2 = facingNormal.dotProduct(toTarget);
            if(std::fabs(dp2) > halfWidth)
                return false;

            return true;
        }

        //--------------------LineIntersection2D-------------------------
        //
        //	Given 2 lines in 2D space AB, CD this returns true if an
        //	intersection occurs.
        //
        //-----------------------------------------------------------------

        export_lua inline bool lineIntersection2D(const Vector2& A, const Vector2& B, const Vector2& C, const Vector2& D)
        {
            float rTop = (A.y - C.y) * (D.x - C.x) - (A.x - C.x) * (D.y - C.y);
            float sTop = (A.y - C.y) * (B.x - A.x) - (A.x - C.x) * (B.y - A.y);

            float Bot = (B.x - A.x) * (D.y - C.y) - (B.y - A.y) * (D.x - C.x);

            if(Bot == 0.0f) // parallel
            {
                return false;
            }

            float invBot = 1.0f / Bot;
            float r      = rTop * invBot;
            float s      = sTop * invBot;

            if((r > 0.0f) && (r < 1.0f) && (s > 0.0f) && (s < 1.0f))
            {
                // lines intersect
                return true;
            }

            // lines do not intersect
            return false;
        }

        //-------------------- LineIntersection2D-------------------------
        //
        //	Given 2 lines in 2D space AB, CD this returns true if an
        //	intersection occurs and sets dist to the distance the intersection
        //  occurs along AB. Also sets the 2d vector point to the point of
        //  intersection
        //-----------------------------------------------------------------
        export_lua inline bool LineIntersection2D(const Vector2& A, const Vector2& B, const Vector2& C, const Vector2& D, float& dist, Vector2& point)
        {

            float rTop = (A.y - C.y) * (D.x - C.x) - (A.x - C.x) * (D.y - C.y);
            float rBot = (B.x - A.x) * (D.y - C.y) - (B.y - A.y) * (D.x - C.x);

            float sTop = (A.y - C.y) * (B.x - A.x) - (A.x - C.x) * (B.y - A.y);
            float sBot = (B.x - A.x) * (D.y - C.y) - (B.y - A.y) * (D.x - C.x);

            if((rBot == 0) || (sBot == 0))
            {
                // lines are parallel
                return false;
            }

            float r = rTop / rBot;
            float s = sTop / sBot;

            if((r > 0) && (r < 1) && (s > 0) && (s < 1))
            {
                dist = distance(A, B) * r;

                point = A + r * (B - A);

                return true;
            }

            else
            {
                dist = 0;

                return false;
            }
        }

        export_lua inline Vector2 getNearestPoint(const Vector2& pt1, const Vector2& pt2, const Vector2& testPoint)
        {
            Vector2 A = testPoint - pt1;
            Vector2 u = (pt2 - pt1).normalisedCopy();

            return pt1 + (A.dotProduct(u)) * u;
        }
    }

    export_lua namespace Intersection
    {
        /** Checks whether a given 3D point is inside a triangle.
        @remarks
             The vertices of the triangle must be given in either
             trigonometrical (anticlockwise) or inverse trigonometrical
             (clockwise) order, and the point must be guaranteed to be in the
             same plane as the triangle
         @param p
             p The point.
         @param a
             The triangle's first vertex.
         @param b
             The triangle's second vertex.
         @param c
             The triangle's third vertex.
         @param normal
             The triangle plane's normal (passed in rather than calculated
             on demand since the caller may already have it)
         @return
             If the point resides in the triangle, <b>true</b> is
             returned.
         @par
             If the point is outside the triangle, <b>false</b> is
             returned.
         */
        static bool pointInTri3D(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& normal);
        /** Ray / box intersection */
        static std::optional<float> intersects(const Ray& ray, const AxisAlignedBox& box);

        /** Ray / box intersection, returns boolean result and two intersection distance.
        @param ray
            The ray.
        @param box
            The box.
        @param d1
            A real pointer to retrieve the near intersection distance
            from the ray origin, maybe <b>null</b> which means don't care
            about the near intersection distance.
        @param d2
            A real pointer to retrieve the far intersection distance
            from the ray origin, maybe <b>null</b> which means don't care
            about the far intersection distance.
        @return
            If the ray is intersects the box, <b>true</b> is returned, and
            the near intersection distance is return by <i>d1</i>, the
            far intersection distance is return by <i>d2</i>. Guarantee
            <b>0</b> <= <i>d1</i> <= <i>d2</i>.
        @par
            If the ray isn't intersects the box, <b>false</b> is returned, and
            <i>d1</i> and <i>d2</i> is unmodified.
        */
        static bool intersects(const Ray& ray, const AxisAlignedBox& box, float* d1, float* d2);

        /** Ray / triangle intersection, returns boolean result and distance.
        @param ray
            The ray.
        @param a
            The triangle's first vertex.
        @param b
            The triangle's second vertex.
        @param c
            The triangle's third vertex.
        @param normal
            The triangle plane's normal (passed in rather than calculated
            on demand since the caller may already have it), doesn't need
            normalised since we don't care.
        @param positiveSide
            Intersect with "positive side" of the triangle
        @param negativeSide
            Intersect with "negative side" of the triangle
        */
        static std::optional<float> intersects(const Ray&     ray,
                                               const Vector3& a,
                                               const Vector3& b,
                                               const Vector3& c,
                                               const Vector3& normal,
                                               bool           positiveSide = true,
                                               bool           negativeSide = true);

        /** Ray / triangle intersection, returns boolean result and distance.
        @param ray
            The ray.
        @param a
            The triangle's first vertex.
        @param b
            The triangle's second vertex.
        @param c
            The triangle's third vertex.
        @param positiveSide
            Intersect with "positive side" of the triangle
        @param negativeSide
            Intersect with "negative side" of the triangle
        */
        static std::optional<float> intersects(const Ray&     ray,
                                               const Vector3& a,
                                               const Vector3& b,
                                               const Vector3& c,
                                               bool           positiveSide = true,
                                               bool           negativeSide = true);
    }
};

#endif //__GAMEMATH_H_2007_2_7