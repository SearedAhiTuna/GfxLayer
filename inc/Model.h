
#pragma once

#include "Mesh.h"
#include "Libs.h"

#include <iostream>

#define MDL_ATT_POSITION 0
#define MDL_ATT_UV 1
#define MDL_ATT_NORMAL 2

class Shape;

class Model : public Mesh
{
public:
    void vert_tf_3d(Vert& v, const mat4& tf);

    template <typename VertsIn>
    void vert_tf_3d(const VertsIn& vs, const mat4& tf);

    void edge_tf_3d(Edge& e, const mat4& tf);

    template <typename EdgesIn>
    void edge_tf_3d(const EdgesIn& es, const mat4& tf);

    void face_tf_3d(Face& f, const mat4& tf);

    template <typename FacesIn>
    void face_tf_3d(const FacesIn& fs, const const mat4& tf);

    void generate_vert_normals();
    void generate_face_normals();

    Shape* generate_shape(const GLenum& mode = GL_TRIANGLES);
    void generate_shape(Shape& shape, const GLenum& mode = GL_TRIANGLES);

    void export_obj(std::ostream& os, const GLfloat& merge = -1);
    void import_obj(std::istream& is);

private:
    bool _useFaceNormals{};
    std::map<Face*, vec3> _normals;
};

template <typename VertsIn>
void Model::vert_tf_3d(const VertsIn& vs, const mat4& tf)
{
    for (Vert* v : vs)
    {
        vert_tf_3d(*v, tf);
    }
}

template <typename EdgesIn>
void Model::edge_tf_3d(const EdgesIn& es, const mat4& tf)
{
    std::list<Vert*> vs;
    verts.within_edges(es, vs);

    for (Vert* v : vs)
    {
        vert_tf_3d(*v, tf);
    }
}