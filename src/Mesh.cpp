#include "..\inc\Mesh.h"

Mesh::Mesh(NormalGeneration normals):
    mNormals(normals)
{
}

int Mesh::EmplaceVert(const vec3& pos)
{
    mVerts.emplace_back(pos);
    return (int)mVerts.size() - 1;
}

int Mesh::EmplaceFace(const std::list<int>& verts)
{
    // Emplace face
    mFaces.emplace_back();
    int f = (int)mFaces.size() - 1;

    // Add verts
    for (int v : verts)
    {
        // Add to face
        if (v >= 0 && v < mVerts.size())
            mFaces.back().emplace_back(v);

        // Add face to reverse
        mFacesReverse[v].emplace_back(f);
    }

    // Return face index
    return f;
}

int Mesh::ExtrudeVert(int v, glm::mat4 tf)
{
    // New vertex
    int newV = EmplaceVert();
    mVerts[newV] = mVerts[v];

    // Transform
    vec4 pos = vec4(mVerts[v].pos, 1);
    pos = tf * pos;
    mVerts[newV].pos = pos.xyz;

    return newV;
}

void Mesh::ExtrudeVerts(const std::list<int>& verts, glm::mat4 tf, std::list<int>* outVerts)
{
    // Extrude a series of rectangular faces
    int v0, v1, v2, v3;

    // Start with the first vert
    v0 = verts.front();
    v3 = ExtrudeVert(v0, tf);
    if (outVerts)
        outVerts->emplace_back(v3);

    // For each subsequent vert
    for (auto it = ++verts.begin(); it != verts.end(); ++it)
    {
        // Get the next vert
        v1 = *it;

        // Extrude
        v2 = ExtrudeVert(v1, tf);
        if (outVerts)
            outVerts->emplace_back(v2);

        // Fill in with a face (generate normal with right hand rule)
        EmplaceFace({ v0, v1, v2, v3 });

        // Set the previous
        v0 = v1;
        v3 = v2;
    }
}

vec3 Mesh::VertNormal(int v) const
{
    vec3 normal = VEC3_ORIGIN;

    // Add up all adjacent face normals
    for (int f : mFacesReverse.at(v))
    {
        normal += FaceNormal(f);
    }

    // Return normalized
    return glm::normalize(normal);
}

vec3 Mesh::FaceNormal(int f) const
{
    if (mFaces.at(f).size() < 3)  // Not a real face
        return VEC3_ORIGIN;

    // Get three vertices
    auto it = mFaces.at(f).begin();
    int v1 = *(it++);
    int v2 = *(it++);
    int v3 = *(it++);

    // Get the positions
    vec3 p1 = mVerts.at(v1).pos;
    vec3 p2 = mVerts.at(v2).pos;
    vec3 p3 = mVerts.at(v3).pos;

    // Get two sides
    vec3 side1 = p1 - p2;
    vec3 side2 = p3 - p2;

    // Cross product to get the normal
    vec3 normal = glm::cross(side2, side1);

    // Normalize before returning
    return glm::normalize(normal);
}

const vec3& Mesh::VertPos(int v) const
{
    return mVerts.at(v).pos;
}

void Mesh::VertPos(int v, const vec3& pos)
{
    mVerts[v].pos = pos;
}

const vec2& Mesh::VertUV(int v) const
{
    return mVerts.at(v).uv;
}

void Mesh::VertUV(int v, const vec2& uv)
{
    mTextured = true;
    mVerts[v].uv = uv;
}

#define OBJ(X) (X+1)

