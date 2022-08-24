#include "World.h"

#include <glad/glad.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>



World::World(unsigned int x, unsigned int y) : mWorldX(x), mWorldY(y) {
    std::srand(time(nullptr));
    mFinished = false;
	mColorData = std::vector<glm::vec3>(x * y, glm::vec3(1.0f));
    mDomains = {0,1,2,3};
    mBaseEntropy = mDomains.size();
    mCols = {glm::vec3(0.0f,0.25f,0.75f),glm::vec3(0.0f,0.75f,0.0f),
            glm::vec3(1.0f,0.75f,0.0f),glm::vec3(0.75f,0.0f,0.0f)};
    genVertices(x, y);
    genIndices(x, y);
}



void World::genVertices(unsigned int x, unsigned int y) {
    //each square has 4 vertices
    //vertex order in list: tl, tr, bl, br -- needs to be consistent with genIndices!!
    //vertices currently have position3 and color3, so multiply by 6 for stride
    std::vector<float> verts;
    for (int j = 0; j < y; j++) {
        for (int i = 0; i < x; i++) {

            //tl
            verts.push_back(0.0f + i);
            verts.push_back(0.0f + j);
            verts.push_back(0.0f);

            verts.push_back(1.0f);
            verts.push_back(0.0f);
            verts.push_back(0.0f);

            //tr
            verts.push_back(1.0f + i);
            verts.push_back(0.0f + j);
            verts.push_back(0.0f);

            verts.push_back(0.0f);
            verts.push_back(1.0f);
            verts.push_back(0.0f);

            //bl
            verts.push_back(0.0f + i);
            verts.push_back(1.0f + j);
            verts.push_back(0.0f);

            verts.push_back(0.0f);
            verts.push_back(0.0f);
            verts.push_back(0.0f);

            //br
            verts.push_back(1.0f + i);
            verts.push_back(1.0f + j);
            verts.push_back(0.0f);

            verts.push_back(0.0f);
            verts.push_back(0.0f);
            verts.push_back(1.0f);
        }
    }
    mVertices = verts;
}



void World::genIndices(unsigned int x, unsigned int y) {
    //as with genVertices, corner order is tl,tr,bl,br
    //triangles will be drawn as top left and bottom right (tl,bl,tr, tr,bl,br)
    std::vector<int> inds;
    int base, tl, tr, bl, br;
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {

            base = (i + (j * x)) * 4;
            tl = base;
            tr = base + 1;
            bl = base + 2;
            br = base + 3;

            inds.push_back(tl);
            inds.push_back(bl);
            inds.push_back(tr);

            inds.push_back(tr);
            inds.push_back(bl);
            inds.push_back(br);
        }
    }
    mIndices = inds;
}



std::vector<float> World::getVertices() {
    return mVertices;
}



std::vector<int> World::getIndices() {
    return mIndices;
}



void World::draw() {
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
}



void World::pushColor(glm::vec3 color, unsigned int x, unsigned int y) {
	mColorData[x + (y * mWorldX)] = color;
	int tl, tr, bl, br;
	tl = ((((x + (y * mWorldX)) * 4) * 6) + 3) * sizeof(float);
	tr = (((((x + (y * mWorldX)) * 4) + 1) * 6) + 3) * sizeof(float);
	bl = (((((x + (y * mWorldX)) * 4) + 2) * 6) + 3) * sizeof(float);
	br = (((((x + (y * mWorldX)) * 4) + 3) * 6) + 3) * sizeof(float);
	glBufferSubData(GL_ARRAY_BUFFER, tl, 3 * sizeof(float), &color[0]);
	glBufferSubData(GL_ARRAY_BUFFER, tr, 3 * sizeof(float), &color[0]);
	glBufferSubData(GL_ARRAY_BUFFER, bl, 3 * sizeof(float), &color[0]);
	glBufferSubData(GL_ARRAY_BUFFER, br, 3 * sizeof(float), &color[0]);
}



