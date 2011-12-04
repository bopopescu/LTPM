#pragma once

#include <map>
#include <set>

using namespace std;

class Graph
{
public:
	void addVertexIfNotPresent(int v)
	{
		if(!vertexExists(v))
		{
			graph[v] = set<int>();
			cout << "added " << v << " to graph" << endl;
		}
	}

	void addEdgeAndVerticesIfNotPresent(int v1, int v2)
	{
		addVertexIfNotPresent(v1);
		addVertexIfNotPresent(v2);
		if(!edgeExists(v1, v2))
		{
			graph[v1].insert(v2);
			graph[v2].insert(v1);
		}
	}

	bool edgeExists(int v1, int v2)
	{
		if(!vertexExists(v1) || !vertexExists(v2))
			return false;

		if(graph[v1].find(v2) != graph[v1].end())
			return true;
		else
			return false;
	}

	bool vertexExists(int v)
	{
		bool exists = graph.find(v) != graph.end();
		if(!exists) cout << v << " does not exist in graph" << endl;
		return exists;
	}

	void collapseEdge(int v1, int v2);

private:
	map<int, set<int> > graph;
	
};
