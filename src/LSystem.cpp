#include "LSystem.h"

#include <iostream>

#include "cinder/Rand.h"

#include "TreeNode.h"

using namespace cinder;

void LSystem::computeSystem() const
{
	if (mCalculationsCached) {
		return;
	}

	TreeNode root = TreeNode();

	std::vector<TreeNode *> iterChildren;
	std::vector<TreeNode *> nextChildren;

	iterChildren.push_back(& root);

	for (int i = 1; i <= mNumIterations; i++) {
		nextChildren.clear();

		for (TreeNode * iterBranch : iterChildren) {
			iterBranch->generateChildren(i);

			std::vector<TreeNode> & branchChildren = iterBranch->getChildrenRef();
			for (TreeNode & child : branchChildren) {
				nextChildren.push_back(& child);
			}
		}

		iterChildren = nextChildren;
	}

	root.visitBreadthFirst([&] (TreeNode * theNode) {
		Color const brown = Color("brown");
		std::vector<vec3> nodePositions = theNode->getPositions();
		for (vec3 & pos : nodePositions) {
			this->mIndices.push_back((uint32_t) this->mPositions.size());
			this->mPositions.push_back(pos);
			this->mColors.push_back(brown.get(CM_RGB));
			this->mNormals.push_back(vec3(0, 0, 0));
		}
	});

	mCalculationsCached = true;
}

void LSystem::addNodeToBuffers(TreeNode * theNode) {
	std::cout << this << std::endl;
}

uint8_t LSystem::getAttribDims( geom::Attrib attr ) const {
	switch (attr) {
		case geom::Attrib::POSITION: return 3;
		case geom::Attrib::NORMAL: return 3;
		case geom::Attrib::COLOR: return 3;
		// to be added
		// case geom::Attrib::TANGENT: return 3;
		default:
			return 0;
	}
}

geom::AttribSet LSystem::getAvailableAttribs() const {
	return { geom::Attrib::POSITION, geom::Attrib::NORMAL, geom::Attrib::COLOR };
	// to be added
	// return { geom::Attrib::POSITION, geom::Attrib::NORMAL, geom::Attrib::COLOR, geom::Attrib::TANGENT };
}

void LSystem::loadInto( geom::Target *target, const geom::AttribSet &requestedAttribs ) const {
	computeSystem();

	// Copy attributes over to the target
	target->copyAttrib(geom::Attrib::POSITION, 3, 0, value_ptr(*(mPositions.data())), mPositions.size());
	target->copyAttrib(geom::Attrib::NORMAL, 3, 0, value_ptr(*(mNormals.data())), mNormals.size());
	target->copyAttrib(geom::Attrib::COLOR, 3, 0, value_ptr(*(mColors.data())), mColors.size());

	// to be added
	// std::vector<vec3> tangents;
	// need to calculate tangents
	// target->copyAttrib(geom::Attrib::TANGENT, 3, 0, value_ptr(*(tangents.data())), tangents.size());

	// Assume 4 bytes per vertex index (highest index can be pretty big)
	target->copyIndices(getPrimitive(), mIndices.data(), mIndices.size(), 4);
}

