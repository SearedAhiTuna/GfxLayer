
#include "Graph.h"

#include <list>

Graph::NodeIndex Graph::node_create()
{
    NodeIndex n = (NodeIndex)nodes.size();
    nodes.emplace_back();

    return n;
}

Graph::EdgeIndex Graph::edge_create(const NodeIndex& n1, const NodeIndex& n2)
{
    // Check first if there is already an edge between these nodes
    EdgeIndex e = edge_connecting(n1, n2);

    if (e >= 0)
        return e;

    // Create a new edge
    e = (EdgeIndex)edges.size();
    edges.emplace_back(n1, n2);

    nodes[n1].edges.emplace(e);
    nodes[n2].edges.emplace(e);

    return e;
}

Graph::NodeIndex Graph::edge_get_opposite_node(const EdgeIndex& e, const NodeIndex& n) const
{
    const Edge& edge = edges[e];
    return edge.opposite(n);
}

Graph::EdgeIndex Graph::edge_connecting(const NodeIndex& n1, const NodeIndex& n2) const
{
    for (const EdgeIndex& e : nodes[n1].edges)
    {
        const Edge& edge = edges[e];
        if (edge.opposite(n1) == n2)
            return e;
    }

    return -1;
}