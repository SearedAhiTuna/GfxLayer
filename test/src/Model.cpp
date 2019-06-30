
#include "Model.h"
#include "Shape.h"
#include "vecx.h"

#include <list>
#include <map>

void Model::vert_tf_3d(Vert& v, const AttributeID& att, const mat4& tf)
{
    vec4 vec(v.getAtt<vec3>(att), 1);
    vec = tf * vec;
    v.setAtt<vec3>(att, vec.xyz);
}

void Model::edge_tf_3d(Edge& e, const AttributeID& att, const mat4& tf)
{
    std::list<Vert*> vs;
    e.adjacent_verts(vs);

    for (Vert* v : vs)
    {
        vert_tf_3d(*v, att, tf);
    }
}

void Model::face_tf_3d(Face& f, const AttributeID& att, const mat4& tf)
{
    std::list<Vert*> vs;
    f.adjacent_verts(vs);

    for (Vert* v : vs)
    {
        vert_tf_3d(*v, att, tf);
    }
}

Shape* Model::generate_shape(const GLenum& mode)
{
    Shape* shape = new Shape(mode);
    generate_shape(*shape, mode);
    return shape;
}

void Model::generate_vert_normals(const AttributeID& att, const AttributeID& pos_att)
{
    for (Vert& v : verts)
    {
        vec3 pos = v.getAtt<vec3>(pos_att);

        std::list<Vert*> adj;
        v.adjacent_verts(adj);

        vec3 n;

        // Get normal facing outwards from the corner
        for (Vert* pv : adj)
        {
            n += pv->getAtt<vec3>(pos_att) - pos;
        }
        n = normalize(-n);

        v.setAtt(att, vec4(n, 0));
    }
}

void Model::generate_face_normals(const AttributeID& att, const AttributeID& pos_att)
{
    _useFaceNormals = true;
    _normAtt = att;
    _normals.clear();

    for (Face& f : faces)
    {
        std::list<Vert*> adj;
        f.adjacent_verts(adj);

        vec3 v1 = adj.front()->getAtt<vec3>(pos_att);
        vec3 v2 = (*++adj.begin())->getAtt<vec3>(pos_att);
        vec3 v3 = adj.back()->getAtt<vec3>(pos_att);

        vec3 d1 = v2 - v1;
        vec3 d2 = v3 - v1;

        vec3 n = normalize(cross(d1, d2));

        _normals[&f] = n;
    }
}

void Model::generate_shape(Shape& shape, const GLenum& mode)
{
    // Get every attribute ID
    std::vector<AttributeID> attrs;
    verts.allAtt(attrs);

    // If using hard normals, add the normal attribute
    if (_useFaceNormals)
        attrs.emplace_back(_normAtt);

    // Create containers to hold all values
    std::vector<std::list<vecx>> values(attrs.size());

    if (!(faces.begin() != faces.end()))
    {
        // No faces
        // Do nothing
        return;
    }

    // For each face
    for (Face& f : faces)
    {
        // Get all vertices in the face
        std::vector<Vert*> vs;
        f.adjacent_verts(vs);

        /*std::cout << "Verts in order: ";
        for (Vert* v : vs)
            std::cout << " " << *v;
        std::cout << "\n";*/

        // Construct the face out of triangles
        for (size_t v = 2; v < vs.size(); ++v)
        {
            for (size_t i = 0; i < attrs.size(); ++i)
            {
                AttributeID id = attrs[i];

                if (_useFaceNormals && id == _normAtt)
                {
                    // Use the normal for the face
                    std::any n(vec4(_normals[&f], 0));
                    for (int j = 0; j < 3; ++j)
                        values[i].emplace_back(n);
                }
                else
                {
                    // Add a triangle with the attribute values
                    values[i].emplace_back(vs[0]->getAtt(id));
                    values[i].emplace_back(vs[v - 1]->getAtt(id));
                    values[i].emplace_back(vs[v]->getAtt(id));
                }

                //std::cout << id << ": " << *vs[0] << "-" << *vs[v - 1] << "-" << *vs[v] << "\n";
            }
        }
    }

    // Get the number of vertices
    size_t nverts;
    if (attrs.empty())
        nverts = 0;
    else
        nverts = values.front().size();

    /*std::cout << "All verts:\n";
    for (const vecx& v : values.front())
    {
        std::cout << v.length() << ":";
        for (int i = 0; i < v.length(); ++i)
            std::cout << " " << v[i];
        std::cout << "\n";
    }*/
    
    // Resize the provided shape
    shape = Shape(nverts, mode);

    // Add the vertices to the shape
    for (size_t i = 0; i < attrs.size(); ++i)
    {
        // Get the dimensions for this attribute
        size_t ndims = values[i].front().length();

        // If this is not a vector attribute
        if (ndims == 0)
            continue;

        auto buf = shape.GenBuffer(ndims);
        buf.WriteRange(values[i]);
    }
}