void World::wfcGen() {
	std::vector<std::vector<node*>> nodeList;
	for (int i = 0; i < mWorldX; i++) {
        std::vector<node*> row;
		for (int j = 0; j < mWorldY; j++) {
            node* n = new node{mDomains, colorNode(mDomains), mBaseEntropy, glm::vec2(i,j), false};
            row.push_back(n);
            pushColor(n->color, i, j);
		}
        nodeList.push_back(row);
	}
    int startX = std::rand() % mWorldX;
    int startY = std::rand() % mWorldY;
    node* start = nodeList[startX][startY];
    mEntropyQueue.push_back(start);
    start->queued = true;
    mNodeList = nodeList;
}



glm::vec3 World::colorNode(std::vector<int> domain) {
    glm::vec3 final = {0.0f, 0.0f, 0.0f};
    if (domain.size() == 0) {
        final = {0.0f, 0.0f, 0.0f};
    } else {
        int count = domain.size();
        final = {0.0f, 0.0f, 0.0f};
        glm::vec3 col;
        for (int n = 0; n < count; n++) {
            col = mCols[domain[n]];
            final.r += (col.r / count);
            final.g += (col.g / count);
            final.b += (col.b / count);
        }
    }
    return final;
}



void World::colorWorld() {
    /*        self.world = []
        for i in range(self.worldSize[0]):
            row = []
            for j in range(self.worldSize[1]):
                row.append(Tile(nodes[i][j][1]))
            self.world.append(row)
        self.pushWorld()*/
}



void World::step() {
    collapseWF();
}



void World::collapseWF() {
    if (mEntropyQueue.empty()) {
        mFinished = true;
        return;
    }
    node* n = mEntropyQueue.back();
    glm::vec2 pos = n->pos;
    mEntropyQueue.pop_back();
    n->queued = false;
    collapseNode(n, n->pos.x, n->pos.y);
    //std::cout << mEntropyQueue.size() << std::endl;
}



void World::collapseNode(node* n, int x, int y) {
    if ((n->domain).size() == 0) {
        return;
    }
    int val = (n -> domain)[std::rand() % (n->domain).size()];
    n -> domain = {val};
    n -> color = colorNode(n -> domain);
    n -> entropy = 1;

    bool left = ((x - 1) >= 0);
    bool right = ((x + 1) < mWorldX);
    bool up = ((y - 1) >= 0);
    bool down = ((y + 1) < mWorldY);
    node* t;
    std::vector<int> domain;

    if (left) {
        t = mNodeList[x - 1][y];
        pruneNeighbor(t, val);
        pushColor(t->color, x-1, y);
    }
    if (right) {
        t = mNodeList[x + 1][y];
        pruneNeighbor(t, val);
        pushColor(t->color, x+1, y);
    }
    if (up) {
        t = mNodeList[x][y - 1];
        pruneNeighbor(t, val);
        pushColor(t->color, x, y-1);
    }
    if (down) {
        t = mNodeList[x][y + 1];
        pruneNeighbor(t, val);
        pushColor(t->color, x, y+1);
    }
    std::sort(mEntropyQueue.begin(), mEntropyQueue.end(), Util::NodeCompare);
    pushColor(n -> color, x, y);
}



void World::pruneNeighbor(node* t, int val) {
    if (t->entropy == 1) {
        return;
    }
    std::vector<int> domain = {};
    for (int i = 0; i < t->entropy; i++) {
        if (val == 0) {
            if ((t->domain)[i] == 1) {
                domain.push_back(1);
            }
        } else if (val == 1) {
            if (((t->domain)[i] == 0) || ((t->domain)[i] == 2)) {
                domain.push_back((t->domain)[i]);
            }
        } else if (val == 2) {
            if (((t->domain)[i] == 1) || ((t->domain)[i] == 3)) {
                domain.push_back((t->domain)[i]);
            }
        } else if (val == 3) {
            if ((t->domain)[i] == 2) {
                domain.push_back(2);
            }
        }
    }
    t->domain = domain;
    t->color = colorNode(t->domain);
    t->entropy = domain.size();
    if ((t->entropy > 0) && (t->entropy <= mBaseEntropy) && !(t->queued)) {
        int rando = 0;
        if (mEntropyQueue.size() != 0) {
            rando = std::rand() % mEntropyQueue.size();
        }
        mEntropyQueue.insert(mEntropyQueue.begin() + rando, t);
        t->queued = true;
    }
}



bool World::finished() {
    return mFinished;
}