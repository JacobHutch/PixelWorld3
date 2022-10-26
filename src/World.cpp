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
    mDomains = {0,1,2,3,4,5,6,7,8,9,10,11};
    mBaseEntropy = mDomains.size();
    //rainbow scheme
    mCols = {glm::vec3(1.0f,0.0f,0.0f),glm::vec3(1.0f,0.5f,0.0f),
            glm::vec3(1.0f,1.0f,0.0f),glm::vec3(0.5f,1.0f,0.0f),
            glm::vec3(0.0f,1.0f,0.0f),glm::vec3(0.0f,1.0f,0.5f),
            glm::vec3(0.0f,1.0f,1.0f),glm::vec3(0.0f,0.5f,1.0f),
            glm::vec3(0.0f,0.0f,1.0f),glm::vec3(0.5f,0.0f,1.0f),
            glm::vec3(1.0f,0.0f,1.0f),glm::vec3(1.0f,0.0f,0.5f)};
    //weather radar scheme
    /*mCols = {glm::vec3(1.0f,1.0f,1.0f),glm::vec3(0.5f,0.0f,1.0f),
            glm::vec3(1.0f,0.0f,1.0f),glm::vec3(0.5f,0.0f,0.0f),
            glm::vec3(1.0f,0.0f,0.0f),glm::vec3(1.0f,0.5f,0.0f),
            glm::vec3(1.0f,1.0f,0.0f),glm::vec3(0.0f,0.5f,0.0f),
            glm::vec3(0.0f,1.0f,0.0f),glm::vec3(0.0f,0.0f,1.0f),
            glm::vec3(0.0f,1.0f,1.0f),glm::vec3(0.5f,0.5f,0.5f)};*/
    genVertices(x, y);
    genIndices(x, y);
}



void World::genVertices(unsigned int x, unsigned int y) {
    //each square has 4 vertices
    //vertex order in list: tl, tr, bl, br -- needs to be consistent with genIndices!!
    //vertices currently have position3 and color3, so multiply by 6 for stride
    std::vector<float> verts;
    for (unsigned int j = 0; j < y; j++) {
        for (unsigned int i = 0; i < x; i++) {

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
    for (unsigned int i = 0; i < x; i++) {
        for (unsigned int j = 0; j < y; j++) {

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
	//mColorData[x + (y * mWorldX)] = color;
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
	for (unsigned int i = 0; i < mWorldX; i++) {
        std::vector<node*> row;
		for (unsigned int j = 0; j < mWorldY; j++) {
            node* n = new node{mDomains, glm::vec3{0.0f,0.0f,0.0f}, mBaseEntropy, glm::vec2(i,j), false, nullptr, nullptr, nullptr, nullptr};
            row.push_back(n);
		}
        nodeList.push_back(row);
	}
    for (int i = 0; i < static_cast<int>(mWorldX); i++) {
        for (int j = 0; j < static_cast<int>(mWorldY); j++) {
            node* n = nodeList[i][j];
            n->color = colorNode(n);

            bool left = ((i - 1) >= 0);
            bool right = ((i + 1) < static_cast<int>(mWorldX));
            bool up = ((j - 1) >= 0);
            bool down = ((j + 1) < static_cast<int>(mWorldY));

            if (left) {
                n->left = nodeList[i - 1][j];
            }
            if (right) {
                n->right = nodeList[i + 1][j];
            }
            if (up) {
                n->up = nodeList[i][j - 1];
            }
            if (down) {
                n->down = nodeList[i][j + 1];
            }
        }
    }
    int startX = std::rand() % mWorldX;
    int startY = std::rand() % mWorldY;
    node* start = nodeList[startX][startY];
    mEntropyQueue.push_back(start);
    start->queued = true;
    startX = std::rand() % mWorldX;
    startY = std::rand() % mWorldY;
    start = nodeList[startX][startY];
    mEntropyQueue.push_back(start);
    start->queued = true;
    mNodeList = nodeList;
}



glm::vec3 World::colorNode(node* n) {
    std::vector<int> domain = n->domain;
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
    pushColor(final, n->pos.x, n->pos.y);
    return final;
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
    mEntropyQueue.pop_back();
    n->queued = false;
    collapseNode(n);
    //std::cout << mEntropyQueue.size() << std::endl;
}



void World::collapseNode(node* n) {
    if ((n->domain).size() == 0) {
        return;
    }
    int val = (n -> domain)[std::rand() % (n->domain).size()];
    n -> domain = {val};
    n -> color = colorNode(n);
    n -> entropy = 1;

    std::vector<int> domain;

    std::vector<node*> tList;

    if (n->left) {
        tList.push_back(n->left);
        pruneNeighbor(n->left, n);
    }
    if (n->right) {
        tList.push_back(n->right);
        pruneNeighbor(n->right, n);
    }
    if (n->up) {
        tList.push_back(n->up);
        pruneNeighbor(n->up, n);
    }
    if (n->down) {
        tList.push_back(n->down);
        pruneNeighbor(n->down, n);
    }

    /*int rInd = std::rand() % tList.size();
    pruneNeighbor(tList[rInd], n);*/

    std::sort(mEntropyQueue.begin(), mEntropyQueue.end(), Util::NodeCompare);
}



void World::pruneNeighbor(node* t, node* n) {
    if (t->entropy == 1) {
        return;
    }
    int val = (n -> domain)[0];
    std::vector<int> domain = {};
    for (int i = 0; i < t->entropy; i++) {
        if (((t->domain)[i]) == val) {
            domain.push_back((t->domain)[i]);
        }
        if (val == 0) {
            if (((t->domain)[i] == 11) || ((t->domain)[i] == 1)) {
                domain.push_back((t->domain)[i]);
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
            if (((t->domain)[i] == 2) || ((t->domain)[i] == 4)) {
                domain.push_back((t->domain)[i]);
            }
        } else if (val == 4) {
            if (((t->domain)[i] == 3) || ((t->domain)[i] == 5)) {
                domain.push_back((t->domain)[i]);
            }
        } else if (val == 5) {
            if (((t->domain)[i] == 4) || ((t->domain)[i] == 6)) {
                domain.push_back((t->domain)[i]);
            }
        } else if (val == 6) {
            if (((t->domain)[i] == 5) || ((t->domain)[i] == 7)) {
                domain.push_back((t->domain)[i]);
            }
        } else if (val == 7) {
            if (((t->domain)[i] == 6) || ((t->domain)[i] == 8)) {
                domain.push_back((t->domain)[i]);
            }
        } else if (val == 8) {
            if (((t->domain)[i] == 7) || ((t->domain)[i] == 9)) {
                domain.push_back((t->domain)[i]);
            }
        } else if (val == 9) {
            if (((t->domain)[i] == 8) || ((t->domain)[i] == 10)) {
                domain.push_back((t->domain)[i]);
            }
        } else if (val == 10) {
            if (((t->domain)[i] == 9) || ((t->domain)[i] == 11)) {
                domain.push_back((t->domain)[i]);
            }
        } else if (val == 11) {
            if (((t->domain)[i] == 10) || ((t->domain)[i] == 0)) {
                domain.push_back((t->domain)[i]);
            }
        }
    }
    t->domain = domain;
    t->color = colorNode(t);
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