#pragma once
#include <vector>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

static class ObjectLoader {
    static bool LoadObj(
        const char* path,
        std::vector < glm::vec3 >& out_vertices,
        std::vector < glm::vec2 >& out_uvs,
        std::vector < glm::vec3 >& out_normals
    );
};

