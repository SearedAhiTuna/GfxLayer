
#pragma once

#include "unique_container.h"

#include <any>
#include <list>
#include <map>
#include <memory>
#include <ostream>
#include <unordered_set>
#include <vector>

#include <iostream>

#define PTR_VECTOR(TYPE) std::vector<std::shared_ptr<TYPE>>
#define PTR_VECTOR_ITERATOR(TYPE) PtrIterator<PTR_VECTOR(TYPE)::iterator, TYPE>
#define PTR_VECTOR_CONST_ITERATOR(TYPE) PtrIterator<PTR_VECTOR(TYPE)::const_iterator, TYPE>

#define Verts(...) std::list<Mesh::Vert*>({ __VA_ARGS__ })
#define Edges(...) std::list<Mesh::Edge*>({ __VA_ARGS__ })
#define Faces(...) std::list<Mesh::Face*>({ __VA_ARGS__ })

class Mesh
{
public:
    template <typename BaseIterator, typename Type>
    class PtrIterator
    {
    public:
        PtrIterator(BaseIterator it);

        Type& operator*();
        Type* operator->();

        PtrIterator& operator++();

        bool operator!=(const PtrIterator& other);

    private:
        BaseIterator _it;
    };

public:
    typedef size_t AttributeID;

public:
    class Vert;
    class VertList;
    class Edge;
    class EdgeList;
    class Face;
    class FaceList;

    class Vert
    {
    private:
        Vert(const size_t& index, Mesh& mesh);

    public:
        Vert& operator=(const Vert&) = delete;

        bool operator==(const Vert& other) const;
        bool operator!=(const Vert& other) const;

        bool adjacent_to(const Vert& v) const;
        bool adjacent_to(const Edge& e) const;
        bool adjacent_to(const Face& f) const;

        template <typename VertsOut>
        void adjacent_verts(VertsOut& verts);

        template <typename EdgesOut>
        void adjacent_edges(EdgesOut& edges);

        template <typename FacesOut>
        void adjacent_faces(FacesOut& faces);

        Vert& extrude();

        template <typename Type>
        Vert& setAtt(const AttributeID& id, const Type& t);

        template <typename Type>
        Type getAtt(const AttributeID& id) const;

        const std::any getAtt(const AttributeID& id) const;

        std::ostream& print(std::ostream& out) const
        {
            out << "v" << _index;
            return out;
        }

    private:
        friend class Mesh;
        friend class Edge;
        friend class Face;
        friend class VertList;
        friend class EdgeList;
        friend class FaceList;

    private:
        size_t _index;
        Mesh& _mesh;

        std::unordered_set<Edge*> _edges;

        std::map<AttributeID, std::any> attribs;
    };

    class VertList
    {
    private:
        VertList(Mesh& Mesh);

    public:
        VertList& operator=(const VertList&) = delete;

        Vert& operator[](const size_t& ind);
        const Vert& operator[](const size_t& ind) const;

        Vert& emplace();

        template <typename Type>
        Vert& emplace(const Type& attrib);

        template <typename Type, typename ... Types>
        Vert& emplace(const Type& attrib, const Types& ... attribs);

    private:
        template <typename Type>
        Vert& emplace_int(Vert& v, const size_t& depth, const Type& attrib);

        template <typename Type, typename ... Types>
        Vert& emplace_int(Vert& v, const size_t& depth, const Type& attrib, const Types& ... attribs);

    public:
        template <typename EdgesIn, typename VertsOut>
        void within_edges(const EdgesIn& input, VertsOut& output);  // Undefined

        template <typename FacesIn, typename VertsOut>
        void within_faces(const FacesIn& input, VertsOut& output);  // Undefined

        PTR_VECTOR_ITERATOR(Vert) begin();
        PTR_VECTOR_ITERATOR(Vert) end();

        PTR_VECTOR_CONST_ITERATOR(Vert) begin() const;
        PTR_VECTOR_CONST_ITERATOR(Vert) end() const;