std::ostream& Mesh::ToObj(std::ostream& out, bool nl) const
{
    out << "# verts";
    for (const auto& v : mVerts)
    {
        out << "\nv " << v.pos.x
            << " " << v.pos.y
            << " " << v.pos.z;
    }

    if (mTextured)
    {
        for (const auto& v : mVerts)
        {
            out << "\nvt " << v.uv.x
                << " " << v.uv.y;
        }
    }

    if (mNormals == NORMALS_PER_FACE)
    {
        out << "\n# normals (per face)";

        for (int f = 0; f < mFaces.size(); ++f)
        {
            vec3 normal = FaceNormal(f);
            out << "\nvn " << normal.x
                << " " << normal.y
                << " " << normal.z;
        }
    }
    else if (mNormals == NORMALS_PER_VERTEX)
    {
        out << "\n# normals (per vertex)";

        for (int v = 0; v < mVerts.size(); ++v)
        {
            vec3 normal = VertNormal(v);
            out << "\nvn " << normal.x
                << " " << normal.y
                << " " << normal.z;
        }
    }

    out << "\n# faces";
    int f = 0;
    for (const auto& verts : mFaces)
    {
        out << "\nf";

        for (int v : verts)
        {
            // Vertex index
            out << " " << OBJ(v);

            if (mTextured || mNormals != NO_NORMALS)
            {
                out << "/";
            }

            if (mTextured)
            {
                out << " " << OBJ(v);
            }

            if (mNormals == NORMALS_PER_FACE)
            {
                out << "/" << OBJ(f);
            }
            else if (mNormals == NORMALS_PER_VERTEX)
            {
                out << "/" << OBJ(v);
            }
        }

        ++f;
    }

    if (nl)
        out << "\n";

    return out;
}

#define EMPLACE_MULTIPLE(LIST, SRC, AMOUNT) \
    { for (int j = 0; j < (AMOUNT); ++j) (LIST).emplace_back((SRC)[j]); }

void Mesh::ToShape(Shape& s, int posAtt, int uvAtt, int normAtt) const
{
    if (mFaces.empty())  // No faces
        return;

    // Make space for all attribute buffers
    std::list<GLfloat> posVals;
    std::list<GLfloat> uvVals;
    std::list<GLfloat> normVals;

    // For each face
    int f = 0;
    for (const auto& vs : mFaces)
    {
        // We need to turn a face into its component triangles
        // We can do this using the pattern
        // face = {0, 1, 2, 3, 4, ...}
        // triangles = {0, 1, 2}, {0, 2, 3}, {0, 3, 4}, ...

        // Triangle vertices
        int v[3];
        v[0] = vs.front();  // First vertex is always used
        v[1] = -1;
        v[2] = -1;

        // Iterate
        for (auto it = ++vs.begin(); it != vs.end(); ++it)
        {
            // Get the next vert
            v[1] = v[2];
            v[2] = *it;

            // If we have a full triangle
            if (v[0] >= 0 && v[1] >= 0 && v[2] >= 0)
            {
                if (posAtt != -1)
                {
                    for (int index : v)  // Add the position of each vert
                    {
                        vec3 pos = VertPos(index);
                        EMPLACE_MULTIPLE(posVals, pos, 3);
                    }
                }

                if (uvAtt != -1)
                {
                    for (int index : v)  // Add the UV of each vert
                    {
                        vec2 uv = VertUV(index);
                        EMPLACE_MULTIPLE(uvVals, uv, 2);
                    }
                }

                if (normAtt != -1)
                {
                    for (int index : v)  // Add the normal of each vert
                    {
                        if (mNormals == NORMALS_PER_FACE)
                        {
                            vec3 norm = FaceNormal(f);
                            EMPLACE_MULTIPLE(normVals, norm, 3);
                        }
                        else if (mNormals == NORMALS_PER_VERTEX)
                        {
                            vec3 norm = VertNormal(index);
                            EMPLACE_MULTIPLE(normVals, norm, 3);
                        }
                        else
                        {
                            EMPLACE_MULTIPLE(normVals, VEC3_ORIGIN, 3);
                        }
                    }
                }
            }
        }

        // Increment f
        ++f;

    }  // for (const auto& vs : mFaces)

    // Resize the provided shape
    s = Shape(posVals.size() / 3, GL_TRIANGLES);

    // Get the total number of attributes for the shape
    int nattr = glm::max(posAtt, glm::max(uvAtt, normAtt)) + 1;

    // For each attribute
    for (int n = 0; n < nattr; ++n)
    {
        if (n == posAtt)
        {
            // Write the position buffer
            auto buf = s.GenBuffer(3);
            buf.WriteRangeRaw(posVals);
        }
        else if (n == uvAtt)
        {
            // Write the UV buffer
            auto buf = s.GenBuffer(2);
            buf.WriteRangeRaw(uvVals);
        }
        else if (n == normAtt)
        {
            // Write the normal buffer
            auto buf = s.GenBuffer(3);
            buf.WriteRangeRaw(normVals);
        }
        else  // Attribute not filled
        {
            // Generate an empty buffer
            auto buf = s.GenBuffer(1);
            buf.Clear(0);
        }
    }
}
