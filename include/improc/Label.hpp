#pragma once

#ifndef SIPL_IMPROC_LABEL_H
#define SIPL_IMPROC_LABEL_H

#include "matrix/Matrix"
#include "matrix/Vector"
#include <deque>
#include <vector>

namespace sipl
{

// Different types of connectivity
enum class Connectivity { N4, N8 };

// Defines a component. A collection of these is returned by
// sipl::connected_components
struct Component {
    size_t mass;
    Vector2f center_of_mass;
    std::vector<Vector2i> indices;

    Component() : mass(0), center_of_mass({0.0, 0.0}), indices() {}

    void push_back(const Vector2i& i)
    {
        indices.push_back(i);
        mass++;
    }

    void push_back(Vector2i&& i)
    {
        indices.push_back(i);
        mass++;
    }

    void calculate_center_of_mass()
    {
        // Add this component to the list of components
        auto a = std::accumulate(std::begin(indices), std::end(indices),
                                 Vector2d{0.0, 0.0});
        center_of_mass = a / mass;
    }
};

// Runs a one-pass connected component algorithm over img using connectivity
// conn. img must be a binary image with the background being defined as black
// (0) and blobs as white (max(Dtype)). Does *not* return the background as a
// component
template <typename Dtype>
std::vector<Component> connected_components(const MatrixX<Dtype>& img,
                                            Connectivity conn)
{
    // Get a map of pixels that are unmapped (sets all background pixels to
    // labeled so we skip them)
    auto labeled = img.apply([](auto e) { return e == Dtype(0); });
    std::vector<Component> components;

    for (int32_t i = 0; i < img.dims[0]; ++i) {
        for (int32_t j = 0; j < img.dims[1]; ++j) {

            // Skip any pixels that have already been labeled
            if (labeled(i, j)) {
                continue;
            }

            Component comp;
            switch (conn) {
            case Connectivity::N4: {
                throw std::runtime_error("not implemented");
            }
            case Connectivity::N8: {

                // Fill queue initially with N8 neighbors
                std::deque<Vector2i> nbors;
                for (int32_t r = i - 1; r <= i + 1; ++r) {
                    for (int32_t c = j - 1; c <= j + 1; ++c) {

                        // Skip out-of-bounds or already connected pixels or
                        // center
                        if ((r == i && j == c) || r < 0 || r >= img.dims[0] ||
                            c < 0 || c >= img.dims[1] || labeled(r, c)) {
                            continue;
                        }

                        labeled(r, c) = true;
                        nbors.push_back({r, c});
                    }
                }

                // Go through all neighbors in the component
                while (!nbors.empty()) {
                    auto pixel = nbors.front();
                    nbors.pop_front();
                    comp.push_back(pixel);

                    for (int32_t r = pixel[0] - 1; r <= pixel[0] + 1; ++r) {
                        for (int32_t c = pixel[1] - 1; c <= pixel[1] + 1; ++c) {

                            // Skip out-of-bounds or already connected pixels
                            if ((pixel[0] == r && pixel[1] == c) || r < 0 ||
                                r >= img.dims[0] || c < 0 || c >= img.dims[1] ||
                                labeled(r, c)) {
                                continue;
                            }

                            labeled(r, c) = true;
                            nbors.push_back({r, c});
                        }
                    }
                }

                comp.calculate_center_of_mass();
                components.push_back(comp);
            }
            }
        }
    }

    return components;
}

double total_mass(const std::vector<Component>& cs)
{
    return std::accumulate(std::begin(cs), std::end(cs), 0.0,
                           [](double sum, auto c) { return sum + c.mass; });
}

double average_mass(const std::vector<Component>& cs)
{
    return total_mass(cs) / cs.size();
}
}

#endif
