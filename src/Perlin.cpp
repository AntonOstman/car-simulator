#include <cstdint>
#include <cmath>
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Perlin.hpp"
#include "Debug.hpp"

uint32_t PerlinNoise::wang_hash(int x, int y, uint32_t seed) {
    uint32_t hash_val = seed;
    hash_val ^= static_cast<uint32_t>(x) * 0x1f351f35U;
    hash_val ^= static_cast<uint32_t>(y) * 0x3f893f89U;
    hash_val = (hash_val ^ (hash_val >> 16)) * 0x85ebca6bU;
    hash_val = (hash_val ^ (hash_val >> 13)) * 0xc2b2ae35U;
    hash_val = hash_val ^ (hash_val >> 16);
    return hash_val;
}

float PerlinNoise::perlin_hash(int x, int y, uint32_t seed) {
    uint32_t hash_val = wang_hash(x, y, seed);

    // Bitmask 16 bits and scale value [0, 2pi]
    return static_cast<float>(hash_val & 0xFFFF) * (2.0f * glm::pi<float>() / 65536.0f);
}

float PerlinNoise::perlin_hash_discrete(int x, int y, uint32_t seed) {
    uint32_t hash_val = wang_hash(x, y, seed);

    uint32_t gradient_index = hash_val & 7;  

    return static_cast<float>(gradient_index) * glm::pi<float>() / 4.0f;
}


float PerlinNoise::improved_perlin_hash(int x, int y, uint32_t seed) {
    uint32_t hash_val = wang_hash(x, y, seed);

    const glm::vec2 gradients[12] = {
        glm::vec2(1.0f, 1.0f), glm::vec2(-1.0f, 1.0f), glm::vec2(1.0f, -1.0f), glm::vec2(-1.0f, -1.0f),  
        glm::vec2(1.0f, 0.0f), glm::vec2(-1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, -1.0f),    
        glm::vec2(2.0f, 1.0f), glm::vec2(-2.0f, 1.0f), glm::vec2(1.0f, 2.0f), glm::vec2(-1.0f, 2.0f)     
    };

    uint32_t gradient_index = hash_val % 12;
    return atan2(gradients[gradient_index].y, gradients[gradient_index].x);
}

glm::vec2 PerlinNoise::improved_perlin_gradient(int x, int y, uint32_t seed) {
    uint32_t hash_val = wang_hash(x, y, seed);

    const glm::vec2 gradients[12] = {

        glm::vec2(0.7071f, 0.7071f), glm::vec2(-0.7071f, 0.7071f), 
        glm::vec2(0.7071f, -0.7071f), glm::vec2(-0.7071f, -0.7071f),

        glm::vec2(1.0f, 0.0f), glm::vec2(-1.0f, 0.0f), 
        glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, -1.0f),

        glm::vec2(0.8944f, 0.4472f), glm::vec2(-0.8944f, 0.4472f), 
        glm::vec2(0.4472f, 0.8944f), glm::vec2(-0.4472f, 0.8944f)
    };

    uint32_t gradient_index = hash_val % 12;
    return gradients[gradient_index];
}

float PerlinNoise::terrain_noise(int x, int y, uint32_t seed) {
    uint32_t hash_val = wang_hash(x, y, seed);
    return static_cast<float>(hash_val & 0xFFFFFF) / static_cast<float>(0xFFFFFF);
}

float PerlinNoise::smoothstep(float t) {
    return t * t * (3.0f - 2.0f * t);
}

float PerlinNoise::fade(float t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float PerlinNoise::perlin_noise_at(const glm::vec2& pos, uint32_t seed) {

    glm::ivec2 grid_pos = glm::ivec2(glm::floor(pos));

    glm::vec2 local_pos = pos - glm::vec2(grid_pos);

    glm::vec2 g00 = improved_perlin_gradient(grid_pos.x, grid_pos.y, seed);
    glm::vec2 g01 = improved_perlin_gradient(grid_pos.x + 1, grid_pos.y, seed);
    glm::vec2 g10 = improved_perlin_gradient(grid_pos.x, grid_pos.y + 1, seed);
    glm::vec2 g11 = improved_perlin_gradient(grid_pos.x + 1, grid_pos.y + 1, seed);

    glm::vec2 d00 = local_pos;
    glm::vec2 d01 = local_pos - glm::vec2(1.0f, 0.0f);
    glm::vec2 d10 = local_pos - glm::vec2(0.0f, 1.0f);
    glm::vec2 d11 = local_pos - glm::vec2(1.0f, 1.0f);

    float dot00 = glm::dot(g00, d00);
    float dot01 = glm::dot(g01, d01);
    float dot10 = glm::dot(g10, d10);
    float dot11 = glm::dot(g11, d11);

    glm::vec2 fade_pos = glm::vec2(fade(local_pos.x), fade(local_pos.y));

    float top = glm::mix(dot00, dot01, fade_pos.x);
    float bottom = glm::mix(dot10, dot11, fade_pos.x);
    return glm::mix(top, bottom, fade_pos.y);
}

float PerlinNoise::octave_noise(const glm::vec2& pos, uint32_t seed, 
                         int octaves, float persistence, float lacunarity) {
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float max_value = 0.0f;

    for (int i = 0; i < octaves; ++i) {
        value += perlin_noise_at(pos * frequency, seed + i) * amplitude;
        max_value += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return value / max_value;
}
