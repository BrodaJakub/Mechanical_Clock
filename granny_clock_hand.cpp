#include "granny_clock_hand.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

namespace Models {

    GrannyClockHand grannyClockHand("granny_clock_hand.obj");

    GrannyClockHand::GrannyClockHand(const std::string& filename) {
        std::vector<glm::vec4> temp_vertices;
        std::vector<glm::vec3> temp_normals;
        std::vector<glm::vec2> temp_texCoords;

        std::vector<unsigned int> vertexIndices, normalIndices, texCoordIndices;

        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Could not open OBJ file: " << filename << std::endl;
            vertexCount = 0;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "v") {
                glm::vec3 v;
                iss >> v.x >> v.y >> v.z;
                temp_vertices.push_back(glm::vec4(v, 1.0f));
            }
            else if (prefix == "vn") {
                glm::vec3 n;
                iss >> n.x >> n.y >> n.z;
                temp_normals.push_back(n);
            }
            else if (prefix == "vt") {
                glm::vec2 t;
                iss >> t.x >> t.y;
                temp_texCoords.push_back(t);
            }
            else if (prefix == "f") {
                for (int i = 0; i < 3; ++i) {
                    std::string token;
                    iss >> token;

                    unsigned int vi = 0, ti = 0, ni = 0;
                    size_t vPos = token.find('/');
                    size_t tPos = token.find('/', vPos + 1);

                    if (vPos == std::string::npos) {
                        vi = std::stoi(token);
                    }
                    else if (tPos == std::string::npos || tPos == vPos + 1) {
                        vi = std::stoi(token.substr(0, vPos));
                        ni = std::stoi(token.substr(vPos + 2));
                    }
                    else {
                        vi = std::stoi(token.substr(0, vPos));
                        ti = std::stoi(token.substr(vPos + 1, tPos - vPos - 1));
                        ni = std::stoi(token.substr(tPos + 1));
                    }

                    vertexIndices.push_back(vi - 1);
                    texCoordIndices.push_back(ti ? ti - 1 : 0);
                    normalIndices.push_back(ni ? ni - 1 : 0);
                }
            }
        }

        vertexCount = vertexIndices.size();
        vertices = new float[vertexCount * 4];
        normals = new float[vertexCount * 4];
        vertexNormals = new float[vertexCount * 4];
        texCoords = new float[vertexCount * 4];
        colors = new float[vertexCount * 4];

        for (unsigned int i = 0; i < vertexCount; ++i) {
            glm::vec4 v = temp_vertices[vertexIndices[i]];
            glm::vec3 n = temp_normals.empty() ? glm::vec3(0.0f) : temp_normals[normalIndices[i]];
            glm::vec2 t = temp_texCoords.empty() ? glm::vec2(0.0f) : temp_texCoords[texCoordIndices[i]];

            for (int j = 0; j < 4; ++j) {
                vertices[i * 4 + j] = v[j];
                normals[i * 4 + j] = (j < 3) ? n[j] : 0.0f;
                vertexNormals[i * 4 + j] = (j < 3) ? n[j] : 0.0f;
            }

            texCoords[i * 4 + 0] = t.x;
            texCoords[i * 4 + 1] = t.y;
            texCoords[i * 4 + 2] = 0.0f;
            texCoords[i * 4 + 3] = 0.0f;

            colors[i * 4 + 0] = 1.0f;
            colors[i * 4 + 1] = 1.0f;
            colors[i * 4 + 2] = 1.0f;
            colors[i * 4 + 3] = 1.0f;
        }
    }

    GrannyClockHand::~GrannyClockHand() {
        delete[] vertices;
        delete[] normals;
        delete[] vertexNormals;
        delete[] texCoords;
        delete[] colors;
    }

    void GrannyClockHand::drawSolid(bool smooth) {
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, vertices);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, smooth ? vertexNormals : normals);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, texCoords);

        glDrawArrays(GL_TRIANGLES, 0, vertexCount);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }
}
