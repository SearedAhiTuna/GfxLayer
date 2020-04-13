
#pragma once

#include "Shape.h"
#include "Types.h"
#include <initializer_list>
#include <iostream>
#include <list>
#include <map>
#include <utility>
#include <vector>

class Mesh
{
private:
    struct int_pair : public std::pair<int, int>
    {
        using std::pair<int, int>::pair;

        int_pair()
        {
        }
        int_pair(int first_, int second_)
        {
            if (first_ <= second_)
            {
                first = first_;
                second = second_;
            }
            else
            {
                first = second_;
                second = first_;
            }
        }
    };

    struct VertData
    {
        vec3 pos;
        vec2 uv;

        VertData() {}
        VertData(const vec3& pos_) : pos(pos_) {}
    };

public:
    enum NormalGeneration
    {
        NO_NORMALS,
        NORMALS_PER_VERTEX,
        NORMALS_PER_FACE
    };

public:
    Mesh(NormalGeneration normals = NO_NORMALS);

    //--------
    // Emplace
    //--------
    int EmplaceVert(const vec3& pos = VEC3_ORIGIN);
    int EmplaceVert(GLfloat x, GLfloat y, GLfloat z)
    {
        return EmplaceVert(glm::vec3(x, y, z));
    }

    int EmplaceFace(const std::list<int>& verts);

    //--------
    // Extrude
    //--------
    int ExtrudeVert(int v, glm::mat4 tf = glm::mat4());
    void ExtrudeVerts(const std::list<int>& verts, glm::mat4 tf = glm::mat4(), std::list<int>* outVerts = nullptr);

    //------------------
    // Normal Generation
    //------------------
    vec3 VertNormal(int v) const;

    vec3 FaceNormal(int f) const;

    //------------
    // Vertex Data
    //------------
    const vec3& VertPos(int v) const;
    void VertPos(int v, const vec3& pos);

    const vec2& VertUV(int v) const;
    void VertUV(int v, const vec2& uv);

    //-------
    // Export
    //-------
    std::ostream& ToObj(std::ostream& out, bool nl = true) const;

    void ToShape(Shape& s, int posAtt = 0, int uvAtt = -1, int normAtt = -1) const;
    Shape* ToShape(int posAtt = 0, int uvAtt = -1, int normAtt = -1) const
    {
        Shape* s = new Shape(GL_TRIANGLES);
        ToShape(*s, posAtt, uvAtt, normAtt);
        return s;
    }

private:
    bool mTextured{};
    NormalGeneration mNormals;

    std::vector<VertData> mVerts;

    std::vector<std::list<int>> mFaces;  // face -> verts
    std::map<int, std::list<int>> mFacesReverse;  // vert -> faces
};

inline std::ostream& operator<<(std::ostream& out, const Mesh& m)
{
    return m.ToObj(out, false);
}