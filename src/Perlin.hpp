#include "glm/fwd.hpp"
#include "glm/vec2.hpp"
#include <cstdint>

class PerlinNoise {

public:
    static float perlin_noise_at(const glm::vec2& pos, uint32_t seed);
    static float octave_noise(const glm::vec2& pos, uint32_t seed,
                             int octaves = 4, float persistence = 0.5f, float lacunarity = 2.0f);

private:
    static uint32_t wang_hash(int x, int y, uint32_t seed);
    static float perlin_hash(int x, int y, uint32_t seed);
    static float perlin_hash_discrete(int x, int y, uint32_t seed);
    static float improved_perlin_hash(int x, int y, uint32_t seed);
    static glm::vec2 improved_perlin_gradient(int x, int y, uint32_t seed);
    static float terrain_noise(int x, int y, uint32_t seed);
    static float smoothstep(float t);
    static float fade(float t);

    // Convinience functions
public:

    inline static float octave_noise(float x, float y, uint32_t seed, 
                             int octaves = 4, float persistence = 0.5f, float lacunarity = 2.0f) {
        return octave_noise(glm::vec2(x, y), seed, octaves, persistence, lacunarity);
    }

    inline static float perlin_noise_at(float x, float y, uint32_t seed) {
        return perlin_noise_at(glm::vec2(x, y), seed);
    }

private:

    inline static uint32_t wang_hash(const glm::ivec2& pos, uint32_t seed) {
        return wang_hash(pos.x, pos.y, seed);
    }

    inline static float perlin_hash(const glm::ivec2& pos, uint32_t seed) {
        return perlin_hash(pos.x, pos.y, seed);
    }

    inline static float improved_perlin_hash(const glm::ivec2& pos, uint32_t seed) {
        return improved_perlin_hash(pos.x, pos.y, seed);
    }


    inline static float perlin_hash_discrete(const glm::ivec2& pos, uint32_t seed) {
        return perlin_hash_discrete(pos.x, pos.y, seed);
    }

    inline static glm::vec2 improved_perlin_gradient(const glm::ivec2& pos, uint32_t seed) {
        return improved_perlin_gradient(pos.x, pos.y, seed);
    }

    inline static float terrain_noise(const glm::ivec2& pos, uint32_t seed) {
        return terrain_noise(pos.x, pos.y, seed);
    }
};
