
#include "Model.h"
#include "Shape.h"
#include "vecx.h"

#include <list>
#include <map>

Shape* Model::generate_shape(const GLenum& mode)
{
    Shape* shape = new Shape(mode);
    generate_shape(*shape, mode);
    return shape;
}

void Model::generate_shape(Shape& shape, const GLenum& mode)
{
    // Get every attribute ID
    std::vector<AttributeID> attrs;
    verts().allAtt(attrs);

    // Create containers to hold all values
    std::vector<std::list<vecx>> values(attrs.size());

    // For each face
    for (Face& f : faces())
    {
        // Get all vertices in the face
        std::vector<Vert*> vs;
        f.adjacent_verts(vs);

        std::cout << "Verts in order: ";
        for (Vert* v : vs)
            std::cout << " " << *v;
        std::cout << "\n";

        // Construct the face out of triangles
        for (size_t v = 2; v < vs.size(); ++v)
        {
            for (size_t i = 0; i < attrs.size(); ++i)
            {
                AttributeID id = attrs[i];

                values[i].emplace_back(vs[0]->getAtt(id));
                values[i].emplace_back(vs[v - 1]->getAtt(id));
                values[i].emplace_back(vs[v]->getAtt(id));
            }
        }
    }

    // Get the number of vertices
    size_t nverts;
    if (attrs.empty())
        nverts = 0;
    else
        nverts = values.front().size();
    
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