
#include "Mesh.h"

#include <iostream>
#include <list>

Mesh::Attribute::Attribute()
{
}

Mesh::Attribute::Attribute(const Attribute& other)
{
    _size = other._size;
    data = new char[_size];

    memcpy(data, other.data, _size);
}

Mesh::Attribute::~Attribute()
{
    if (data)
        delete[] data;
}

bool Mesh::Attribute::valid() const
{
    return data != nullptr;
}

size_t Mesh::Attribute::size() const
{
    return _size;
}

Mesh::AttribList& Mesh::AttribList::operator=(const AttribList& other)
{
    _attribs.reserve(other.size());
    for (size_t i = 0; i < other.size(); ++i)
    {
        if (other.has(i))
        {
            _attribs.emplace_back(new Attribute(other._attribs.at(i)));
        }
        else
        {
            _attribs.emplace_back(new Attribute());
        }
    }

    return *this;
}

size_t Mesh::AttribList::size() const
{
    return _attribs.size();
}

size_t Mesh::AttribList::sizeat(const size_t& i) const
{
    if (i >= size())
        return 0;

    return _attribs.at(i).size();
}

bool Mesh::AttribList::has(const size_t& i) const
{
    if (i >= size())
        return false;

    return _attribs.at(i).valid();
}

Mesh::Vert::Vert(const size_t& index, Mesh& mesh) :
    _index(index),
    _mesh(mesh)
{
}

bool Mesh::Vert::operator==(const Vert& other) const
{
    return _index == other._index;
}

bool Mesh::Vert::operator!=(const Vert& other) const
{
    return !(*this == other);
}

bool Mesh::Vert::adjacent_to(const Vert& v) const
{
    for (Edge* e : _edges)
    {
        if (e->adjacent_to(v))
            return true;
    }

    return false;
}

bool Mesh::Vert::adjacent_to(const Edge& e) const
{
    for (Edge* _e : _edges)
    {
        if (_e == &e)
            return true;
    }

    return false;
}

bool Mesh::Vert::adjacent_to(const Face& f) const
{
    for (Edge* _e : _edges)
    {
        if (_e->directly_adjacent_to(f))
            return true;
    }

    return false;
}

Mesh::Vert& Mesh::Vert::extrude()
{
    return _mesh.verts.extrude(*this);
}

Mesh::VertList::VertList(Mesh& mesh) :
    _mesh(mesh)
{
}

Mesh::Vert& Mesh::VertList::operator[](const size_t& ind)
{
    return _verts.at(ind);
}

const Mesh::Vert& Mesh::VertList::operator[](const size_t& ind) const
{
    return _verts.at(ind);
}

Mesh::Vert* Mesh::VertList::operator()(const size_t& ind)
{
    return &_verts.at(ind);
}

const Mesh::Vert* Mesh::VertList::operator()(const size_t& ind) const
{
    return &_verts.at(ind);
}

Mesh::Vert& Mesh::VertList::emplace()
{
    Vert* pVert = new Vert(_verts.size(), _mesh);
    _verts.emplace_back(pVert);

    return *pVert;
}

Mesh::Vert& Mesh::VertList::emplace(const Vert& v)
{
    // TODO: insert return statement here
    Vert& nv = emplace();
    for (size_t i = 0; i < v.attribs.size(); ++i)
    {
        if (v.attribs.has(i))
        {
            nv.attribs = v.attribs;
        }
    }

    return nv;
}

REF_VECTOR_ITERATOR(Mesh::Vert) Mesh::VertList::begin()
{
    return _verts.begin();
}

REF_VECTOR_ITERATOR(Mesh::Vert) Mesh::VertList::end()
{
    return _verts.end();
}

REF_VECTOR_CONST_ITERATOR(Mesh::Vert) Mesh::VertList::begin() const
{
    return _verts.cbegin();
}

REF_VECTOR_CONST_ITERATOR(Mesh::Vert) Mesh::VertList::end() const
{
    return _verts.cend();
}

