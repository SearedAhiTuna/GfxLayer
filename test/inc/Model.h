
#pragma once

#include "Mesh.h"
#include "Libs.h"

#define MDL_ATT_POSITION 0
#define MDL_ATT_UV 1

class Shape;

class Model : public Mesh
{
public:
    void vert_tf_2d(Vert& v, const AttributeID& att, const mat4& tf);
    void vert_tf_3d(Vert& v, const AttributeID& att, const mat4& tf);

    template <typename VertsIn>
    void vert_tf_2d(const VertsIn& v, const AttributeID& att, const mat4& tf);

    template <typename VertsIn>
    void vert_tf_3d(const VertsIn& v, const AttributeID& att, const mat4& tf);

    void edge_tf_2d(Edge& e, const AttributeID& att, const mat4& tf);
    void edge_tf_3d(Edge& e, const AttributeID& att, const mat4& tf);

    template <typename EdgesIn>
    void edge_tf_2d(const EdgesIn& e, const AttributeID& att, const mat4& tf);

    template <typename EdgesIn>
    void edge_tf_3d(const EdgesIn& e, const AttributeID& att, const mat4& tf);

    void face_tf_2d(Face& f, const AttributeID& att, const mat4& tf);
    void face_tf_3d(Face& f, const AttributeID& att, const mat4& tf);

    template <typename FacesIn>
    void face_tf_2d(const FacesIn& f, const AttributeID& att, const mat4& tf);

    template <typename FacesIn>
    void face_tf_3d(const FacesIn& f, const AttributeID& att, const mat4& tf);

    Shape* generate_shape(const GLenum& mode = GL_TRIANGLES);
    void generate_shape(Shape& shape, const GLenum& mode = GL_TRIANGLES);
};