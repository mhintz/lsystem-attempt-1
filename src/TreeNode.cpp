#include "TreeNode.h"

#include <deque>
#include <cmath>

#include "Util.h"

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

// Anonymous namespace for scoping purposes
namespace {

// Like a % b, but returns b (not 0) when a == b
// returns 0 only when a == 0
int inclusiveModulo(int a, int b) {
	return a == b ? b : a % b;
}

} // Anonymous namespace

TreeNode::BranchAttribsRef TreeNode::getAttributes() {
	BranchAttribsRef attribs = BranchAttribsRef(new BranchAttribs());

	quat orientation = this->getGlobalOrientation();
	vec3 basePos = this->getGlobalBasePosition();
	vec3 endPos = basePos + this->getVector();

	// The peak of the cone (index 0)
	attribs->positions.push_back(endPos);
	attribs->colors.push_back(Color::hex(0x5C3118));

	int NUM_SIDES = 6;
	float rotation = M_2_PI / NUM_SIDES;
	float radius = 1.0f;
	for (int count = 0; count < NUM_SIDES; count++) {
		vec3 radial = radius * glm::normalize(orientation * glm::rotateY(vec3(1, 0, 0), rotation * count));
		std::cout << radial << std::endl;
		// vec3 vertPosition = basePos + radial;
		vec3 vertPosition = basePos;

		attribs->positions.push_back(vertPosition);
		attribs->colors.push_back(Color::hex(0x5C3118));

		// Build a triangle
		// The 0 index is the tip of the cone
		std::cout << inclusiveModulo(count + 1, NUM_SIDES) << "," << 0 << "," << inclusiveModulo(count + 2, NUM_SIDES) << std::endl;

		attribs->indices.push_back(inclusiveModulo(count + 1, NUM_SIDES));
		attribs->indices.push_back(0);
		attribs->indices.push_back(inclusiveModulo(count + 2, NUM_SIDES));
	}

	attribs->normals = util::calculateNormals(attribs->positions, attribs->indices);

	return attribs;
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