Mesh::Vert& Mesh::VertList::extrude(Vert& v)
{
    // Create a new vertex
    Vert &nv = emplace();

    // Create an edge connecting the two verts
    _mesh.edges.emplace(v, nv);

    // Copy the attributes to the new vertex
    for (size_t i = 0; i < v.attribs.size(); ++i)
    {
        if (v.attribs.has(i))
            nv.attribs._attribs.emplace_back(new Attribute(v.attribs._attribs[i]));
        else
            nv.attribs._attribs.emplace_back();
    }

    // Return the new vertex
    return nv;
}

Mesh::Edge::Edge(const size_t& index, Mesh& mesh, Vert& v1, Vert& v2) :
    _index(index),
    _mesh(mesh),
    _v1(&v1),
    _v2(&v2)
{
}

bool Mesh::Edge::operator==(const Edge& other) const
{
    return _index == other._index;
}

bool Mesh::Edge::operator!=(const Edge& other) const
{
    return !(*this == other);
}

bool Mesh::Edge::adjacent_to(const Vert& v) const
{
    return *_v1 == v || *_v2 == v;
}

bool Mesh::Edge::adjacent_to(const Edge& e) const
{
    return _v1->adjacent_to(e) || _v2->adjacent_to(e);
}

bool Mesh::Edge::adjacent_to(const Face& f) const
{
    return _v1->adjacent_to(f) || _v2->adjacent_to(f);
}

bool Mesh::Edge::directly_adjacent_to(const Face& f) const
{
    for (Face* _f : _faces)
    {
        if (*_f == f)
            return true;
    }

    return false;
}

Mesh::Vert& Mesh::Edge::opposite_vert(const Vert& v)
{
    if (v == *_v1)
        return *_v2;
    else
        return *_v1;
}

Mesh::Vert& Mesh::Edge::opposite_vert(const Edge& e)
{
    if (_v1->adjacent_to(e))
        return *_v2;
    else
        return *_v1;
}

Mesh::Face* Mesh::Edge::opposite_face(const Face& f)
{
    if (_faces.size() != 2)
        return nullptr;
    
    if (f == *_faces[0])
        return _faces[1];
    else if (f == *_faces[1])
        return _faces[0];
    else
        return nullptr;
}

Mesh::Edge& Mesh::Edge::extrude()
{
    return _mesh.edges.extrude(*this);
}

Mesh::EdgeList::EdgeList(Mesh& mesh) :
    _mesh(mesh)
{
}

Mesh::Edge& Mesh::EdgeList::operator[](const size_t& ind)
{
    return _edges.at(ind);
}

const Mesh::Edge& Mesh::EdgeList::operator[](const size_t& ind) const
{
    return _edges.at(ind);
}

Mesh::Edge& Mesh::EdgeList::emplace(Vert& v1, Vert& v2)
{
    // Check if there is already an edge
    Edge* pEdge = between(v1, v2);

    // If there is not an edge, create a new one
    if (!pEdge)
    {
        pEdge = new Edge(_edges.size(), _mesh, v1, v2);
        _edges.emplace_back(pEdge);

        // Connect the vertices
        v1._edges.emplace(pEdge);
        v2._edges.emplace(pEdge);
    }

    return *pEdge;
}

Mesh::Edge* Mesh::EdgeList::between(const Vert& v1, const Vert& v2)
{
    for (Edge* e : v1._edges)
    {
        if (e->opposite_vert(v1) == v2)
        {
            return e;
        }
    }

    return nullptr;
}

Mesh::Edge* Mesh::EdgeList::between(const Edge& e1, const Edge& e2)
{
    Edge* conn = nullptr;

    conn = between(*e1._v1, *e2._v1);
    if (conn)
        return conn;
    
    conn = between(*e1._v1, *e2._v2);
    if (conn)
        return conn;
    
    conn = between(*e1._v2, *e2._v1);
    if (conn)
        return conn;
    
    conn = between(*e1._v2, *e2._v2);
    if (conn)
        return conn;
    
    return nullptr;
}

REF_VECTOR_ITERATOR(Mesh::Edge) Mesh::EdgeList::begin()
{
    return _edges.begin();
}

REF_VECTOR_ITERATOR(Mesh::Edge) Mesh::EdgeList::end()
{
    return _edges.end();
}

