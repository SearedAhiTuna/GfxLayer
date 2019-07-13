
#include "Model.h"
#include "Shape.h"

#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>

void Model::vert_tf_3d(Vert& v, const mat4& tf)
{
    vec3& pos = v.attribs.at<vec3>(MDL_ATT_POSITION);

    vec4 vec(pos, 1);
    vec = tf * vec;

    pos = vec.xyz;
}

void Model::edge_tf_3d(Edge& e, const mat4& tf)
{
    std::list<Vert*> vs;
    e.adjacent_verts(vs);

    for (Vert* v : vs)
    {
        vert_tf_3d(*v, tf);
    }
}

void Model::face_tf_3d(Face& f, const mat4& tf)
{
    std::list<Vert*> vs;
    f.adjacent_verts(vs);

    for (Vert* v : vs)
    {
        vert_tf_3d(*v, tf);
    }
}

Shape* Model::generate_shape(const GLenum& mode)
{
    Shape* shape = new Shape(mode);
    generate_shape(*shape, mode);
    return shape;
}

void Model::generate_vert_normals()
{
    for (Vert& v : verts)
    {
        const vec3& pos = v.attribs.at<vec3>(MDL_ATT_POSITION);
        vec3& norm = v.attribs.at<vec3>(MDL_ATT_NORMAL);

        std::list<Vert*> adj;
        v.adjacent_verts(adj);

        // Get normal facing outwards from the corner
        for (Vert* pv : adj)
        {
            norm += pv->attribs.at<vec3>(MDL_ATT_POSITION) - pos;
        }
        norm = normalize(-norm);
    }
}

void Model::generate_face_normals()
{
    _useFaceNormals = true;
    _normals.clear();

    for (Face& f : faces)
    {
        std::list<Vert*> adj;
        f.adjacent_verts(adj);

        const vec3& v1 = adj.front()->attribs.at<vec3>(MDL_ATT_POSITION);
        const vec3& v2 = (*++adj.begin())->attribs.at<vec3>(MDL_ATT_POSITION);
        const vec3& v3 = adj.back()->attribs.at<vec3>(MDL_ATT_POSITION);

        vec3 d1 = v2 - v1;
        vec3 d2 = v3 - v1;

        _normals[&f] = normalize(cross(d1, d2));
    }
}

#define EMPLACE_MULTIPLE(LIST, SRC, AMOUNT) \
    { for (int j = 0; j < (AMOUNT); ++j) (LIST).emplace_back((SRC)[j]); }

#define MAX(A,B) ((A) > (B) ? (A) : (B))

void Model::generate_shape(Shape& shape, const GLenum& mode)
{
    if (!(faces.begin() != faces.end()))
    {
        // No faces
        // Do nothing
        return;
    }

    // Make space for all attribute buffers
    std::map<size_t, std::list<GLfloat>> values;
    std::map<size_t, size_t> sizes;

    // For each face
    for (Face& f : faces)
    {
        // Get all vertices in the face
        std::vector<Vert*> vs;
        f.adjacent_verts(vs);

        // Construct the face out of triangles
        for (size_t v = 2; v < vs.size(); ++v)
        {
            // Three vertices in the triangle
            Vert* triangle[] = { vs[0], vs[v - 1], vs[v] };
            Vert& v1 = *triangle[0];

            size_t nattr = !_useFaceNormals ? v1.attribs.size() : MAX(v1.attribs.size(), MDL_ATT_NORMAL + 1);

            // For each attribute
            for (size_t i = 0; i < nattr; ++i)
            {
                // If this is the normal attribute,
                // and the model is set to use face normals instead,
                // use the face normal
                if (i == MDL_ATT_NORMAL && _useFaceNormals)
                {
                    // Update the size if needed
                    if (sizes.count(i) == 0)
                        sizes[i] = 3;

                    // Use the normal for the face
                    vec3 norm = _normals[&f];

                    // Add each vertex in the triangle
                    for (size_t t = 0; t < 3; ++t)
                    {
                        EMPLACE_MULTIPLE(values[i], norm, 3)
                    }

                    continue;
                }

                // Skip this attribute if it's not present
                if (!v1.attribs.has(i))
                    continue;

                // Update the size if needed
                if (sizes.count(i) == 0)
                {
                    size_t sizeb = v1.attribs.sizeat(i);
                        if (sizeb == sizeof(GLfloat))
                            sizes[i] = 1;
                        else if (sizeb == sizeof(vec2))
                            sizes[i] = 2;
                        else if (sizeb == sizeof(vec3))
                            sizes[i] = 3;
                        else if (sizeb == sizeof(vec4))
                            sizes[i] = 4;
                        else
                            sizes[i] = 0;
                }
                
                // Skip the attribute if it is invalid
                size_t size = sizes[i];
                if (size == 0)
                    continue;

                // Add each vertex in the triangle
                for (size_t t = 0; t < 3; ++t)
                {
                    if (size == 1)
                        values[i].emplace_back(triangle[t]->attribs.at<GLfloat>(i));
                    else if (size == 2)
                        EMPLACE_MULTIPLE(values[i], triangle[t]->attribs.at<vec2>(i), 2)
                    else if (size == 3)
                        EMPLACE_MULTIPLE(values[i], triangle[t]->attribs.at<vec3>(i), 3)
                    else if (size == 4)
                        EMPLACE_MULTIPLE(values[i], triangle[t]->attribs.at<vec4>(i), 4)
                }
            }
        }
    }

    // Get the number of vertices and attributes
    size_t nverts = 0;
    size_t nattr = 0;
    for (const auto& pair : values)
    {
        std::cout << "(" << pair.first << "," << (pair.second.size() / sizes[pair.first]) << ")\n";

        if (pair.first >= nattr)
            nattr = pair.first + 1;
        
        if (pair.second.size() / sizes[pair.first] > nverts)
            nverts = pair.second.size() / sizes[pair.first];
    }
    
    // Resize the provided shape
    shape = Shape(nverts, mode);

    // Add the vertices to the shape
    for (size_t i = 0; i < nattr; ++i)
    {
        if (values.count(i))
        {
            std::cout << "Writing " << i << "\n";

            for (const GLfloat& v : values[i])
            {
                std::cout << " " << v;
            }
            std::cout << "\n";

            // Get the dimensions for this attribute
            size_t ndims = sizes[i];

            // If the attribute is invalid
            if (ndims == 0)
                continue;

            // Write the buffer
            auto buf = shape.GenBuffer(ndims);
            buf.WriteRangeRaw(values[i]);
        }
        else if (i == MDL_ATT_POSITION)
        {
            // Generate an empty position buffer
            auto buf = shape.GenBuffer(3);
            buf.Clear(0);
        }
        else if (i == MDL_ATT_UV)
        {
            // Generate an empty position buffer
            auto buf = shape.GenBuffer(2);
            buf.Clear(0);
        }
        else if (i == MDL_ATT_NORMAL)
        {
            // Generate an empty position buffer
            auto buf = shape.GenBuffer(3);
            buf.Clear(0);
        }
    }
}

