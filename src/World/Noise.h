#ifndef DEV_CPP_NOISE_H
#define DEV_CPP_NOISE_H

#include <array>
#include <cmath>
#include <random>
#include <memory>
#include "utils.h"


template<size_t dim>
using Vec = std::array<float, dim>;

template<size_t dim>
Vec<dim> operator+(Vec<dim> const& v1, Vec<dim> const& v2) {
    Vec<dim> result;
    size_t index{0};
    while (index < dim) {
        result[index] = v1[index] + v2[index];
        ++index;
    }
    return result;
}

template<size_t dim>
Vec<dim> operator*(Vec<dim> const& v1, Vec<dim> const& v2) {
    Vec<dim> result;
    size_t index{0};
    while (index < dim) {
        result[index] = v1[index] * v2[index];
        ++index;
    }
    return result;
}

template<size_t dim>
Vec<dim> operator*(Vec<dim> const& v, float f) {
    Vec<dim> result;
    size_t index{0};
    while (index < dim) {
        result[index] = v[index] * f;
        ++index;
    }
    return result;
}


template<size_t dim>
Vec<dim> operator*(float f, Vec<dim> const& v) {
    return v * f;
}

template<size_t dim>
Vec<dim> operator-(Vec<dim> const& v1, Vec<dim> const& v2) {
    Vec<dim> result;
    size_t index{0};
    while (index < dim) {
        result[index] = v1[index] - v2[index];
        ++index;
    }
    return result;
}

template<size_t dim>
float dot(Vec<dim> const& v1, Vec<dim> const& v2) {
    float result{0};
    size_t index{0};
    while (index < dim) {
        result += v1[index] * v2[index];
        ++index;
    }
    return result;
}

template<size_t dim>
Vec<dim> floor(Vec<dim> const& v) {
    Vec<dim> result;
    size_t index{0};
    while (index < dim) {
        result[index++] = static_cast<float>(static_cast<int>(v[index]) - (v[index] < 0.f));
    }
    return result;
}


template<size_t dim>
float sq_mag(Vec<dim> const& v) {
    float result{0.};
    size_t index{0};
    while (index < dim) {
        result += v[index] * v[index];
        ++index;
    }
    return result;
}


template <size_t dim>
class Noise {
public:
    [[nodiscard]] virtual float at(Vec<dim> const& point) const = 0;
    virtual ~Noise() = default;
};


template<size_t dim, typename hash_table_members, size_t hash_table_size>
class SimplexNoise : public Noise<dim> {
public:
    SimplexNoise() {
        for (size_t index{0}; index < hash_table_size; ++index) {
            hash_table[index] = index;
        }
        // shuffling a hash table
        for (size_t index{0}; index < hash_table_size-1; ++index) {
            size_t exchange{random_size() % (hash_table_size - index) + index};
            if (exchange == index)
                continue;
            // a      // b
            hash_table[index] ^= hash_table[exchange];
            // a ^ b  // b
            hash_table[exchange] ^= hash_table[index];
            // a ^ b  // a
            hash_table[index] ^= hash_table[exchange];
            // b      // a
        }

        // gradients selected on a hypercube
        for (size_t index{0}; index < pow_i<size_t>(3, dim); ++index) {
            size_t remainder{index};
            for (size_t coordinate{0}; coordinate < dim; ++coordinate) {
                static const std::array<float, 3> values{-1.f, 0.f, 1.f};
                gradients[index][coordinate] = values[remainder % 3];
                remainder /= 3;
            }
        }
    };

    float at(Vec<dim> const& point) const override {
        return process_simplex(skew(point));
    }
private:
    float max_sq_radius{0.6f};
    std::array<Vec<dim>, pow_i<size_t>(3, dim)> gradients;
    std::array<hash_table_members, hash_table_size> hash_table;

    void sub_skew(Vec<dim>& vec, float const factor) const {
        float sum{0.f};
        size_t index{0};
        while (index < dim)
            sum += vec[index++];
        sum *= factor;
        while (index--) {
            vec[index] += sum;
        }
    }

    Vec<dim> skew(Vec<dim> const& vec) const {
        static float const factor{(sqrtf(dim + 1) - 1.f) / static_cast<float>(dim)};
        Vec<dim> result{vec};
        sub_skew(result, factor);
        return result;
    }