REF_VECTOR_CONST_ITERATOR(Mesh::Edge) Mesh::EdgeList::begin() const
{
    return _edges.cbegin();
}

REF_VECTOR_CONST_ITERATOR(Mesh::Edge) Mesh::EdgeList::end() const
{
    return _edges.cend();
}

Mesh::Edge& Mesh::EdgeList::extrude(Edge& e)
{
    // Extrude from both vertices
    Vert& nv1 = e._v1->extrude();
    Vert& nv2 = e._v2->extrude();

    // Connect the vertices with an edge
    Edge& ne = emplace(nv1, nv2);

    // Connect all vertices with a face
    _mesh.faces.emplace_verts(Verts(&nv1, &nv2, e._v2, e._v1));

    // Return the connecting edge
    return ne;
}

Mesh::Face::Face(const size_t& index, Mesh& mesh) :
    _index(index),
    _mesh(mesh)
{
}

bool Mesh::Face::operator==(const Face& other) const
{
    return _index == other._index;
}

bool Mesh::Face::operator!=(const Face& other) const
{
    return !(*this == other);
}

bool Mesh::Face::adjacent_to(const Vert& v) const
{
    for (Edge* _e : _edges)
    {
        if (_e->adjacent_to(v))
            return true;
    }

    return false;
}

bool Mesh::Face::adjacent_to(const Edge& e) const
{
    for (Edge* _e : _edges)
    {
        if (*_e == e)
            return true;
    }

    return false;
}

bool Mesh::Face::adjacent_to(const Face& f) const
{
    for (Edge* _e : _edges)
    {
        if (_e->adjacent_to(f))
            return true;
    }

    return false;
}

bool Mesh::Face::directly_adjacent_to(const Face& f) const
{
    for (Edge* _e : _edges)
    {
        if (_e->directly_adjacent_to(f))
            return true;
    }

    return false;
}

Mesh::Face& Mesh::Face::extrude()
{
    return _mesh.faces.extrude(*this);
}

Mesh::FaceList::FaceList(Mesh& mesh):
    _mesh(mesh)
{
}

Mesh::Face& Mesh::FaceList::operator[](const size_t& ind)
{
    return _faces.at(ind);
}

const Mesh::Face& Mesh::FaceList::operator[](const size_t& ind) const
{
    return _faces.at(ind);
}

REF_VECTOR_ITERATOR(Mesh::Face) Mesh::FaceList::begin()
{
    return _faces.begin();
}

REF_VECTOR_ITERATOR(Mesh::Face) Mesh::FaceList::end()
{
    return _faces.end();
}

REF_VECTOR_CONST_ITERATOR(Mesh::Face) Mesh::FaceList::begin() const
{
    return _faces.cbegin();
}

REF_VECTOR_CONST_ITERATOR(Mesh::Face) Mesh::FaceList::end() const
{
    return _faces.cend();
}

Mesh::Face& Mesh::FaceList::extrude(Face& f)
{
    std::list<Edge*> es;
    std::list<Edge*> nes;

    f.directly_adjacent_edges(es);
    _mesh.edges.extrude(es, nes);

    return emplace_edges(nes);
}

Mesh::Mesh():
    verts(*this),
    edges(*this),
    faces(*this)
{
}

Mesh::Mesh(Mesh& other) :
    verts(*this),
    edges(*this),
    faces(*this)
{
    *this = other;
}

Mesh::~Mesh()
{
}

Mesh& Mesh::operator=(Mesh& other)
{
    verts._verts.clear();
    edges._edges.clear();
    faces._faces.clear();

    return *this += other;
}

