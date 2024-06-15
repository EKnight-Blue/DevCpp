#include "VectorField.h"

VectorField::VectorField(size_t width, size_t height) :
        width{width}, height{height}, vectors(width * height) {

}