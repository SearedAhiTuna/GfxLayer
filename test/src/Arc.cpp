
#include "Arc.h"



Arc::Arc(FunctionR1R3* func, GLfloat t0, GLfloat tf):
    _func(func),
    _t0(t0),
    _tf(tf)
{
}

void Arc::generate(Model& m, const size_t& res)
{
    std::list<Model::Edge*> dummy;
    generate(m, res, dummy);
}

void Arc::connect_verts(Model& m, Model::Vert& v1, Model::Vert& v2, const size_t& res, std::list<Model::Vert*>& output)
{
    GLfloat step = (_tf - _t0) / res;

    // Get unscaled start and end of this arc
    vec3 myStart = (*_func)(_t0);
    vec3 myEnd = (*_func)(_tf);

    // Get start and end vertices
    vec3 start = v1.getAtt<vec3>(MDL_ATT_POSITION);
    vec3 end = v2.getAtt<vec3>(MDL_ATT_POSITION);

    // Calculate scale
    vec3 myDisp = myEnd - myStart;
    vec3 disp = end - start;

    vec3 scVec(1,1,1);

    for (length_t i = 0; i != scVec.length(); ++i)
    {
        if (myDisp[i] != 0)
        {
            if (disp[i] == 0)
                scVec[i] = 0;
            else
                scVec[i] = disp[i] / myDisp[i];
        }
    }

    mat4 sc = scale(scVec);

    // Calculate transformation
    mat4 tf = translate(-myStart);
    tf = sc * tf;
    tf = translate(start) * tf;

    // Get the first vertex
    Model::Vert* prev = &v1;
    output.emplace_back(prev);

    // Extrude up to the last vertex
    for (size_t i = 1; i < res - 1; ++i)
    {
        GLfloat t = _t0 + i * step;

        // Extrude the previous vertex
        Model::Vert* cur = &prev->extrude();
        output.emplace_back(cur);

        // Set the vertex position
        vec3 val = (*_func)(t);
        val = (tf * vec4(val,1)).xyz;
        cur->setAtt(MDL_ATT_POSITION, val);

        // Set the current as previous
        prev = cur;
    }

    // Connect the last two vertices
    m.edges.emplace(*prev, v2);
    output.emplace_back(&v2);
}