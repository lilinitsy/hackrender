#ifndef SEARCH_H
#define SEARCH_H

#include <utility>
#include <vector>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "Graph.h"

class Search
{
	public:
		Search()
		{

		}

		virtual std::vector<GraphNode*> find_path(std::vector<GraphNode*> graph, GraphNode *start, GraphNode *goal)
		{
			return graph;
		}
};

#endif