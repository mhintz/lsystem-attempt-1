#include "TreeNode.h"

#include <deque>
#include <cmath>

using namespace cinder;

vec3 TreeNode::getVector() {
	vec3 nodeVec = this->getGlobalOrientation() * vec3(0, 1, 0);
	return mLength * normalize(nodeVec);
}

vec3 TreeNode::getGlobalBasePosition() {
	if (mParent) {
		return mParent->getGlobalBasePosition() + mPosition * mParent->getVector();
	}

	return vec3(0, 0, 0);
}

quat TreeNode::getGlobalOrientation() {
	if (mParent) {
		return glm::normalize(mOrientation * mParent->getGlobalOrientation());
	}
	return mOrientation;
}

std::vector<vec3> TreeNode::getPositions() {
	vec3 basePos = this->getGlobalBasePosition();
	vec3 endPos = basePos + this->getVector();
	std::vector<vec3> retVal = { basePos, endPos };
	return std::move(retVal);
}

void TreeNode::generateChildren(int level) {
	int const NUM_CHILDREN = 4;
	for (int i = 0; i < NUM_CHILDREN; i++) {
		int xAxisVal = i & 1; // 0, 1, 0, 1
		int zAxisVal = 1 - (i & 1);
		int multiplier = std::round((float) i / NUM_CHILDREN) * 2 - 1; // -1, -1, 1, 1
		float rotationAngle = glm::radians(30.0f * multiplier);
		// axis order becomes -z, -x, +z, +x
		mChildren.push_back(TreeNode()
			.orientation(angleAxis(rotationAngle, vec3(xAxisVal, 0, zAxisVal)))
			.position(1.0f)
			.length(1.0f)
			.level(level)
			.scale(1.0f));
		// I think this is necessary, because the argument to push_back is copied...
		mChildren.back().setParent(this);
	}
}


void TreeNode::visitBreadthFirst(std::function<void (TreeNode *)> const & visitFunc) {
	std::deque<TreeNode *> unvisited = { this };
	while (unvisited.size()) {
		TreeNode * current = unvisited.front();
		unvisited.pop_front();

		for (TreeNode & child : current->getChildrenRef()) {
			unvisited.push_back(& child);
		}

		visitFunc(current);
	}
}