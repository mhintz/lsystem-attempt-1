#pragma once

#include <vector>
#include <functional>

class TreeNode {
public:
	TreeNode & orientation(ci::quat const & _orientation) { mOrientation = _orientation; return *this; }
	TreeNode & position(float _position) { mPosition = _position; return *this; }
	TreeNode & length(float _length) { mLength = _length; return *this; }
	TreeNode & diameter(float _diameter) { mDiameter = _diameter; return *this; }
	TreeNode & level(int _level) { mLevel = _level; return *this; }
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

	float getSuperTreeLengthSum();
	float getSubTreeLengthSum();
	float getDiameter() { return mDiameter; };

	BranchAttribsRef getAttributes();
	std::vector<TreeNode> & getChildrenRef() { return mChildren; };

	// The parameter here seems to be the best type declaration for a lambda function
	void visitBreadthFirst(std::function<void (TreeNode *)> const &);
	void visitSuperTree(std::function<void (TreeNode *)> const &);
	// Using a template and letting the compiler deduce the type information is also a
	// valid way to declare a function taking a lambda
	template<typename ReducerType, typename FuncType>
	ReducerType reduceBreadthFirst(ReducerType, FuncType);

	template <typename ReducerType, typename FuncType>
	ReducerType reduceSuperTree(ReducerType, FuncType);

	void generateChildren();
	size_t numChildren() { return mChildren.size(); }

	friend float lengthReducer(float memo, TreeNode * node) {
		return memo + node->mLength;
	}

protected:
	ci::quat mOrientation = ci::quat();
	float mPosition = 1.0f;
	float mLength = 1.0f;
	float mDiameter = 1.0f;
	int mLevel = 0;
	ci::vec3 growthDirection = ci::vec3(0, 1, 0);
	std::vector<TreeNode> mChildren;
	TreeNode * mParent = nullptr; // Raw, weak pointer

	// LeafBlob leaves; // TODO: create some leaves
};

template<typename ReducerType, typename FuncType>
ReducerType TreeNode::reduceBreadthFirst(ReducerType reducerValue, FuncType reduceFunc) {
	this->visitBreadthFirst([&] (TreeNode * node) {
		reducerValue = reduceFunc(reducerValue, node);
	});
	return reducerValue;
}

template <typename ReducerType, typename FuncType>
ReducerType TreeNode::reduceSuperTree(ReducerType reducerValue, FuncType reduceFunc) {
	this->visitSuperTree([&] (TreeNode * node) {
		reducerValue = reduceFunc(reducerValue, node);
	});
	return reducerValue;
}