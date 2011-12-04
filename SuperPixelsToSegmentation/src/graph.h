#pragma once

#include <map>
#include <set>

#include "Segment.h"

using namespace std;

class Edge
{
public:
	Segment* a;
	Segment* b;
	bool operator<(const Edge & other) const {
		if(a->label < other.a->label)
			return true;
		else if(a->label == other.a->label)
			if(b->label < other.b->label)
				return true;
		return false;
	}
};

class Graph
{
public:
	void addVertexIfNotPresent(Segment* s)
	{
		if(!vertexExists(s))
		{
			graph[s] = set<Segment*>();
			//cout << "added Segment " << s->label << " to graph" << endl;
		}
	}

	void addEdgeAndVerticesIfNotPresent(Segment* s1, Segment* s2)
	{
		addVertexIfNotPresent(s1);
		addVertexIfNotPresent(s2);
		if(!edgeExists(s1, s2))
		{
			graph[s1].insert(s2);
			graph[s2].insert(s1);
		}
	}

	bool edgeExists(Segment* s1, Segment* s2)
	{
		if(!vertexExists(s1) || !vertexExists(s2))
			return false;

		if(graph[s1].find(s2) != graph[s1].end())
			return true;
		else
			return false;
	}

	bool vertexExists(Segment* s)
	{
		bool exists = graph.find(s) != graph.end();
		/*if(!exists)
			cout << s << " is NOT in the graph" << endl;
		else
			cout << s << " is in the graph" << endl;*/
		return exists;
	}

	set<Edge> edges()
	{
		set<Edge> edges;
		for(map<Segment*, set<Segment*> >::iterator node = graph.begin(); node != graph.end(); node++)
		{
			Segment* currentSegment = (*node).first;
			set<Segment*> neighbors = (*node).second;
			for(set<Segment*>::iterator neighbor_node = neighbors.begin(); neighbor_node != neighbors.end(); neighbor_node++)
			{
				Segment* neighborSegment = (*neighbor_node);
				Edge canonicalEdge;
				if(currentSegment->label < neighborSegment->label)
				{
					canonicalEdge.a = currentSegment;
					canonicalEdge.b = neighborSegment;
				}
				else
				{
					canonicalEdge.a = neighborSegment;
					canonicalEdge.b = currentSegment;
				}
				edges.insert(canonicalEdge);
			}
		}
		return edges;
	}

	void collapseEdge(Segment* s1, Segment* s2);

private:
	map<Segment*, set<Segment*> > graph;
	
};
