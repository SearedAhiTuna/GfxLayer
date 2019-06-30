
#pragma once

#include "Mesh.h"
#include "Libs.h"

#define MDL_ATT_POSITION 0
#define MDL_UV_POSITION 1
#define MDL_NORM_POSITION 2

class Shape;

class Model : public Mesh
{
public:
    void vert_tf_3d(Vert& v, const AttributeID& att, const mat4& tf);

    template <typename VertsIn>
    void vert_tf_3d(const VertsIn& vs, const AttributeID& att, const mat4& tf);

    void edge_tf_3d(Edge& e, const AttributeID& att, const mat4& tf);

    template <typename EdgesIn>
    void edge_tf_3d(const EdgesIn& es, const AttributeID& att, const mat4& tf);

    void face_tf_3d(Face& f, const AttributeID& att, const mat4& tf);

    template <typename FacesIn>
    void face_tf_3d(const FacesIn& fs, const AttributeID& att, const mat4& tf);

    void generate_vert_normals(const AttributeID& att, const AttributeID& pos_att = MDL_ATT_POSITION);
    void generate_face_normals(const AttributeID& att, const AttributeID& pos_att = MDL_ATT_POSITION);

    Shape* generate_shape(const GLenum& mode = GL_TRIANGLES);
    void generate_shape(Shape& shape, const GLenum& mode = GL_TRIANGLES);

private:
    bool _useFaceNormals{};
    AttributeID _normAtt;
    std::map<Face*, vec3> _normals;
};

template <typename VertsIn>
void Model::vert_tf_3d(const VertsIn& vs, const AttributeID& att, const mat4& tf)
{
    for (Vert* v : vs)
    {
        vert_tf_3d(*v, att, tf);
    }
}

template <typename EdgesIn>
void Model::edge_tf_3d(const EdgesIn& es, const AttributeID& att, const mat4& tf)
{
    std::list<Vert*> vs;
    verts.within_edges(es, vs);

    for (Vert* v : vs)
    {
        vert_tf_3d(*v, att, tf);
    }
}