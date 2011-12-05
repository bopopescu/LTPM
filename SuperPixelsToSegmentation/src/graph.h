#pragma once

#include <map>
#include <set>

#include "Segment.h"


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
			graph[s] = std::set<Segment*>();
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

	std::set<Edge> edges()
	{
		std::set<Edge> edges;
		for(map<Segment*, std::set<Segment*> >::iterator node = graph.begin(); node != graph.end(); node++)
		{
			Segment* currentSegment = (*node).first;
			std::set<Segment*> neighbors = (*node).second;
			for(std::set<Segment*>::iterator neighbor_node = neighbors.begin(); neighbor_node != neighbors.end(); neighbor_node++)
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

	void collapseEdge(const Edge e)
	{
		Segment *s1 = e.a, *s2 = e.b;

		Segment *newSegment = Segment::combine(e.a, e.b);
		std::set<Segment*> newSegmentNeighbors;
		newSegmentNeighbors.insert(graph[s1].begin(), graph[s1].end());
		newSegmentNeighbors.insert(graph[s2].begin(), graph[s2].end());
		newSegmentNeighbors.erase(s1);
		newSegmentNeighbors.erase(s2);
			


		// replace s2 references with s1 references
		for(map<Segment*, std::set<Segment*> >::iterator node = graph.begin(); node != graph.end(); node++)
		{
			std::set<Segment*>::iterator s2Location = (*node).second.find(s2);
			if(s2Location != (*node).second.end())
			{
				(*node).second.erase(s2);
				(*node).second.insert(newSegment);
			}

			std::set<Segment*>::iterator s1Location = (*node).second.find(s1);
			if(s1Location != (*node).second.end())
			{
				(*node).second.erase(s1);
				(*node).second.insert(newSegment);
			}


		}


		// remove s1 and s2
		graph.erase(s1);
		graph.erase(s2);
		
		graph[newSegment] = newSegmentNeighbors;

		
	}

	vector<Segment*> vertices()
	{
		vector<Segment*> vertices;
		for(map<Segment*, std::set<Segment*> >::iterator node = graph.begin(); node != graph.end(); node++)
		{
			vertices.push_back((*node).first);
		}
		return vertices;
	}

private:
	map<Segment*, std::set<Segment*> > graph;
	
};