        Vert& extrude(Vert& v);
        
        template <typename VertsIn, typename VertsOut>
        void extrude(const VertsIn& input, VertsOut& output);

        template <typename AttrsOut>
        void allAtt(AttrsOut& attrs) const;

    private:
        friend class Mesh;
    
    private:
        Mesh& _mesh;
        PTR_VECTOR(Vert) _verts;
        std::list<AttributeID> _attrs;
    };

    class Edge
    {
    private:
        Edge(const size_t& index, Mesh& mesh, Vert& v1, Vert& v2);

    public:
        Edge& operator=(const Edge&) = delete;

        bool operator==(const Edge& other) const;
        bool operator!=(const Edge& other) const;

        bool adjacent_to(const Vert& v) const;
        bool adjacent_to(const Edge& e) const;
        bool adjacent_to(const Face& f) const;
        bool directly_adjacent_to(const Face& f) const;

        template <typename VertsOut>
        void adjacent_verts(VertsOut& verts);

        template <typename EdgesOut>
        void adjacent_edges(EdgesOut& edges);

        template <typename FacesOut>
        void adjacent_faces(FacesOut& faces);

        template <typename FacesOut>
        void directly_adjacent_faces(FacesOut& faces);

        Vert& opposite_vert(const Vert& v);
        Vert& opposite_vert(const Edge& e);

        Face* opposite_face(const Face& f);

        Edge& extrude();

        std::ostream& print(std::ostream& out) const
        {
            out << "e" << _index;
            return out;
        }
    
    private:
        friend class Mesh;
        friend class Vert;
        friend class Face;
        friend class VertList;
        friend class EdgeList;
        friend class FaceList;
    
    private:
        size_t _index;
        Mesh& _mesh;

        Vert* _v1;
        Vert* _v2;

        std::vector<Face*> _faces;
    };

    class EdgeList
    {
    private:
        EdgeList(Mesh& mesh);

    public:
        EdgeList& operator=(const EdgeList&) = delete;

        Edge& operator[](const size_t& ind);
        const Edge& operator[](const size_t& ind) const;

        Edge& emplace(Vert& v1, Vert& v2);

        Edge* between(const Vert& v1, const Vert& v2);
        Edge* between(const Edge& e1, const Edge& e2);

        template <typename EdgesIn, typename EdgesOut>
        void sort_by_adjacency(const EdgesIn& input, EdgesOut& output);

        PTR_VECTOR_ITERATOR(Edge) begin();
        PTR_VECTOR_ITERATOR(Edge) end();

        PTR_VECTOR_CONST_ITERATOR(Edge) begin() const;
        PTR_VECTOR_CONST_ITERATOR(Edge) end() const;

        Edge& extrude(Edge& e);

        template <typename EdgesIn, typename EdgesOut>
        void extrude(const EdgesIn& input, EdgesOut& output);

    private:
        friend class Mesh;

    private:
        Mesh& _mesh;
        PTR_VECTOR(Edge) _edges;
    };

    class Face
    {
    private:
        Face(const size_t& index, Mesh& mesh);

        template <typename VertsIn>
        void add_verts(const VertsIn& verts);

        template <typename EdgesIn>
        void add_edges(const EdgesIn& edges);

    public:
        Face& operator=(const Face&) = delete;

        bool operator==(const Face& other) const;
        bool operator!=(const Face& other) const;

        bool adjacent_to(const Vert& v) const;
        bool adjacent_to(const Edge& e) const;
        bool adjacent_to(const Face& f) const;
        bool directly_adjacent_to(const Face& f) const;

        template <typename VertsOut>
        void adjacent_verts(VertsOut& verts);

        template <typename EdgesOut>
        void adjacent_edges(EdgesOut& edges);

        template <typename EdgesOut>
        void directly_adjacent_edges(EdgesOut& edges);

        template <typename FacesOut>
        void adjacent_faces(FacesOut& faces);

