#include "LSystem.h"

#include <iostream>

#include "cinder/Rand.h"

#include "TreeNode.h"

using namespace cinder;
using std::vector;

void LSystem::computeSystem() const
{
	if (mCalculationsCached) {
		return;
	}

	mPositions = vector<vec3>();
	mNormals = vector<vec3>();
	mColors = vector<vec3>();
	mIndices = vector<uint32_t>();

	TreeNode root = TreeNode();

	vector<TreeNode *> iterChildren;
	vector<TreeNode *> nextChildren;

	iterChildren.push_back(& root);

	for (int iterNum = 1; iterNum <= mNumIterations; iterNum++) {
		nextChildren.clear();

		for (TreeNode * iterBranch : iterChildren) {
			iterBranch->generateChildren();

			vector<TreeNode> & branchChildren = iterBranch->getChildrenRef();
			for (TreeNode & child : branchChildren) {
				nextChildren.push_back(& child);
			}
		}

		iterChildren = nextChildren;
	}

	root.visitBreadthFirst([&] (TreeNode * theNode) {
		TreeNode::BranchAttribsRef nodeAttributes = theNode->getAttributes();

		uint32_t baseIndex = this->mPositions.size();

		this->mPositions.insert(this->mPositions.end(), nodeAttributes->positions.begin(), nodeAttributes->positions.end());
		this->mColors.insert(this->mColors.end(), nodeAttributes->colors.begin(), nodeAttributes->colors.end());
		this->mNormals.insert(this->mNormals.end(), nodeAttributes->normals.begin(), nodeAttributes->normals.end());

		for (uint32_t index : nodeAttributes->indices) {
			this->mIndices.push_back(baseIndex + index);
		}
	});

	mCalculationsCached = true;
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
	// vector<vec3> tangents;
	// need to calculate tangents
	// target->copyAttrib(geom::Attrib::TANGENT, 3, 0, value_ptr(*(tangents.data())), tangents.size());

	// Assume 4 bytes per vertex index (highest index can be pretty big)
	target->copyIndices(getPrimitive(), mIndices.data(), mIndices.size(), 4);
}

