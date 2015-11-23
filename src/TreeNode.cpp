#include "TreeNode.h"

#include <deque>
#include <cmath>

#include "cinder/Rand.h"

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

int maxChildrenForLevel(int level) {
	return round(fmax(1, 7 - level * 0.6f));
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

// According to Da Vinci, the diameter of a branch is the sum of all the diameters
// of its child branches. According to Søren Pirk, to get the diameter of any given
// branch (call it "B"):
// dRoot * ( sum of lengths of subtree from "B" to root / sum of lengths of entire tree's branches ) ^ some parameter
// The value of "some parameter" is usually about 1.5, but it varies.
// See "Efficient Processing of Plant Life", by Soeren Pirk
float TreeNode::getSuperTreeLengthSum() {
	return this->reduceSuperTree(0.0f, [] (float memo, TreeNode * node) -> float {
		return memo + node->mLength;
	});
}

float TreeNode::getSubTreeLengthSum() {
	return this->reduceBreadthFirst(0.0f, [this] (float memo, TreeNode * node) -> float {
		if (node == this) {
			return memo;
		} else {
			return memo + node->mLength;
		}
	});
}

TreeNode::BranchAttribsRef TreeNode::getAttributes() {
	TreeNode::BranchAttribsRef attribs = TreeNode::BranchAttribsRef(new BranchAttribs());

	quat orientation = this->getGlobalOrientation();
	vec3 basePos = this->getGlobalBasePosition();
	vec3 endPos = basePos + this->getVector();

	int const NUM_SIDES = 10;
	float rotation = 2.f * glm::pi<float>() / NUM_SIDES;
	float radius = this->mDiameter / 2.f;
	for (int count = 0; count < NUM_SIDES; count++) {
		float radialRotation1 = rotation * count;
		vec3 radial1 = radius * glm::normalize(orientation * glm::rotateY(vec3(1, 0, 0), radialRotation1));

		float radialRotation2 = rotation * (count + 1);
		vec3 radial2 = radius * glm::normalize(orientation * glm::rotateY(vec3(1, 0, 0), radialRotation2));

		vec3 v1 = basePos + radial1;
		vec3 v2 = basePos + radial2;
		vec3 v3 = endPos + radial1;
		vec3 v4 = endPos + radial2;

		Color brownColor = Color::hex(0x5C3118);

		addVertexToAttribs(attribs, basePos, brownColor);
		addVertexToAttribs(attribs, v2, brownColor);
		addVertexToAttribs(attribs, v1, brownColor);

		addVertexToAttribs(attribs, v1, brownColor);
		addVertexToAttribs(attribs, v2, brownColor);
		addVertexToAttribs(attribs, v3, brownColor);

		addVertexToAttribs(attribs, v2, brownColor);
		addVertexToAttribs(attribs, v4, brownColor);
		addVertexToAttribs(attribs, v3, brownColor);

		addVertexToAttribs(attribs, endPos, brownColor);
		addVertexToAttribs(attribs, v3, brownColor);
		addVertexToAttribs(attribs, v4, brownColor);
	}

	attribs->normals = util::calculateNormals(attribs->positions, attribs->indices);

	return attribs;
}

void TreeNode::generateChildren() {
	int NUM_CHILDREN = randInt(1, maxChildrenForLevel(this->mLevel));
	int MAX_ANGLE = randFloat(60.0f);
	for (int i = 0; i < NUM_CHILDREN; i++) {
		float xAxisRot = glm::radians(randFloat(-MAX_ANGLE, MAX_ANGLE));
		float zAxisRot = glm::radians(randFloat(-MAX_ANGLE, MAX_ANGLE));
		quat orientation = angleAxis(xAxisRot, vec3(1, 0, 0)) * angleAxis(zAxisRot, vec3(0, 0, 1));
		// axis order becomes -z, -x, +z, +x
		mChildren.push_back(TreeNode()
			.orientation(orientation)
			.position(1.0f)
			.length(this->mLength * 0.7f)
			.diameter(this->mDiameter * 0.85f)
			.level(this->mLevel + 1)
			.setParent(this));
	}
}

void TreeNode::visitSuperTree(std::function<void (TreeNode *)> const & visitFunc) {
	if (mParent) {
		visitFunc(mParent);
		return mParent->visitSuperTree(visitFunc);
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