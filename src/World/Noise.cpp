#include "Noise.h"

Toroidal4::Toroidal4(std::unique_ptr<Noise<4>> noise, const float rad1, const float rad2, const float x_period, const float y_period) :
    noise{std::move(noise)},
    rad1{rad1}, rad2{rad2},
    x_period{x_period}, y_period{y_period} {

}

float Toroidal4::at(const Vec<2> &point) const {
    float nx = TWO_PI * point[0] / x_period;
    float ny = TWO_PI * point[1] / y_period;
    Vec<4> n_point{rad1 * cosf(nx), rad2 * cosf(ny), rad1 * sinf(nx), rad2 * sinf(ny)};
    return normalize_simplex4(noise->at(n_point));
}


ToroidalPair::ToroidalPair(std::unique_ptr<Noise<4>> noise, const float rad1, const float rad2, const float x_period, const float y_period, Vec<4> center1, Vec<4> center2) :
    noise{std::move(noise)},
    rad1{rad1}, rad2{rad2},
    x_period{x_period}, y_period{y_period},
    center1{center1}, center2{center2} {

}

Vec<2> ToroidalPair::at(const Vec<2> &point) const {
    float nx = TWO_PI * point[0] / x_period;
    float ny = TWO_PI * point[1] / y_period;
    Vec<4> n_point{rad1 * cosf(nx), rad2 * cosf(ny), rad1 * sinf(nx), rad2 * sinf(ny)};
    return {
        normalize_simplex4(noise->at(n_point + center1)),
        normalize_simplex4(noise->at(n_point + center2))
    };
}