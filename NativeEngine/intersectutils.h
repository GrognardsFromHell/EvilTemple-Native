#ifndef INTERSECTUTILS_H
#define INTERSECTUTILS_H

#include <Ogre.h>

struct IntersectResult {
    bool intersects;
    float distance;
};

class IntersectionUtils {
public:

    static IntersectResult intersect(const Ogre::Ray &ray, const Ogre::Vector3 &cylinderBase, float radius, float height);

};

#endif // INTERSECTUTILS_H
