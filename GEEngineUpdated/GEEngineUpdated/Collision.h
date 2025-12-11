#pragma once
#include "Maths.h"
#include <algorithm> // For min/max
#include <cmath>

struct BoundingBox {
    Vec3 min;
    Vec3 max;

    BoundingBox() {
        float huge = 1e20f;
        min = Vec3(huge, huge, huge);
        max = Vec3(-huge, -huge, -huge);
    }

    // Helper to calculate center
    Vec3 getCenter() const {
        return (min + max) * 0.5f;
    }

    // Helper to set size at a position
    void set(const Vec3& pos, const Vec3& halfSize) {
        min = pos - halfSize;
        max = pos + halfSize;
    }

    // Extend bounds to include a point
    void extend(const Vec3& p) {
        if (p.x < min.x) min.x = p.x;
        if (p.y < min.y) min.y = p.y;
        if (p.z < min.z) min.z = p.z;

        if (p.x > max.x) max.x = p.x;
        if (p.y > max.y) max.y = p.y;
        if (p.z > max.z) max.z = p.z;
    }
};

struct Ray {
    Vec3 origin;
    Vec3 direction;
    Vec3 invDirection;

    Ray(Vec3 o, Vec3 d) {
        origin = o;
        direction = d;
        invDirection = Vec3(1.0f / d.x, 1.0f / d.y, 1.0f / d.z);
    }
};

class Collision {
public:
    // Collision Detection
    static bool CheckBoundingBox(const BoundingBox& a, const BoundingBox& b, Vec3& resolution) {
        // Calculate Penetration Depth (PD) on all axes
        float pdX = min(a.max.x, b.max.x) - max(a.min.x, b.min.x);
        float pdY = min(a.max.y, b.max.y) - max(a.min.y, b.min.y);
        float pdZ = min(a.max.z, b.max.z) - max(a.min.z, b.min.z);

        // Collision only if ALL PDs are > 0
        if (pdX > 0 && pdY > 0 && pdZ > 0) {

            // Collision Normal is axis with minimal penetration, we find the smallest overlap and push out that way
            resolution = Vec3(0, 0, 0);

            if (pdX < pdY && pdX < pdZ) {
                // X is smallest overlap
                if (a.getCenter().x < b.getCenter().x) resolution.x = -pdX;
                else resolution.x = pdX;
            }
            else if (pdY < pdX && pdY < pdZ) {
                // Y is smallest overlap
                if (a.getCenter().y < b.getCenter().y) resolution.y = -pdY;
                else resolution.y = pdY;
            }
            else {
                // Z is smallest overlap
                if (a.getCenter().z < b.getCenter().z) resolution.z = -pdZ;
                else resolution.z = pdZ;
            }

            return true;
        }
        return false;
    }

    static bool CheckRay(const Ray& r, const BoundingBox& box, float& t) {
        // Calculate intersection t-values for X planes
        float t1 = (box.min.x - r.origin.x) * r.invDirection.x;
        float t2 = (box.max.x - r.origin.x) * r.invDirection.x;

        // Ensure tmin is the near plane, tmax is the far plane
        float tMin = min(t1, t2);
        float tMax = max(t1, t2);

        // Calculate intersection for Y planes
        t1 = (box.min.y - r.origin.y) * r.invDirection.y;
        t2 = (box.max.y - r.origin.y) * r.invDirection.y;

        // Narrow the search window
        tMin = max(tMin, min(t1, t2));
        tMax = min(tMax, max(t1, t2));

        // Calculate intersection for Z planes
        t1 = (box.min.z - r.origin.z) * r.invDirection.z;
        t2 = (box.max.z - r.origin.z) * r.invDirection.z;

        // Final window adjustment
        tMin = max(tMin, min(t1, t2));
        tMax = min(tMax, max(t1, t2));

        // [Slide 21] Collision exists if max >= min and the hit is in front of us
        if (tMax >= tMin && tMax > 0) {
            t = tMin; // Return distance to impact
            return true;
        }

        return false;
    }
};