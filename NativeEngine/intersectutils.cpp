
#include <QtCore/QtGlobal>

#include "intersectutils.h"

using namespace Ogre;

bool IntersectSegmentCylinder(const Vector3 &sa, Vector3 n, const Vector3 &p, float r, float height, float &t);

IntersectResult IntersectionUtils::intersect(const Ray &ray, const Vector3 &cylinderBase, float radius, float height)
{
    IntersectResult result;

    result.intersects = IntersectSegmentCylinder(ray.getOrigin(),
                                          ray.getDirection(),
                                          cylinderBase,
                                          radius,
                                          height,
                                          result.distance);

    return result;
}

static const float EPSILON = 0.00001f; // We're working with pixel values, integer arithmetic would alomst be enough

// Intersect segment S(t)=sa+t(sb-sa), 0<=t<=1 against cylinder specified by p, q and r
bool IntersectSegmentCylinder(const Vector3 &sa, Vector3 n, const Vector3 &p, float r, float height, float &t)
{
    Vector3 d(0, height, 0); // Directional vector from cylinder's base to cap
    n *= 10000;
    Vector3 m = sa - p;
    float md = m.dotProduct(d);
    float nd = n.dotProduct(d);
    float dd = d.dotProduct(d);
    // Test if segment fully outside either endcap of cylinder
    if (md < 0.0f && md + nd < 0.0f) return false; // Segment outside ’p’ side of cylinder
    if (md > dd && md + nd > dd) return false; // Segment outside ’q’ side of cylinder
    float nn = n.dotProduct(n);
    float mn = m.dotProduct(n);
    float a  = dd * nn - nd * nd;
    float k = m.dotProduct(m) - r * r;
    float c = dd * k - md * md;
    if (qAbs(a) < EPSILON) {
        // Segment runs parallel to cylinder axis
        if (c > 0.0f) return false; // ’a’ and thus the segment lie outside cylinder
        // Now known that segment intersects cylinder; figure out how it intersects
        if (md < 0.0f) t = -mn / nn; // Intersect segment against ’p’ endcap
        else if (md > dd) t = (nd - mn) / nn; // Intersect segment against ’q’ endcap
        else t = 0.0f; // ’a’ lies inside cylinder
        return true;
    }
    float b = dd * mn - nd * md;
    float discr = b * b - a * c;
    if (discr < 0.0f) return false; // No real roots; no intersection

    t = (-1 * b - sqrt(discr)) / a;
    if (t < 0.0f || t > 1.0f) return false; // Intersection lies outside segment
    if (md + t * nd < 0.0f) {
        // Intersection outside cylinder on ’p’ side
        if (nd <= 0.0f) return false; // Segment pointing away from endcap
        t = -md / nd;
        // Keep intersection if Dot(S(t) - p, S(t) - p) <= r?2
        return k + 2 * t * (mn + t * nn) <= 0.0f;
    } else if (md + t * nd > dd) {
        // Intersection outside cylinder on ’q’ side
        if (nd >= 0.0f) return false; // Segment pointing away from endcap
        t = (dd - md) / nd;
        // Keep intersection if Dot(S(t) - q, S(t) - q) <= r?2
        return k + dd - 2 * md + t * (2 * (mn - nd) + t * nn) <= 0.0f;
    }
    // Segment intersects cylinder between the endcaps; t is correct
    return true;
}
