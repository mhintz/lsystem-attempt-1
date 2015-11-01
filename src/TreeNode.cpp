#include "TreeNode.h"

#include <deque>
#include <cmath>

#include "Util.h"

using namespace cinder;

// Anonymous namespace for scoping purposes
namespace {

// Like a % b, but returns b (not 0) when a == b
// returns 0 only when a == 0
int inclusiveModulo(int a, int b) {
	return a == b ? b : a % b;
}

void addVertexToAttribs(TreeNode::BranchAttribsRef attribs, vec3 const & pos, Color const & col) {
	attribs->indices.push_back(attribs->positions.size());
	attribs->positions.push_back(pos);
	attribs->colors.push_back(col);
}

} // Anonymous namespace

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

int TreeNode::getLevel() {
	if (mParent) {
		return mParent->getLevel() + 1;
	}

	return 0;
}

TreeNode::BranchAttribsRef TreeNode::getAttributes() {
	TreeNode::BranchAttribsRef attribs = TreeNode::BranchAttribsRef(new BranchAttribs());

	quat orientation = this->getGlobalOrientation();
	vec3 basePos = this->getGlobalBasePosition();
	vec3 endPos = basePos + this->getVector();

	int NUM_SIDES = 6;
	float rotation = 2.0f * glm::pi<float>() / NUM_SIDES;
	float radius = 0.25f;
	for (int count = 0; count < NUM_SIDES; count++) {
		float radialRotation1 = rotation * count;
		vec3 radial1 = radius * glm::normalize(orientation * glm::rotateY(vec3(1, 0, 0), radialRotation1));
		vec3 vertPosition1 = basePos + radial1;

		float radialRotation2 = rotation * (count + 1);
		vec3 radial2 = radius * glm::normalize(orientation * glm::rotateY(vec3(1, 0, 0), radialRotation2));
		vec3 vertPosition2 = basePos + radial2;

		Color brownColor = Color::hex(0x5C3118);

		addVertexToAttribs(attribs, endPos, brownColor);
		addVertexToAttribs(attribs, vertPosition1, brownColor);
		addVertexToAttribs(attribs, vertPosition2, brownColor);

		addVertexToAttribs(attribs, vertPosition1, brownColor);
		addVertexToAttribs(attribs, basePos, brownColor);
		addVertexToAttribs(attribs, vertPosition2, brownColor);
	}

	attribs->normals = util::calculateNormals(attribs->positions, attribs->indices);

	return attribs;
}

void TreeNode::generateChildren() {
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