        template <typename FacesOut>
        void directly_adjacent_faces(FacesOut& faces);

        std::ostream& print(std::ostream& out) const
        {
            out << "f" << _index;
            return out;
        }

    private:
        friend class Mesh;
        friend class Vert;
        friend class Edge;
        friend class VertList;
        friend class EdgeList;
        friend class FaceList;

    private:
        size_t _index;
        Mesh& _mesh;

        std::list<Edge*> _edges;
    };

    class FaceList
    {
    private:
        FaceList(Mesh& mesh);

    public:
        FaceList& operator=(const FaceList&) = delete;

        Face& operator[](const size_t& ind);
        const Face& operator[](const size_t& ind) const;

        template <class VertsIn>
        Face& emplace_verts(const VertsIn& verts);

        template <class EdgesIn>
        Face& emplace_edges(const EdgesIn& edges);

        template <class VertsIn>
        Face* between_verts(const VertsIn& verts);  // Undefined

        template <class EdgesIn>
        Face* between_edges(const EdgesIn& edges);  // Undefined

        PTR_VECTOR_ITERATOR(Face) begin();
        PTR_VECTOR_ITERATOR(Face) end();

        PTR_VECTOR_CONST_ITERATOR(Face) begin() const;
        PTR_VECTOR_CONST_ITERATOR(Face) end() const;

        Face& extrude(Face& f);  // Undefined

        template <typename FacesIn, typename FacesOut>
        void extrude(const FacesOut& input, FacesOut& output);  // Undefined

    private:
        friend class Mesh;

    private:
        Mesh& _mesh;
        PTR_VECTOR(Face) _faces;
    };

public:
    Mesh();
    Mesh(const Mesh* other);  // Undefined
    virtual ~Mesh();

    Mesh& operator=(const Mesh& other);  // Undefined

    VertList& verts();
    const VertList& verts() const;

    EdgeList& edges();
    const EdgeList& edges() const;

    FaceList& faces();
    const FaceList& faces() const;

    Vert& vert(const size_t& ind);
    Edge& edge(const size_t& ind);
    Face& face(const size_t& ind);

    virtual std::ostream& print(std::ostream& out) const;
    void print_verbose(std::ostream& out);

private:
    VertList _verts;
    EdgeList _edges;
    FaceList _faces;
};

std::ostream& operator<<(std::ostream& out, const Mesh::Vert& v);
std::ostream& operator<<(std::ostream& out, const Mesh::Edge& e);
std::ostream& operator<<(std::ostream& out, const Mesh::Face& f);
std::ostream& operator<<(std::ostream& out, const Mesh& m);

template <typename BaseIterator, typename Type>
Mesh::PtrIterator<BaseIterator, Type>::PtrIterator(BaseIterator it):
    _it(it)
{
}

template <typename BaseIterator, typename Type>
Type& Mesh::PtrIterator<BaseIterator, Type>::operator*()
{
    return *_it->get();
}

template <typename BaseIterator, typename Type>
Type* Mesh::PtrIterator<BaseIterator, Type>::operator->()
{
    return _it->get();
}

template <typename BaseIterator, typename Type>
Mesh::PtrIterator<BaseIterator, Type>& Mesh::PtrIterator<BaseIterator, Type>::operator++()
{
    ++_it;
    return *this;
}

template <typename BaseIterator, typename Type>
bool Mesh::PtrIterator<BaseIterator, Type>::operator!=(const PtrIterator& other)
{
    return _it != other._it;
}

template <typename VertsOut>
void Mesh::Vert::adjacent_verts(VertsOut& verts)
{
    for (Edge* e : _edges)
    {
        verts.emplace_back(&e->opposite_vert(*this));
    }
}

template <typename EdgesOut>
void Mesh::Vert::adjacent_edges(EdgesOut& edges)
{
    for (Edge* e : _edges)
    {
        edges.emplace_back(e);
    }
}

