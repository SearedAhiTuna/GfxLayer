
#pragma once

#include "Graph.h"
#include "Libs.h"

#include <initializer_list>
#include <iostream>
#include <list>

class Shape;

class Model : public Graph
{
public:
    typedef int FaceIndex;

private:
    struct Face
    {
        bool invalid{};
        std::list<EdgeIndex> edges;

        Face(const std::initializer_list<EdgeIndex>& es = {})
        {
            for (const EdgeIndex& e : es)
                edges.emplace_back(e);
        }
    };

public:
    NodeIndex vert_create(const vec3& vec = VEC3_ORIGIN);
    NodeIndex vert_create(const vec2& vec);

    NodeIndex vert_create(const vec3& vec, const vec2& uv);
    NodeIndex vert_create(const vec2& vec, const vec2& uv);

    FaceIndex face_create_from_edges(const std::initializer_list<EdgeIndex>& es);
    FaceIndex face_create_from_nodes(const std::initializer_list<NodeIndex>& ns);

    vec3 node_get_vert(const NodeIndex& n) const;
    vec2 node_get_uv(const NodeIndex& n) const;

    template <class Container>
    void face_get_edges(Container& c, const FaceIndex& f) const
    {
        const Face& face = faces[f];

        for (const EdgeIndex& e : face.edges)
        {
            c.push_back(e);
        }
    }

    template <class Container>
    void face_get_nodes(Container& c, const FaceIndex& f) const
    {
        const Face& face = faces[f];

        std::list<NodeIndex> allN;
        std::unordered_set<NodeIndex> prevN;

        for (const EdgeIndex& e : face.edges)
        {
            std::vector<NodeIndex> ns;
            edge_get_connected_nodes<std::vector<NodeIndex>>(ns, e);

            for (const NodeIndex& n : ns)
            {
                if (!prevN.count(n))
                {
                    allN.emplace_back(n);
                    prevN.emplace(n);
                }
            }
        }

        for (const NodeIndex& n : allN)
        {
            c.push_back(n);
        }
    }

    template <class Container>
    void face_get_all(Container& c) const
    {
        FaceIndex f = 0;
        for (const Face& face : faces)
        {
            if (!face.invalid)
                c.push_back(f);

            ++f;
        }
    }

    Shape* genShape() const;

    const bool& isTextured() const { return textured; }

private:
    std::vector<Face> faces;
    bool textured{};
};

std::ostream& operator<<(std::ostream& os, const Model& m);