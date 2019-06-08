
#include "Graph.h"

Graph::NodeIndex Graph::node_create()
{
	NodeIndex n = (NodeIndex)nodes.size();
	nodes.emplace_back();

	return n;
}

Graph::EdgeIndex Graph::edge_create(const NodeIndex& n1, const NodeIndex& n2)
{
	EdgeIndex e = (EdgeIndex)edges.size();
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