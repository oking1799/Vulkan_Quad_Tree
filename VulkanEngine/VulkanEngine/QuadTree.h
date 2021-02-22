#pragma once
#include "pch.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#define MAX_COUNT 5

class QuadTree
{
	//Children of the quad
	QuadTree* children = nullptr;

	//Points stored in the quad
	std::vector<glm::vec2> points;
	int pointCount = 0;

	//Depth level
	int level = 0;

	//min and max of quad
	glm::vec2 topLeft;
	glm::vec2 bottomRight;

	bool InBounds(glm::vec2 point) 
	{
		//std::cout << topLeft.x << std::endl;
		return(point.x >= topLeft.x &&
			point.x <= bottomRight.x &&
			point.y >= bottomRight.y &&
			point.y <= topLeft.y);

		
	}
	//find child that contains the point
	int ChooseChild(glm::vec2 point) {
		for (int i = 0; i < 4; i++) {
			if (children[i].InBounds(point)) {
				return i;
			}
		}
		return -1;
	}
	//split a parent into 4 child quads
	void SplitQuad(std::vector<glm::vec2>& linePoints) 
	{
		float halfWidth = abs(topLeft.x - bottomRight.x) / 2.0f;
		float halfHeight = abs(topLeft.y - bottomRight.y) / 2.0f;

		children = new QuadTree[4];

		//top left
		children[0] = QuadTree(topLeft, glm::vec2(topLeft.x + halfWidth, bottomRight.y + halfHeight), level + 1);

		//top right 
		children[1] = QuadTree(glm::vec2(topLeft.x + halfWidth, topLeft.y), glm::vec2(bottomRight.x, bottomRight.y + halfHeight), level + 1);

		//bottom right 
		children[2] = QuadTree(glm::vec2(topLeft.x + halfWidth, bottomRight.y + halfHeight), bottomRight, level + 1);

		//bottom left 
		children[3] = QuadTree(glm::vec2(topLeft.x, bottomRight.y + halfHeight), glm::vec2(topLeft.x + halfWidth, bottomRight.y), level + 1);

		//vertical line 
		linePoints.push_back(glm::vec2(topLeft.x + halfWidth, topLeft.y));
		linePoints.push_back(glm::vec2(topLeft.x + halfWidth, bottomRight.y));

		DebugManager::GetInstance()->DrawLine(glm::vec3(topLeft.x + halfWidth, topLeft.y, 0.0f), glm::vec3(topLeft.x + halfWidth, bottomRight.y, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), -1.0f);

		//horizontal line
		linePoints.push_back(glm::vec2(topLeft.x, bottomRight.y + halfHeight));
		linePoints.push_back(glm::vec2(bottomRight.x, bottomRight.y + halfHeight));

		DebugManager::GetInstance()->DrawLine(glm::vec3(topLeft.x, bottomRight.y + halfHeight, 0.0f), glm::vec3(bottomRight.x, bottomRight.y + halfHeight, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), -1.0f);

		//Distribute points into chilfren
		for (int i = pointCount - 1; i >= 0; i--) 
		{
			int index = ChooseChild(points[i]);

			if (index > -1) 
			{
				children[index].Insert(points[i], linePoints);
			}
		}

		pointCount = 0;
	}


public:
	QuadTree() 
	{
		topLeft = glm::vec2(-2.0f, 2.0f);
		bottomRight = glm::vec2(2.0f, -2.0f);
		level = 0;
	}
	QuadTree(glm::vec2 topL, glm::vec2 bottomR, int l) 
	{
		topLeft = topL;
		bottomRight = bottomR;
		level = l;
	}
	~QuadTree() 
	{
		if (children != nullptr) {
			delete[] children;
		}
	}

	//Recursive insert method for a point and a refernce to container holding quadtree render points
	void Insert(glm::vec2 point, std::vector<glm::vec2>& linePoints) 
	{
		//if no point in quad, return
		if (!InBounds(point)) {
			return;
		}

		//If we already split the quad, find it's child
		//whcih contains the point and insert there
		if (children != nullptr) {
			//get child index
			int index = ChooseChild(point);

			if (index > -1) {
				children[index].Insert(point, linePoints);
			}

			return;
		}

		//If we have not yet split this quad:
		//If we have exceeded the max number of contained points, 
		//split the quad and distribute contained points
		//Constrained by a max depth of 20
		if (pointCount == MAX_COUNT && level < 20) {
			SplitQuad(linePoints);
			Insert(point, linePoints);
		}
		else {
			points.push_back(point);
			pointCount++;
		}
	}


};