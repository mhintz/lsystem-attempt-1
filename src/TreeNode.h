#pragma once

#include <vector>
#include <functional>

class TreeNode {
public:
	TreeNode & orientation(ci::quat const & _orientation) { mOrientation = _orientation; return *this; }
	TreeNode & position(float _position) { mPosition = _position; return *this; }
	TreeNode & length(float _length) { mLength = _length; return *this; }
	TreeNode & level(int _level) { mLevel = _level; return *this; }
	TreeNode & scale(float _scale) { mScale = _scale; return *this; }
	TreeNode & setParent(TreeNode * newParent) { mParent = newParent; return *this; }

	struct BranchAttribs {
		std::vector<ci::vec3> positions;
		std::vector<ci::Color> colors;
		std::vector<ci::vec3> normals;
		std::vector<uint32_t> indices;
	};

	typedef std::shared_ptr<BranchAttribs> BranchAttribsRef;

	ci::vec3 getVector();
	ci::vec3 getGlobalBasePosition();
	ci::quat getGlobalOrientation();
	BranchAttribsRef getAttributes();
	std::vector<TreeNode> & getChildrenRef() { return mChildren; };
	// The parameter here seems to be the best type declaration for a lambda function
	void visitBreadthFirst(std::function<void (TreeNode *)> const &);
	// Could also do:
	// template<typename FuncType>
	// void visitBreadthFirst(FuncType visitFunc) {
		// Implementation of visitBreadtFirst, using `visitFunc(node)`
	// };

	void generateChildren(int);
	size_t numChildren() { return mChildren.size(); }

protected:
	ci::quat mOrientation = ci::quat();
	float mPosition = 0.0f;
	float mLength = 1.0f;
	int mLevel = 0;
	float mScale = 1.0f;
	ci::vec3 growthDirection = ci::vec3(0, 1, 0);
	std::vector<TreeNode> mChildren;
	TreeNode * mParent = nullptr;

	// LeafBlob leaves; // TODO: create some leaves
};