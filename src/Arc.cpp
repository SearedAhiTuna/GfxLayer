
#include "Arc.h"

#define TF(MAT4, VEC3) ((MAT4 * vec4(VEC3, 1)).xyz)

static vec3 angle_between(const vec3& v1, const vec3& v2)
{
    GLfloat num = dot(v1, v2);
    GLfloat denom = length(v1) * length(v2);

    GLfloat cosTheta = num / denom;
    GLfloat theta = acosf(cosTheta);

    vec3 crossProd = cross(v1, v2);

    return theta * normalize(crossProd);
}

Arc::Arc(FunctionR1R3* func, GLfloat t0, GLfloat tf):
    _func(func),
    _t0(t0),
    _tf(tf)
{
}

vec3 Arc::func(const GLfloat& t)
{
    return (*_func)(t);
}

void Arc::generate(Model& m, const size_t& res)
{
    std::list<Model::Edge*> dummy;
    generate(m, res, dummy);
}

void Arc::connect_verts(Model& m, Model::Vert& v1, Model::Vert& v2, const size_t& res, std::list<Model::Vert*>& output)
{
    // Get displacement of connector function
    vec3 myStart = (*_func)(_t0);
    vec3 myEnd = (*_func)(_tf);
    vec3 myDisp = myEnd - myStart;
    
    //vec3 myDir = normalize(myDisp);
    //GLfloat myDist = length(myDisp);

    // Get displacement from source to destination
    vec3 start = v1.attribs.at<vec3>(MDL_ATT_POSITION);
    vec3 end = v2.attribs.at<vec3>(MDL_ATT_POSITION);
    vec3 disp = end - start;

    //vec3 dir = normalize(disp);
    //GLfloat dist = length(disp);

    // Attempt to connect by scaling each axis
    vec3 scVec(1,1,1);

    // Keep track of which dimensions cannot be scaled
    vec3 zeroAxis;

    for (length_t i = 0; i != 3; ++i)
    {
        zeroAxis[i] = (myDisp[i] == 0) ? 0.0f : 1.0f;

        if (myDisp[i] != 0)
        {
            if (disp[i] == 0)
                scVec[i] = 0;
            else
                scVec[i] = disp[i] / myDisp[i];
        }
    }

    mat4 baseTF = scale(scVec);

    // Check if extra transformation is needded is needed
    vec3 tfDisp = TF(baseTF, myDisp);
    vec3 diff = disp - tfDisp;
    bool extra = length(diff) > .000001;

    // Apply extra transformation
    if (extra)
    {
        // Get headings along the yz plane
        vec3 heading = vec3(0, 1, 1) * disp;
        vec3 myHeading = vec3(0, 1, 1) * myDisp;

        // Get the angle to rotate
        vec3 angBetween = angle_between(myHeading, heading);

        // Get the perpendicular scale factor
        GLfloat perpScale = length(heading) / length(myHeading);

        // Get the parallel scale factor
        GLfloat parScale = length(disp - heading) / length(myDisp - myHeading);

        // Rotate about the x axis
        baseTF = rotate(length(angBetween), vec3(1, 0, 0));

        // Scale to the correct size
        baseTF = scale(vec3(parScale, perpScale, perpScale)) * baseTF;
    }

    // Calculate transformation
    mat4 tf = translate(-myStart);
    tf = baseTF * tf;
    tf = translate(start) * tf;

    // Get the first vertex
    Model::Vert* prev = &v1;
    output.emplace_back(prev);

    // Get the step
    GLfloat step = (_tf - _t0) / (res - 1);

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
        cur->attribs.at<vec3>(MDL_ATT_POSITION) = val;

        // Set the current as previous
        prev = cur;
    }

    // Connect the last two vertices
    m.edges.emplace(*prev, v2);
    output.emplace_back(&v2);
}