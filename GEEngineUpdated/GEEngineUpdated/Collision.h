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
};