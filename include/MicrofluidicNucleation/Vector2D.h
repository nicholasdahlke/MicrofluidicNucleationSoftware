//
// Created by nicholas on 23.02.25.
//

#ifndef MICROFLUIDICNUCLEATIONSOFTWARE_VECTOR2D_H
#define MICROFLUIDICNUCLEATIONSOFTWARE_VECTOR2D_H

#include <array>

namespace mfn
{
    class Vector2D
    {
    private:
        typedef std::array<double, 2> vector_type;

    public:
        vector_type content;

        explicit Vector2D(vector_type _content);
        double get_length();


    };
}


#endif //MICROFLUIDICNUCLEATIONSOFTWARE_VECTOR2D_H
