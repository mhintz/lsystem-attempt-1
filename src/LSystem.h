#pragma once

#include <vector>

#include "cinder/GeomIo.h"

#include "TreeNode.h"

// L-System elements description
// A : split-trunk node (this is a bud - it turns into a branched trunk with buds at each end)
// F : solid-trunk forward step (move forward 1 unit along heading)
// B : backward step (move backward 1 unit along heading)
// L : fork left (rotate heading 30deg around Z-axis)
// R : fork right (rotate heading -30deg around Z-axis)
// N : fork anterior (rotate heading 30deg around X-axis) (not yet implemented)
// P : fork posterior (rotate heading -30deg around X-axis) (not yet implemented)
// 
// Transformation rules
// A -> LFABRRFABL
// F -> F
// B -> B
// L -> L
// R -> R
// 
// Example:
// Start: FA
// Iteration 1: FLFABRRFABL
// Iteration 2: FLFLFABRRFABLBRRFLFABRRFABLBL
// Iteration 3: FLFLFLFABRRFABLBRRFLFABRRFABLBLBRRFLFLFABRRFABLBRRFLFABRRFABLBLBL

class LSystem : public ci::geom::Source {
public:
	// Initialization functions
	LSystem & iterations(int iters) { invalidate(); mNumIterations = iters; return *this; }

	// Overloads for ci::geom::Source
	size_t getNumVertices() const override { computeSystem(); return mPositions.size(); };
	size_t getNumIndices() const override { computeSystem(); return mIndices.size(); };
	ci::geom::Primitive getPrimitive() const override { return ci::geom::Primitive::TRIANGLES; }
	uint8_t getAttribDims( ci::geom::Attrib attr ) const override;
	ci::geom::AttribSet getAvailableAttribs() const override;
	void loadInto( ci::geom::Target *target, const ci::geom::AttribSet &requestedAttribs ) const override;
	LSystem* clone() const override { return new LSystem( *this ); /* Uses copy-constructor */ }

	// Actually computes the system. Has to be "const", because ci::geom::Source requires const overrides
	// In fact, this function does more mutation of this object than any other... :/
	void computeSystem() const;

protected:
	int mNumIterations = 3;

	void invalidate() { mCalculationsCached = false; }

	mutable bool mCalculationsCached = false;

	mutable std::vector<ci::vec3> mPositions;
	mutable std::vector<ci::vec3> mNormals;
	mutable std::vector<ci::vec3> mColors;
	mutable std::vector<uint32_t> mIndices;
};