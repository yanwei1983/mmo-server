#include "GameMath.h"

//-----------------------------------------------------------------------
bool GameMath::Intersection2D::pointInTri2D(const Vector2& p, const Vector2& a, const Vector2& b, const Vector2& c)
{
    // Winding must be consistent from all edges for point to be inside
    Vector2 v1, v2;
    float   dot[3];
    bool    zeroDot[3];

    v1 = b - a;
    v2 = p - a;

    // Note we don't care about normalisation here since sign is all we need
    // It means we don't have to worry about magnitude of cross products either
    dot[0]     = v1.crossProduct(v2);
    zeroDot[0] = Math::FloatEqual(dot[0], 0.0f, 1e-3f);

    v1 = c - b;
    v2 = p - b;

    dot[1]     = v1.crossProduct(v2);
    zeroDot[1] = Math::FloatEqual(dot[1], 0.0f, 1e-3f);

    // Compare signs (ignore colinear / coincident points)
    if(!zeroDot[0] && !zeroDot[1] && Math::Sign(dot[0]) != Math::Sign(dot[1]))
    {
        return false;
    }

    v1 = a - c;
    v2 = p - c;

    dot[2]     = v1.crossProduct(v2);
    zeroDot[2] = Math::FloatEqual(dot[2], 0.0f, 1e-3f);
    // Compare signs (ignore colinear / coincident points)
    if((!zeroDot[0] && !zeroDot[2] && Math::Sign(dot[0]) != Math::Sign(dot[2])) ||
       (!zeroDot[1] && !zeroDot[2] && Math::Sign(dot[1]) != Math::Sign(dot[2])))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------
bool GameMath::Intersection::pointInTri3D(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& normal)
{
    // Winding must be consistent from all edges for point to be inside
    Vector3 v1, v2;
    float   dot[3];
    bool    zeroDot[3];

    v1 = b - a;
    v2 = p - a;

    // Note we don't care about normalisation here since sign is all we need
    // It means we don't have to worry about magnitude of cross products either
    dot[0]     = v1.crossProduct(v2).dotProduct(normal);
    zeroDot[0] = Math::FloatEqual(dot[0], 0.0f, 1e-3f);

    v1 = c - b;
    v2 = p - b;

    dot[1]     = v1.crossProduct(v2).dotProduct(normal);
    zeroDot[1] = Math::FloatEqual(dot[1], 0.0f, 1e-3f);

    // Compare signs (ignore colinear / coincident points)
    if(!zeroDot[0] && !zeroDot[1] && Math::Sign(dot[0]) != Math::Sign(dot[1]))
    {
        return false;
    }

    v1 = a - c;
    v2 = p - c;

    dot[2]     = v1.crossProduct(v2).dotProduct(normal);
    zeroDot[2] = Math::FloatEqual(dot[2], 0.0f, 1e-3f);
    // Compare signs (ignore colinear / coincident points)
    if((!zeroDot[0] && !zeroDot[2] && Math::Sign(dot[0]) != Math::Sign(dot[2])) ||
       (!zeroDot[1] && !zeroDot[2] && Math::Sign(dot[1]) != Math::Sign(dot[2])))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------
std::optional<float> GameMath::Intersection::intersects(const Ray& ray, const AxisAlignedBox& box)
{
    if(box.isNull())
        return {};
    if(box.isInfinite())
        return 0.0f;

    float          lowt = 0.0f;
    float          t;
    bool           hit = false;
    Vector3        hitpoint;
    const Vector3& min     = box.getMinimum();
    const Vector3& max     = box.getMaximum();
    const Vector3& rayorig = ray.getOrigin();
    const Vector3& raydir  = ray.getDirection();

    // Check origin inside first
    if(rayorig > min && rayorig < max)
    {
        return 0.0f;
    }

    // Check each face in turn, only check closest 3
    // Min x
    if(rayorig.x <= min.x && raydir.x > 0)
    {
        t = (min.x - rayorig.x) / raydir.x;

        // Substitute t back into ray and check bounds and dist
        hitpoint = rayorig + raydir * t;
        if(hitpoint.y >= min.y && hitpoint.y <= max.y && hitpoint.z >= min.z && hitpoint.z <= max.z && (!hit || t < lowt))
        {
            hit  = true;
            lowt = t;
        }
    }
    // Max x
    if(rayorig.x >= max.x && raydir.x < 0)
    {
        t = (max.x - rayorig.x) / raydir.x;

        // Substitute t back into ray and check bounds and dist
        hitpoint = rayorig + raydir * t;
        if(hitpoint.y >= min.y && hitpoint.y <= max.y && hitpoint.z >= min.z && hitpoint.z <= max.z && (!hit || t < lowt))
        {
            hit  = true;
            lowt = t;
        }
    }
    // Min y
    if(rayorig.y <= min.y && raydir.y > 0)
    {
        t = (min.y - rayorig.y) / raydir.y;

        // Substitute t back into ray and check bounds and dist
        hitpoint = rayorig + raydir * t;
        if(hitpoint.x >= min.x && hitpoint.x <= max.x && hitpoint.z >= min.z && hitpoint.z <= max.z && (!hit || t < lowt))
        {
            hit  = true;
            lowt = t;
        }
    }
    // Max y
    if(rayorig.y >= max.y && raydir.y < 0)
    {
        t = (max.y - rayorig.y) / raydir.y;

        // Substitute t back into ray and check bounds and dist
        hitpoint = rayorig + raydir * t;
        if(hitpoint.x >= min.x && hitpoint.x <= max.x && hitpoint.z >= min.z && hitpoint.z <= max.z && (!hit || t < lowt))
        {
            hit  = true;
            lowt = t;
        }
    }
    // Min z
    if(rayorig.z <= min.z && raydir.z > 0)
    {
        t = (min.z - rayorig.z) / raydir.z;

        // Substitute t back into ray and check bounds and dist
        hitpoint = rayorig + raydir * t;
        if(hitpoint.x >= min.x && hitpoint.x <= max.x && hitpoint.y >= min.y && hitpoint.y <= max.y && (!hit || t < lowt))
        {
            hit  = true;
            lowt = t;
        }
    }
    // Max z
    if(rayorig.z >= max.z && raydir.z < 0)
    {
        t = (max.z - rayorig.z) / raydir.z;

        // Substitute t back into ray and check bounds and dist
        hitpoint = rayorig + raydir * t;
        if(hitpoint.x >= min.x && hitpoint.x <= max.x && hitpoint.y >= min.y && hitpoint.y <= max.y && (!hit || t < lowt))
        {
            hit  = true;
            lowt = t;
        }
    }

    if(hit)
    {
        return lowt;
    }
    else
    {
        return {};
    }
}
//-----------------------------------------------------------------------
bool GameMath::Intersection::intersects(const Ray& ray, const AxisAlignedBox& box, float* d1, float* d2)
{
    if(box.isNull())
        return false;

    if(box.isInfinite())
    {
        if(d1)
            *d1 = 0;
        if(d2)
            *d2 = Math::POS_INFINITY;
        return true;
    }

    const Vector3& min     = box.getMinimum();
    const Vector3& max     = box.getMaximum();
    const Vector3& rayorig = ray.getOrigin();
    const Vector3& raydir  = ray.getDirection();

    Vector3 absDir;
    absDir[0] = Math::Abs(raydir[0]);
    absDir[1] = Math::Abs(raydir[1]);
    absDir[2] = Math::Abs(raydir[2]);

    // Sort the axis, ensure check minimise floating error axis first
    int32_t imax = 0, imid = 1, imin = 2;
    if(absDir[0] < absDir[2])
    {
        imax = 2;
        imin = 0;
    }
    if(absDir[1] < absDir[imin])
    {
        imid = imin;
        imin = 1;
    }
    else if(absDir[1] > absDir[imax])
    {
        imid = imax;
        imax = 1;
    }

    float start = 0, end = Math::POS_INFINITY;

#define _CALC_AXIS(i)                                   \
    do                                                  \
    {                                                   \
        float denom    = 1 / raydir[i];                 \
        float newstart = (min[i] - rayorig[i]) * denom; \
        float newend   = (max[i] - rayorig[i]) * denom; \
        if(newstart > newend)                           \
            std::swap(newstart, newend);                \
        if(newstart > end || newend < start)            \
            return false;                               \
        if(newstart > start)                            \
            start = newstart;                           \
        if(newend < end)                                \
            end = newend;                               \
    } while(0)

    // Check each axis in turn

    _CALC_AXIS(imax);

    if(absDir[imid] < std::numeric_limits<float>::epsilon())
    {
        // Parallel with middle and minimise axis, check bounds only
        if(rayorig[imid] < min[imid] || rayorig[imid] > max[imid] || rayorig[imin] < min[imin] || rayorig[imin] > max[imin])
            return false;
    }
    else
    {
        _CALC_AXIS(imid);

        if(absDir[imin] < std::numeric_limits<float>::epsilon())
        {
            // Parallel with minimise axis, check bounds only
            if(rayorig[imin] < min[imin] || rayorig[imin] > max[imin])
                return false;
        }
        else
        {
            _CALC_AXIS(imin);
        }
    }
#undef _CALC_AXIS

    if(d1)
        *d1 = start;
    if(d2)
        *d2 = end;

    return true;
}
//-----------------------------------------------------------------------
std::optional<float> GameMath::Intersection::intersects(const Ray&     ray,
                                                        const Vector3& a,
                                                        const Vector3& b,
                                                        const Vector3& c,
                                                        const Vector3& normal,
                                                        bool           positiveSide,
                                                        bool           negativeSide)
{
    //
    // Calculate intersection with plane.
    //
    float t;
    {
        float denom = normal.dotProduct(ray.getDirection());

        // Check intersect side
        if(denom > +std::numeric_limits<float>::epsilon())
        {
            if(!negativeSide)
                return {};
        }
        else if(denom < -std::numeric_limits<float>::epsilon())
        {
            if(!positiveSide)
                return {};
        }
        else
        {
            // Parallel or triangle area is close to zero when
            // the plane normal not normalised.
            return {};
        }

        t = normal.dotProduct(a - ray.getOrigin()) / denom;

        if(t < 0)
        {
            // Intersection is behind origin
            return {};
        }
    }

    //
    // Calculate the largest area projection plane in X, Y or Z.
    //
    size_t i0, i1;
    {
        float n0 = Math::Abs(normal[0]);
        float n1 = Math::Abs(normal[1]);
        float n2 = Math::Abs(normal[2]);

        i0 = 1;
        i1 = 2;
        if(n1 > n2)
        {
            if(n1 > n0)
                i0 = 0;
        }
        else
        {
            if(n2 > n0)
                i1 = 0;
        }
    }

    //
    // Check the intersection point is inside the triangle.
    //
    {
        float u1 = b[i0] - a[i0];
        float v1 = b[i1] - a[i1];
        float u2 = c[i0] - a[i0];
        float v2 = c[i1] - a[i1];
        float u0 = t * ray.getDirection()[i0] + ray.getOrigin()[i0] - a[i0];
        float v0 = t * ray.getDirection()[i1] + ray.getOrigin()[i1] - a[i1];

        float alpha = u0 * v2 - u2 * v0;
        float beta  = u1 * v0 - u0 * v1;
        float area  = u1 * v2 - u2 * v1;

        // epsilon to avoid float precision error
        const float EPSILON = 1e-6f;

        float tolerance = -EPSILON * area;

        if(area > 0)
        {
            if(alpha < tolerance || beta < tolerance || alpha + beta > area - tolerance)
                return {};
        }
        else
        {
            if(alpha > tolerance || beta > tolerance || alpha + beta < area - tolerance)
                return {};
        }
    }

    return t;
}

inline Vector3 calculateBasicFaceNormalWithoutNormalize(const Vector3& v1, const Vector3& v2, const Vector3& v3)
{
    return (v2 - v1).crossProduct(v3 - v1);
}

//-----------------------------------------------------------------------
std::optional<float> GameMath::Intersection::intersects(const Ray&     ray,
                                                        const Vector3& a,
                                                        const Vector3& b,
                                                        const Vector3& c,
                                                        bool           positiveSide,
                                                        bool           negativeSide)
{
    Vector3 normal = calculateBasicFaceNormalWithoutNormalize(a, b, c);
    return intersects(ray, a, b, c, normal, positiveSide, negativeSide);
}