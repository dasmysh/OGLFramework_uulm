/**
 * @file   TransferFunction.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.08.24
 *
 * @brief  Definition of the transfer function class.
 */

#include "TransferFunction.h"
#include <algorithm>

namespace cgu {
    namespace tf {
        // Comparison operator for sorting ControlPoints
        struct {
            bool operator()(const ControlPoint& a, const ControlPoint& b) const
            {
                return a.val < b.val;
            }
        } PointLess; // Note the pun! Hilarious!

        // Inserts a new Control Point
        void TransferFunction::InsertControlPoint(const ControlPoint& p)
        {
            points_.push_back(p);
            std::sort(points_.begin(), points_.end(), PointLess);
        }

        // Removes an existing Control Point at index i
        void TransferFunction::RemoveControlPoint(int i)
        {
            assert(i < points_.size());
            points_.erase(points_.begin() + i);
        }

        // Sets the position for a point with a particular index
        // Returns the new index for the point after sorting
        int TransferFunction::SetPosition(int i, const glm::vec2& pos)
        {
            points_[i].SetPos(pos);
            auto p = points_[i];

            std::sort(points_.begin(), points_.end(), PointLess);

            for (auto u = 0; u < static_cast<int>(points_.size()); ++u)
                if (points_[u] == p)
                    return u;

            return i;
        }

        void TransferFunction::SetColor(int i, const glm::vec4& color)
        {
            points_[i].rgba = color;
        }

        // Returns the interpolated value for the transfer function
        glm::vec4 TransferFunction::RGBA(float val) const
        {
            // Clamp that sucker
            val = glm::clamp(val, 0.f, 1.f);

            // If we have no Control Points, just return a ramp
            if (points_.size() == 0)
                return glm::vec4(1.f, 1.f, 1.f, val);

            // if the value is less than the first or greater than the last
            // then we just return the value of those.
            if (val <= points_.front().val)
                return points_.front().rgba;
            if (val >= points_.back().val)
                return points_.back().rgba;

            // Find the corresponding index
            size_t i = 0;
            while (val > points_[i].val)
                ++i;

            // Prev and Next points
            const auto p = points_[i - 1];
            const auto n = points_[i];

            // Linear interpolation value t
            auto t = (val - p.val) / (n.val - p.val);

            // Return interpolated values
            return glm::mix(p.rgba, n.rgba, t);
        }

        // Generates interpolated texture data with a specified resolution
        void TransferFunction::CreateTextureData(glm::vec4* data, int resolution) const
        {
            assert(resolution > 0);
            assert(data);

            for (auto i = 0; i < resolution; ++i) {
                auto x = static_cast<float>(i) / static_cast<float>(resolution - 1);
                data[i] = RGBA(x);
            }
        }
    }
}