Mesh& Mesh::operator+=(Mesh& other)
{
    std::map<Vert*, Vert*> vmap;

    for (Vert& v : other.verts)
    {
        Vert& nv = verts.emplace(v);
        vmap[&v] = &nv;
    }

    std::map<Edge*, Edge*> emap;

    for (Edge& e : other.edges)
    {
        std::vector<Vert*> vs;
        vs.reserve(2);
        e.adjacent_verts(vs);

        Vert& nv1 = *vmap[vs[0]];
        Vert& nv2 = *vmap[vs[1]];

        Edge& ne = edges.emplace(nv1, nv2);
        emap[&e] = &ne;
    }

    for (Face& f : other.faces)
    {
        std::list<Edge*> es;
        f.directly_adjacent_edges(es);

        std::list<Edge*> nes;
        for (Edge* e : es)
        {
            nes.push_back(emap[e]);
        }

        faces.emplace_edges(nes);
    }

    return *this;
}

std::ostream& Mesh::print(std::ostream& out) const
{
    int index = 0;
    for (const auto& v : verts)
    {
        out << v << ":";

        for (Edge* const e : v._edges)
        {
            out << " " << *e;
        }
        out << "\n";
    }

    index = 0;
    for (const auto& e : edges)
    {
        out << e << ":";

        out << " " << *e._v1;
        out << " " << *e._v2;

        out << " /";

        for (Face* const f : e._faces)
        {
            out << " " << *f;
        }

        out << "\n";
    }

    index = 0;
    for (const auto& f : faces)
    {
        out << f<< ":";

        for (Edge* const e : f._edges)
        {
            out << " " << *e;
        }
        out << "\n";
    }

    return out;
}

void Mesh::print_verbose(std::ostream& out)
{
    for (Vert& v : verts)
    {
        out << v << ":";

        // Show adjacent verts
        std::list<Vert*> vs;
        v.adjacent_verts(vs);
        for (Vert* _v : vs)
            out << " " << *_v;
        
        out << " /";

        // Show adjacent edges
        std::list<Edge*> es;
        v.adjacent_edges(es);
        for (Edge* _e : es)
            out << " " << *_e;

        out << " /";

        // Show adjacent faces
        std::list<Face*> fs;
        v.adjacent_faces(fs);
        for (Face* _f : fs)
            out << " " << *_f;

        out << "\n";
    }

    for (Edge& e : edges)
    {
        out << e << ":";

        // Show adjacent verts
        std::list<Vert*> vs;
        e.adjacent_verts(vs);
        for (Vert* _v : vs)
            out << " " << *_v;
        
        out << " /";
        
        // Show adjacent edges
        std::list<Edge*> es;
        e.adjacent_edges(es);
        for (Edge* _e : es)
            out << " " << *_e;
        
        out << " /";

        // Show adjacent faces
        std::list<Face*> fs;
        e.adjacent_faces(fs);
        for (Face* _f : fs)
            out << " " << *_f;

        out << " /";

        // Show directly adjacent faces
        fs.clear();
        e.directly_adjacent_faces(fs);
        for (Face* _f : fs)
            out << " " << *_f;

        out << "\n";
    }

    for (Face& f : faces)
    {
        out << f << ":";

        // Show adjacent verts
        std::list<Vert*> vs;
        f.adjacent_verts(vs);
        for (Vert* _v : vs)
            out << " " << *_v;
        
        out << " /";
        
        // Show adjacent edges
        std::list<Edge*> es;
        f.adjacent_edges(es);
        for (Edge* _e : es)
            out << " " << *_e;
        
        out << " /";

        // Show directly adjacent edges
        es.clear();
        f.directly_adjacent_edges(es);
        for (Edge* _e : es)
            out << " " << *_e;
        
        out << " /";

        // Show adjacent faces
        std::list<Face*> fs;
        f.adjacent_faces(fs);
        for (Face* _f : fs)
            out << " " << *_f;

        out << " /";

        // Show directly adjacent faces
        fs.clear();
        f.directly_adjacent_faces(fs);
        for (Face* _f : fs)
            out << " " << *_f;

        out << "\n";
    }
}

std::ostream& operator<<(std::ostream& out, const Mesh::Vert& v)
{
    return v.print(out);
}

std::ostream& operator<<(std::ostream& out, const Mesh::Edge& e)
{
    return e.print(out);
}

std::ostream& operator<<(std::ostream& out, const Mesh::Face& f)
{
    return f.print(out);
}

std::ostream& operator<<(std::ostream& out, const Mesh& m)
{
    return m.print(out);
}