
#pragma once

#include <list>
#include <unordered_set>
#include <vector>

class Graph
{
public:
	typedef int NodeIndex;
	typedef int EdgeIndex;

private:
	struct Node
	{
		std::unordered_set<EdgeIndex> edges;
	};

	struct Edge
	{
		std::vector<NodeIndex> nodes;

		Edge(const NodeIndex& n1, const NodeIndex& n2)
		{
			nodes.reserve(2);
			nodes.emplace_back(n1);
			nodes.emplace_back(n2);
		}

		NodeIndex opposite(const NodeIndex& n) const
		{
			if (nodes[0] == n)
				return nodes[1];
			else if (nodes[1] == n)
				return nodes[0];
			else
				return -1;
		}
	};

public:
	NodeIndex node_create();
	EdgeIndex edge_create(const NodeIndex& n1, const NodeIndex& n2);

	template <class Container>
	void node_get_adjacent_edges(Container& c, const NodeIndex& n) const
	{
		for (const EdgeIndex& e : nodes[n].edges)
		{
			c.push_back(e);
		}
	}

	template <class Container>
	void node_get_adjacent_nodes(Container& c, const NodeIndex& n) const
	{
		for (const EdgeIndex& e : nodes[n].edges)
		{
			const Edge& edge = edges[e];
			c.push_back(edge.opposite(n));
		}
	}

	template <class Container>
	void edge_get_connected_nodes(Container& c, const EdgeIndex& e) const
	{
		const Edge& edge = edges[e];
		for (const NodeIndex& n : edge.nodes)
		{
			c.push_back(n);
		}
	}

	NodeIndex edge_get_opposite_node(const EdgeIndex& e, const NodeIndex& n) const;

private:
	std::vector<Node> nodes;
	std::vector<Edge> edges;
};