template <typename FacesOut>
void Mesh::Vert::adjacent_faces(FacesOut& faces)
{
    unique_container<std::list<Face*>, Face*> temp_list;

    for (Edge* e : _edges)
    {
        e->directly_adjacent_faces(temp_list);
    }

    for (Face* f : temp_list)
    {
        faces.emplace_back(f);
    }
}

template <typename Type>
Mesh::Vert& Mesh::Vert::setAtt(const AttributeID& id, const Type& t)
{
    attribs[id] = t;

    bool exists = false;
    for (const AttributeID& i : _mesh._verts._attrs)
    {
        if (i == id)
            exists = true;
    }

    if (!exists)
    {
        _mesh._verts._attrs.emplace_back(id);
    }

    return *this;
}

template <typename Type>
Type Mesh::Vert::getAtt(const AttributeID& id) const
{
    if (attribs.count(id) == 0)
        return Type();
    else
        return std::any_cast<Type>(attribs.at(id));
}

template <typename Type>
Mesh::Vert& Mesh::VertList::emplace(const Type& attrib)
{
    Vert& v = emplace();
    v.setAtt(0, attrib);

    return v;
}

template <typename Type, typename ... Types>
Mesh::Vert& Mesh::VertList::emplace(const Type& attrib, const Types& ... attribs)
{
    Vert& v = emplace();
    v.setAtt(0, attrib);

    return emplace_int(v, 1, attribs...);
}

template <typename Type>
Mesh::Vert& Mesh::VertList::emplace_int(Vert& v, const size_t& depth, const Type& attrib)
{
    v.setAtt(depth, attrib);

    return v;
}

template <typename Type, typename ... Types>
Mesh::Vert& Mesh::VertList::emplace_int(Vert& v, const size_t& depth, const Type& attrib, const Types& ... attribs)
{
    v.setAtt(depth, attrib);

    return emplace_int(v, depth + 1, attribs...);
}

template <typename EdgesIn, typename VertsOut>
void Mesh::VertList::within_edges(const EdgesIn& input, VertsOut& output)
{
    unique_container<std::list<Vert*>, Vert*> temp_list;

    Edge* prev = nullptr;
    bool newline = true;
    for (Edge* e : input)
    {
        if (prev == nullptr || !e->adjacent_to(*prev))
        {
            newline = true;
        }
        else if (newline)
        {
            newline = false;
            if (temp_list.back() != e->_v1 && temp_list.back() != e->_v2)
            {
                // Switch the previous two entries
                Vert* v1 = temp_list.back();
                temp_list.pop_back();

                Vert* v2 = temp_list.back();
                temp_list.pop_back();

                temp_list.emplace_back(v1);
                temp_list.emplace_back(v2);
            }
        }

        e->adjacent_verts(temp_list);
        prev = e;
    }

    for (Vert* v : temp_list)
    {
        output.emplace_back(v);
    }
}

template <typename FacesIn, typename VertsOut>
void Mesh::VertList::within_faces(const FacesIn& input, VertsOut& output)
{
    unique_container<std::list<Vert*>, Vert*> temp_list;

    for (Face* f : input)
    {
        f->adjacent_verts(temp_list);
    }

    for (Vert* v : temp_list)
    {
        output.emplace_back(v);
    }
}

template <typename VertsIn, typename VertsOut>
void Mesh::VertList::extrude(const VertsIn& input, VertsOut& output)
{
    for (Vert* v : input)
    {
        output.emplace_back(&extrude(*v));
    }
}

template <typename AttrsOut>
void Mesh::VertList::allAtt(AttrsOut& attrs) const
{
    for (const AttributeID& id : _attrs)
    {
        attrs.emplace_back(id);
    }
}

template <typename VertsOut>
void Mesh::Edge::adjacent_verts(VertsOut& verts)
{
    verts.emplace_back(_v1);
    verts.emplace_back(_v2);
}

