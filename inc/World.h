#pragma once

#include "util.h"

#include <vector>
#include <glm/glm.hpp>
#include <queue>



class World {
	public:
	World(unsigned int x, unsigned int y);

	void genVertices(unsigned int x, unsigned int y);
	void genIndices(unsigned int x, unsigned int y);

	std::vector<float> getVertices();
	std::vector<int> getIndices();

	void draw();

	void pushColor(glm::vec3 color, unsigned int x, unsigned int y);
	void wfcGen();
	glm::vec3 colorNode(node* n);
	void step();
	void collapseWF();
	void collapseNode(node* n);
	void pruneNeighbor(node* t, node* n);
	bool finished();

	private:
	unsigned int mWorldX;
	unsigned int mWorldY;
	std::vector<float> mVertices;
	std::vector<int> mIndices;
	std::vector<glm::vec3> mColorData;
	std::vector<std::vector<node*>> mNodeList;
	std::vector<node*> mEntropyQueue;
	bool mFinished;
	std::vector<glm::vec3> mCols;
	std::vector<int> mDomains;
	int mBaseEntropy;
};