template <typename vec>
struct VecMapper
{
    std::map<void*, std::pair<size_t, vec>> base;
    size_t num = 0;

    bool add(void* v, const vec& val, const GLfloat& merge = -1)
    {
        if (base.count(v) != 0)
            return false;

        if (merge >= 0)
        {
            for (auto& topPair : base)
            {
                auto& pair = topPair.second;

                vec disp = pair.second - val;

                if (glm::length(disp) <= merge)
                {
                    std::cout << "Found one\n";
                    base.emplace(v, pair);
                    return false;
                }
            }
        }

        base.emplace(v, std::pair(++num, val));
        return true;
    }

    size_t get(void* v)
    {
        if (base.count(v) != 0)
            return base[v].first;
        else
            return 0;
    }
};

void Model::export_obj(std::ostream& os, const GLfloat& merge)
{
    VecMapper<vec3> posMap;
    VecMapper<vec2> uvMap;
    VecMapper<vec3> normMap;

    for (Vert& v : verts)
    {
        vec3 pos = v.attribs.at<vec3>(MDL_ATT_POSITION);
        if (posMap.add(&v, pos, merge))
        {
            os << "v " << pos.x << " " << pos.y << " " << pos.z << std::endl;
        }

        if (v.attribs.has(MDL_ATT_UV))
        {
            vec2 uv = v.attribs.at<vec2>(MDL_ATT_UV);
            if (uvMap.add(&v, uv))
            {
                os << "vt " << uv.x << " " << uv.y << std::endl;
            }
        }

        if (!_useFaceNormals && v.attribs.has(MDL_ATT_NORMAL))
        {
            vec3 norm = v.attribs.at<vec3>(MDL_ATT_NORMAL);
            if (normMap.add(&v, norm))
            {
                os << "vn " << norm.x << " " << norm.y << " " << norm.z << std::endl;
            }
        }
    }

    if (_useFaceNormals)
    {
        for (Face& f : faces)
        {
            vec3 norm = _normals[&f];
            if (normMap.add(&f, norm))
            {
                os << "vn " << norm.x << " " << norm.y << " " << norm.z << std::endl;
            }
        }
    }

    for (Face& f : faces)
    {
        os << "f";

        std::list<Vert*> vs;
        f.adjacent_verts(vs);

        std::unordered_set<size_t> usedVs;

        for (Vert* v : vs)
        {
            size_t pos = posMap.get(v);

            if (usedVs.count(pos))
                continue;
            else
                usedVs.emplace(pos);

            size_t uv = uvMap.get(v);
            size_t norm;
            if (_useFaceNormals)
                norm = normMap.get(&f);
            else
                norm = normMap.get(v);

            os << " " << pos;
            if (uv != 0 || norm != 0)
                os << "/";
            if (uv != 0)
                os << uv;
            if (norm != 0)
                os << "/" << norm;
        }

        os << std::endl;
    }
}

void Model::import_obj(std::istream& is)
{
}