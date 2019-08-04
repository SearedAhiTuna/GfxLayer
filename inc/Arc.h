
#pragma once

#include "function/Function.h"
#include "Types.h"
#include "Model.h"

#include <list>
#include <memory>

#include <iostream>

class Arc
{
private:
    typedef Function<vec3, GLfloat> FunctionR1R3;

public:
    Arc(FunctionR1R3* func, GLfloat t0 = 0, GLfloat tf = 1);

    vec3 func(const GLfloat& t);

    void generate(Model& m, const size_t& res);

    template <typename EdgesOut>
    void generate(Model& m, const size_t& res, EdgesOut& output);

    template <typename EdgesIn, typename FacesOut>
    void connect_edges(Model& m, EdgesIn& edges1, EdgesIn& edges2, const size_t& res, FacesOut& output);

    template <typename EdgesIn, typename FacesOut>
    void connect_fan(Model& m, Model::Vert& v, EdgesIn& edges, const size_t& res, FacesOut& output);

private:
    void connect_verts(Model& m, Model::Vert& v1, Model::Vert& v2, const size_t& res, std::list<Model::Vert*>& output);

private:
    std::unique_ptr<FunctionR1R3> _func;
    GLfloat _t0;
    GLfloat _tf;
};

template <typename EdgesOut>
void Arc::generate(Model& m, const size_t& res, EdgesOut& output)
{
    // Emplace the first vertex
    Model::Vert* prev = &m.verts.emplace((*_func)(_t0));

    // Get the step
    GLfloat step = res > 1 ? ((_tf - _t0) / (res - 1)) : 0;

    // Extrude up to the last vertex
    for (size_t i = 1; i < res; ++i)
    {
        GLfloat t = _t0 + i * step;

        // Extrude the next vertex
        Model::Vert* cur = &prev->extrude();

        // Set the position
        vec3 pos = (*_func)(t);
        cur->attribs.at<vec3>(MDL_ATT_POSITION) = pos;

        // Get the connecting edge
        Model::Edge* e = m.edges.between(*prev, *cur);
        output.push_back(e);

        prev = cur;
    }
}

template <typename EdgesIn, typename FacesOut>
void Arc::connect_edges(Model& m, EdgesIn& edges1, EdgesIn& edges2, const size_t& res, FacesOut& output)
{
    if (edges1.size() != edges2.size())
    {
        // Bad args
        return;
    }

    std::list<Model::Vert*> verts1;
    m.verts.within_edges(edges1, verts1);

    std::list<Model::Vert*> verts2;
    m.verts.within_edges(edges2, verts2);

    std::list<Model::Vert*> prev;

    for (auto it1 = verts1.begin(), it2 = verts2.begin(); it1 != verts1.end(); ++it1, ++it2)
    {
        std::list<Model::Vert*> cur;
        connect_verts(m, **it1, **it2, res, cur);

        if (!prev.empty())
        {
            m.faces.emplace_edges(prev, cur, output);
        }

        prev = cur;
    }
}

template<typename EdgesIn, typename FacesOut>
void Arc::connect_fan(Model& m, Model::Vert& v, EdgesIn& edges, const size_t& res, FacesOut& output)
{
    std::list<Model::Vert*> verts;
    m.verts.within_edges(edges, verts);

    std::list<Model::Vert*> prev;

    for (auto it = verts.begin(); it != verts.end(); ++it)
    {
        std::list<Model::Vert*> cur;
        connect_verts(m, v, **it, res, cur);

        if (!prev.empty())
        {
            m.faces.emplace_edges(prev, cur, output);
        }

        prev = cur;
    }
}