template <typename EdgesOut>
void Mesh::Edge::adjacent_edges(EdgesOut& edges)
{
    std::list<Edge*> temp;

    _v1->adjacent_edges(temp);
    _v2->adjacent_edges(temp);

    for (Edge* e : temp)
    {
        if (*e != *this)
            edges.emplace_back(e);
    }
}

template <typename FacesOut>
void Mesh::Edge::adjacent_faces(FacesOut& faces)
{
    unique_container<std::list<Face*>, Face*> temp_list;

    _v1->adjacent_faces(temp_list);
    _v2->adjacent_faces(temp_list);

    for (Face* f : temp_list)
    {
        faces.emplace_back(f);
    }

}

template <typename FacesOut>
void Mesh::Edge::directly_adjacent_faces(FacesOut& faces)
{
    for (Face* f : _faces)
    {
        faces.emplace_back(f);
    }
}

template <typename EdgesIn, typename EdgesOut>
void Mesh::EdgeList::sort_by_adjacency(const EdgesIn& input, EdgesOut& output)
{
    // Move input to a linked list
    std::list<Edge*> edges;
    for (Edge* e : input)
    {
        edges.emplace_back(e);
    }
    
    // Until all edges have been sorted
    while (!edges.empty())
    {
        // Add the first edge to the list unconditionally
        output.emplace_back(edges.front());
        edges.pop_front();

        // Beginning of this contiguous series
        auto begin = --output.end();
        Edge* front = output.back();

        // Loop until unable to add an edge
        bool found = false;
        do
        {
            found = false;

            Edge* back = output.back();

            // For each input edge
            for (auto it = edges.begin(); it != edges.end(); ++it)
            {
                Edge* e = *it;

                if (e->adjacent_to(*back))
                {
                    // Add to the back
                    output.emplace_back(e);
                    edges.erase(it);
                    found = true;
                    break;
                }
                else if (e->adjacent_to(*front))
                {
                    // Add to the front
                    begin = output.emplace(begin, e);
                    front = e;
                    edges.erase(it);
                    found = true;
                    break;
                }
                else
                {
                    std::cout << "No adjacent edges found\n";
                }
                
            }
        }
        while (found && !edges.empty());
    }
}

template <typename EdgesIn, typename EdgesOut>
void Mesh::EdgeList::extrude(const EdgesIn& input, EdgesOut& output)
{
    // Sort input so adjacent edges are next to each other
    std::list<Edge*> es;
    sort_by_adjacency(input, es);

    // Make ordered list of every existing vertex
    // and every extruded vertex
    std::list<Vert*> vs;
    std::list<Vert*> nvs;

    // Extrude every vertex
    Edge* prev;
    if (es.size() == 2)
        prev = nullptr;
    else
        prev = es.back();
    for (Edge* e : es)
    {
        if (e == nullptr || !e->adjacent_to(*prev))
        {
            // If this edge is not connected to the previous edge,
            // add both vertices
            vs.emplace_back(e->_v1);
            vs.emplace_back(e->_v2);

            // Extrude both vertices
            Vert* nv1 = &e->_v1->extrude();
            nvs.emplace_back(nv1);

            Vert* nv2 = &e->_v2->extrude();
            nvs.emplace_back(nv2);
        }
        else
        {
            // If this edge is connected to the previous edge,
            // add the vertex not shared by the previous edge
            Vert* v = &e->opposite_vert(*prev);
            vs.emplace_back(v);

            // Extrude the vertex
            Vert* nv = &v->extrude();
            nvs.emplace_back(nv);
        }

        prev = e;
    }

    // Sanity check
    if (vs.size() != nvs.size())
    {
        std::cerr << "Mismatched sizes in extrude\n";
    }

    std::cout << "Sorted list of verts:\n";
    for (Vert* v : vs)
        std::cout << " " << *v;
    std::cout << "\n";

    std::cout << "Sorted list of new verts:\n";
    for (Vert* v : nvs)
        std::cout << " " << *v;
    std::cout << "\n";

    // Fill the spaces
    Vert* vPrev = vs.back();
    Vert* nvPrev = nvs.back();
    for (auto vIt = vs.begin(), nvIt = nvs.begin(); vIt != vs.end(); ++vIt, ++nvIt)
    {
        Vert* v = *vIt;
        Vert* nv = *nvIt;

        if (v->adjacent_to(*vPrev))
        {
            // If this vertex is connected to the previous vertex,
            // fill in the space

            // Connect the new vertices with an edge
            Edge* ne = &emplace(*nvPrev, *nv);

            // Connect all the vertices with a face
            _mesh.faces().emplace_verts(Verts(nvPrev, nv, v, vPrev));

            // Add the connecting edge to the output
            output.emplace_back(ne);
        }

        vPrev = v;
        nvPrev = nv;
    }
}