    Vec<dim> get_gradient(Vec<dim> const& v) const {
        size_t index{0};
        for (size_t coordinate{0}; coordinate < dim; ++coordinate) {
            index = static_cast<size_t>(hash_table[(index + static_cast<size_t>(v[coordinate])) & (hash_table_size-1)]);
        }
        return gradients[index % pow_i<size_t>(3, dim)];
    }

    float process_simplex(Vec<dim> const& vec) const {
        Vec<dim> vertex{floor(vec)};
        Vec<dim> frac_part{vec - vertex};

        std::array<size_t, dim> sorted_indices;
        for (size_t index{0}; index < dim; ++index) {
            sorted_indices[index] = index;
        }

        std::ranges::sort(sorted_indices.begin(), sorted_indices.end(), [frac_part](size_t a, size_t b) {
            return frac_part[a] > frac_part[b];
        });

        float result{0.f};
        size_t index{0};
        while (index <= dim) {

            Vec<dim> dv{vec - vertex};
            un_skew(dv);
            float sq_distance = sq_mag(dv);
            float coefficient{max_sq_radius - sq_distance};
            coefficient = (coefficient < 0.f) ? 0.f : coefficient;
            coefficient *= coefficient;
            coefficient *= coefficient;

            Vec<dim> gradient;
            result += dot(get_gradient(vertex), dv) * coefficient;

            if (index == dim) break;
            vertex[sorted_indices[index++]] += 1.f;
        }
        return result;
    }

    void un_skew(Vec<dim>& vec) const {
        static float const factor{-(1.f - 1.f / sqrtf(dim + 1)) / static_cast<float>(dim)};
        return sub_skew(vec, factor);
    }
};


template<size_t dim>
class FractalNoise : public Noise<dim> {
public:
    FractalNoise(std::unique_ptr<Noise<dim>> noise, size_t const octaves, float const persistence, float const lacunarity) : noise{move(noise)}, octaves{octaves}, persistence{persistence}, lacunarity{lacunarity}, normalized_amp{(persistence - 1.f) / (pow_i<float>(persistence, octaves+1) - 1.f)} {
        for (size_t index{0}; index < octaves; index++) {
            Vec<dim> center;
            for (size_t d{0}; d < dim; d++) {
                center[d] = 200.f * (2.f * random_float() - 1.f);
            }
            octave_centers.push_back(center);
        }
    };
    float at(Vec<dim> const& point) const override {
        float result{0.f};
        float amp = normalized_amp;
        float pulse = 1.f;

        for (size_t index{0}; index < octaves; index++) {
            result += amp * noise->at(pulse * point - octave_centers[index]);
            amp *= persistence;
            pulse *= lacunarity;
        }
        return result;
    }

private:
    std::unique_ptr<Noise<dim>> noise;
    size_t const octaves;
    float const persistence;
    float const lacunarity;
    float const normalized_amp;
    std::vector<Vec<dim>> octave_centers;
};


constexpr float NOISE4_SCALE = 24.f;

constexpr float normalize_simplex4(const float n) {
    float const value = NOISE4_SCALE * n;
    if (value < -0.5f)
        return 0.f;
    if (value > 0.5f)
        return 1.f;
    return value + .5f;
}

/**
 * Noise for a toroidal world without distortions
 */
class Toroidal4 : public Noise<2> {
public:
    explicit Toroidal4(std::unique_ptr<Noise<4>> noise, float rad1, float rad2, float x_period, float y_period);
    [[nodiscard]] float at(Vec<2> const& point) const override;
private:
    std::unique_ptr<Noise<4>> noise;
    float const rad1; float const rad2;
    float const x_period; float const y_period;
};

class ToroidalPair {
public:
    explicit ToroidalPair(std::unique_ptr<Noise<4>> noise, float rad1, float rad2, float x_period, float y_period, Vec<4> center1, Vec<4> center2);
    [[nodiscard]] Vec<2> at(Vec<2> const& point) const ;

private:
    std::unique_ptr<Noise<4>> noise;
    float const rad1; float const rad2;
    float const x_period; float const y_period;
    Vec<4> center1; Vec<4> center2;
};

#endif // DEV_CPP_NOISE_H
