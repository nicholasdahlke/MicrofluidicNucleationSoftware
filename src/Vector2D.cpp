//
// Created by nicholas on 23.02.25.
//
#include <MicrofluidicNucleation/Vector2D.h>
#include <cmath>


mfn::Vector2D::Vector2D(mfn::Vector2D::vector_type _content)
{
    content = _content;
}

double mfn::Vector2D::get_length() const
{
    return sqrt(pow(content[0], 2) + pow(content[1], 2));
}

cv::Point mfn::Vector2D::get_point() const
{
    return {static_cast<int>(content[0]), static_cast<int>(content[1])};
}