template <typename VertsIn>
void Mesh::Face::add_verts(const VertsIn& verts)
{
    std::list<Edge*> edges;

    Vert* prev = verts.back();
    for (Vert* v : verts)
    {
        Edge* conn = _mesh.edges().between(*v, *prev);
        if (!conn)
        {
            conn = &_mesh.edges().emplace(*v, *prev);
        }
        
        edges.emplace_back(conn);

        prev = v;
    }

    add_edges(edges);
}

template <typename EdgesIn>
void Mesh::Face::add_edges(const EdgesIn& edges)
{
    std::list<Edge*> sorted;
    _mesh.edges().sort_by_adjacency(edges, sorted);

    for (Edge* e : sorted)
    {
        _edges.emplace_back(e);
        e->_faces.emplace_back(this);
    }
}

template <typename VertsOut>
void  Mesh::Face::adjacent_verts(VertsOut& verts)
{
    _mesh.verts().within_edges(_edges, verts);
}

template <typename EdgesOut>
void  Mesh::Face::adjacent_edges(EdgesOut& edges)
{
    unique_container<std::list<Edge*>, Edge*> temp_list;

    for (Edge* e : _edges)
    {
        e->adjacent_edges(temp_list);
    }

    for (Edge* e : temp_list)
    {
        edges.emplace_back(e);
    }
}

template <typename EdgesOut>
void Mesh::Face::directly_adjacent_edges(EdgesOut& edges)
{
    for (Edge* e : _edges)
        edges.emplace_back(e);
}

template <typename FacesOut>
void Mesh::Face::adjacent_faces(FacesOut& faces)
{
    unique_container<std::list<Face*>, Face*> temp_list;

    for (Edge* e : _edges)
    {
        e->adjacent_faces(temp_list);
    }

    for (Face* f : temp_list)
    {
        if (*f != *this)
            faces.emplace_back(f);
    }
}

template <typename FacesOut>
void Mesh::Face::directly_adjacent_faces(FacesOut& faces)
{
    unique_container<std::list<Face*>, Face*> temp_list;

    for (Edge* e : _edges)
    {
        Face* f = e->opposite_face(*this);

        if (f)
        {
            temp_list.emplace_back(f);
        }
    }

    for (Face* f : temp_list)
    {
        faces.emplace_back(f);
    }
}

template <class VertsIn>
Mesh::Face& Mesh::FaceList::emplace_verts(const VertsIn& verts)
{
    Face* pFace = new Face(_faces.size(), _mesh);
    pFace->add_verts(verts);
    _faces.emplace_back(pFace);

    return *pFace;
}

template <class EdgesIn>
Mesh::Face& Mesh::FaceList::emplace_edges(const EdgesIn& edges)
{
    Face* pFace = new Face(_faces.size(), _mesh);
    pFace->add_edges(edges);
    _faces.emplace_back(pFace);

    return *pFace;
}

#undef PTR_VECTOR
#undef PTR_VECTOR_ITERATOR
#undef PTR_VECTOR_CONST_ITERATOR