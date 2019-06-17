
#include "Model.h"

#include "TFShape.h"

#define ATTRIB_POS 0
#define ATTRIB_UV 1

Graph::NodeIndex Model::vert_create(const vec3& vec)
{
    NodeIndex n = node_create();
    node_set_attrib<vec3>(n, ATTRIB_POS, vec);

    return n;
}

Graph::NodeIndex Model::vert_create(const vec2& vec)
{
    vec3 v3 = vec3(vec, 0);
    return vert_create(v3);
}

Graph::NodeIndex Model::vert_create(const vec3& vec, const vec2& uv)
{
    NodeIndex n = node_create();
    node_set_attrib<vec3>(n, ATTRIB_POS, vec);
    node_set_attrib<vec2>(n, ATTRIB_UV, uv);
    textured = true;

    return n;
}

Graph::NodeIndex Model::vert_create(const vec2& vec, const vec2& uv)
{
    vec3 v3 = vec3(vec, 0);
    return vert_create(v3, uv);
}

Model::FaceIndex Model::face_create_from_edges(const std::initializer_list<EdgeIndex>& es)
{
    FaceIndex f = (FaceIndex)faces.size();
    faces.emplace_back(es);

    return f;
}

Model::FaceIndex Model::face_create_from_nodes(const std::initializer_list<NodeIndex>& ns)
{
    FaceIndex f = (FaceIndex)faces.size();
    faces.emplace_back();

    Face& face = faces.back();

    for (size_t i = 0; i < ns.size(); ++i)
    {
        size_t next = (i + 1) % ns.size();

        NodeIndex n1 = ns.begin()[i];
        NodeIndex n2 = ns.begin()[next];

        EdgeIndex e = edge_create(n1, n2);

        face.edges.emplace_back(e);
    }

    return f;
}

vec3 Model::node_get_vert(const NodeIndex& n) const
{
    return node_get_attrib<vec3>(n, ATTRIB_POS);
}

vec2 Model::node_get_uv(const NodeIndex& n) const
{
    return node_get_attrib<vec2>(n, ATTRIB_UV);
}

Shape* Model::genShape() const
{
    std::vector<vec3> verts;
    verts.reserve(1);

    std::vector<vec2> uvs;
    if (textured)
        uvs.reserve(1);

    std::list<FaceIndex> fs;
    face_get_all<std::list<FaceIndex>>(fs);
    for (const FaceIndex& f : fs)
    {
        std::vector<NodeIndex> ns;
        face_get_nodes<std::vector<NodeIndex>>(ns, f);

        size_t needed = (ns.size() - 2) * 3;
        while (verts.capacity() < verts.size() + needed)
        {
            verts.reserve(verts.capacity() * 2);
            if (textured)
                uvs.reserve(verts.capacity() * 2);
        }

        for (size_t i = 2; i < ns.size(); ++i)
        {
            verts.push_back(node_get_vert(ns[0]));
            verts.push_back(node_get_vert(ns[i - 1]));
            verts.push_back(node_get_vert(ns[i]));

            if (textured)
            {
                uvs.push_back(node_get_uv(ns[0]));
                uvs.push_back(node_get_uv(ns[i - 1]));
                uvs.push_back(node_get_uv(ns[i]));
            }
        }
    }

    TFShape* shape = new TFShape(verts);
    if (textured)
        shape->bufferCreate(uvs);
    return shape;
}

std::ostream& operator<<(std::ostream& os, const Model& m)
{
    bool first = true;

    // List vertices
    std::list<Graph::NodeIndex> ns;
    m.node_get_all<std::list<Graph::NodeIndex>>(ns);

    for (const Graph::NodeIndex& n : ns)
    {
        if (first)
            first = false;
        else
            os << std::endl;

        vec3 vec = m.node_get_vert(n);

        os << "v";
        os << " " << vec.x;
        os << " " << vec.y;
        os << " " << vec.z;
    }

    // List UV
    if (m.isTextured())
    {
        for (const Graph::NodeIndex& n : ns)
        {
            if (first)
                first = false;
            else
                os << std::endl;

            vec2 uv = m.node_get_uv(n);

            os << "vt";
            os << " " << uv.x;
            os << " " << uv.y;
        }
    }

    // List faces
    std::list<Model::FaceIndex> fs;
    m.face_get_all<std::list<Model::FaceIndex>>(fs);

    for (const Model::FaceIndex& f : fs)
    {
        if (first)
            first = false;
        else
            os << std::endl;

        os << "f";
        
        std::list<Graph::NodeIndex> faceN;
        m.face_get_nodes<std::list<Graph::NodeIndex>>(faceN, f);

        for (const Graph::NodeIndex& n : faceN)
        {
            os << " " << (n + 1);
        }
    }

    return os;
}