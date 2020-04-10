
#include "Arc.h"

#include <glm/gtx/projection.hpp>
#include <iostream>

#define RADIUS x
#define ANGLE y

#define TF(MAT4, VEC3) ((MAT4 * vec4(VEC3, 1)).xyz)

#define COUT_VEC3(X) "(" << X.x << "," << X.y << "," << X.z << ")"

#define LENGTH_ALONG(V1, V2) (dot(V1, normalize(V2)))

static vec3 angle_between(const vec3& v1, const vec3& v2)
{
    GLfloat num = dot(v1, v2);
    GLfloat denom = length(v1) * length(v2);

    GLfloat cosTheta = num / denom;
    GLfloat theta = acosf(cosTheta);

    vec3 crossProd = cross(v1, v2);

    return theta * normalize(crossProd);
}

// Statics used to speed up rect_to_cyl
static vec3 prevAxisZ = vec3(0, 0, 1);
static vec3 axisR = vec3(1, 0, 0);

static vec3 rect_to_cyl(const vec3& point, const vec3& axisZ)
{
    if (axisZ != prevAxisZ)
    {
        // Arbitrarily generate a vector for 0 rotation
        vec3 arb(1, 0, 0);
        if (axisZ.y == 0 && axisZ.z == 0)  // Must be a different direction than axisZ
            arb = vec3(0, 1, 0);

        // Vector orthogonal to axisZ
        axisR = normalize(arb - proj(arb, axisZ));

        // Keep track for next time
        prevAxisZ = axisZ;

        //std::cout << "AXIS Z = " << COUT_VEC3(axisZ) << std::endl;
        //std::cout << "AXIS R = " << COUT_VEC3(axisR) << std::endl;
    }

    // Get tangent (along z axis) and normal (perp to z axis)
    vec3 tanP = proj(point, axisZ);
    vec3 normP = point - tanP;

    // Get r and z
    GLfloat r = length(normP);
    GLfloat z = LENGTH_ALONG(tanP, axisZ);

    if (r == 0)
    {
        return vec3(0, 0, z);
    }

    // Get I (along r axis) and Q (perp to r axis) vectors
    vec3 vecI = proj(normP, axisR);
    vec3 vecQ = normP - vecI;

    // Get I
    GLfloat I = LENGTH_ALONG(vecI, axisR) / length(normP);

    // Get Q
    GLfloat Q = length(vecQ) / length(normP);

    vec3 turnDir = cross(axisR, vecQ);
    if (LENGTH_ALONG(turnDir, axisZ) < 0)
        Q *= -1;

    // Get angle
    GLfloat angle = atan2f(Q, I);

    return vec3(r, angle, z);
}

static vec3 cyl_to_rect(const vec3& point, const vec3& axisZ)
{
    if (axisZ != prevAxisZ)
    {
        // Arbitrarily generate a vector for 0 rotation
        vec3 arb(1, 0, 0);
        if (axisZ.y == 0 && axisZ.z == 0)  // Must be a different direction than axisZ
            arb = vec3(0, 1, 0);

        // Vector orthogonal to axisZ
        axisR = normalize(arb - proj(arb, axisZ));

        // Keep track for next time
        prevAxisZ = axisZ;
    }

    // Get tangent (along z axis)
    vec3 tanP = normalize(axisZ) * point.z;

    // Get normal (perp to z axis)
    vec3 normP = axisR * point.RADIUS;

    mat4 rot = rotate(point.ANGLE, axisZ);
    normP = (rot * vec4(normP, 1)).xyz;

    return tanP + normP;
}

Arc::Arc(FunctionR1R3* func, GLfloat t0, GLfloat tf):
    _func(func),
    _t0(t0),
    _tf(tf)
{
}

Arc::~Arc()
{
    if (_func)
        delete _func;
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

void Arc::connect_verts_rotate(Model& m, Model::Vert& v1, Model::Vert& v2,
                               const size_t& res, const vec3& axis, std::list<Model::Vert*>& output)
{
    //std::cout << "---connect_verts_rotate---\n";

    // Get displacement of connector function
    vec3 myStart = (*_func)(_t0);
    vec3 myEnd = (*_func)(_tf);
    vec3 myDisp = myEnd - myStart;
    vec3 myDispCyl = rect_to_cyl(myDisp, axis);

    //std::cout << "myDisp=" << COUT_VEC3(myDisp) << std::endl;
    //std::cout << "myDispCyl=R" << COUT_VEC3(myDispCyl) << std::endl;

    // Get displacement from source to destination
    vec3 start = v1.attribs.at<vec3>(MDL_ATT_POSITION);
    vec3 end = v2.attribs.at<vec3>(MDL_ATT_POSITION);
    vec3 disp = end - start;
    vec3 dispCyl = rect_to_cyl(disp, axis);

    //std::cout << "disp=" << COUT_VEC3(disp) << std::endl;
    //std::cout << "dispCyl=R" << COUT_VEC3(dispCyl) << std::endl;

    // Get transformation in cylindrical coords
    GLfloat sR = dispCyl.RADIUS / myDispCyl.RADIUS;
    GLfloat dAngle = dispCyl.ANGLE - myDispCyl.ANGLE;
    GLfloat sZ = dispCyl.z / myDispCyl.z;

    //std::cout << "sR=" << sR << std::endl;
    //std::cout << "dAngle=" << dAngle << std::endl;
    //std::cout << "sZ=" << sZ << std::endl;

    mat4 tfRotCyl = translate(vec3(0, dAngle, 0));
    mat4 tfScaleCyl = scale(vec3(sR, 1, sZ));
    mat4 tfCyl = tfRotCyl * tfScaleCyl;

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

        // Get the vertex position
        vec3 valRect = (*_func)(t);
        //std::cout << "val: " << COUT_VEC3(valRect);
        valRect -= myStart;
        //std::cout << " -> " << COUT_VEC3(valRect);

        vec4 valCyl = vec4(rect_to_cyl(valRect, axis), 1);
        //std::cout << " -> R" << COUT_VEC3(valCyl);
        valCyl = tfCyl * valCyl;
        //std::cout << " -> R" << COUT_VEC3(valCyl);

        valRect = cyl_to_rect(valCyl.xyz, axis);
        //std::cout << " -> " << COUT_VEC3(valRect);
        valRect += start;
        //std::cout << " -> " << COUT_VEC3(valRect) << std::endl;

        // Set the vertex position
        cur->attribs.at<vec3>(MDL_ATT_POSITION) = valRect;

        // Set the current as previous
        prev = cur;
    }

    // Connect the last two vertices
    m.edges.emplace(*prev, v2);
    output.emplace_back(&v2);
}
