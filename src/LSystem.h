#pragma once

#include <string>
#include <vector>

#include "cinder/GeomIo.h"

// System description
// A : split-trunk node (this is a bud - it turns into more trunk)
// F : solid-trunk forward step (move forward 1 unit along heading)
// B : backward step (move backward 1 unit along heading)
// L : fork left (rotate heading 30deg around Z-axis)
// R : fork right (rotate heading -30deg around Z-axis)
// N : fork anterior (rotate heading 30deg around X-axis)
// P : fork posterior (rotate heading -30deg around X-axis)
// 
// A -> LFABRRFABL
// F -> F
// B -> B
// L -> L
// R -> R
// 
// Start: FA
// 1: FLFABRRFABL
// 2: FLFLFABRRFABLBRRFLFABRRFABLBL
// 3: FLFLFLFABRRFABLBRRFLFABRRFABLBLBRRFLFLFABRRFABLBRRFLFABRRFABLBLBL

class LSystem : public ci::geom::Source {
public:
	LSystem();
	LSystem(std::string const &);

	LSystem & colors(bool enable = true) { mHasColors = enable; return *this; }
	LSystem & initialState(std::string trunk) { mInitialState = trunk; return *this; }
	LSystem & initialState(char const * trunk) { mInitialState = trunk; return *this; }
	LSystem & iterations(int iters) { mNumIterations = iters; return *this; }

	size_t getNumVertices() const override { computeSystem(); return mPositions.size(); };
	size_t getNumIndices() const override { computeSystem(); return mIndices.size(); };
	ci::geom::Primitive getPrimitive() const override { return ci::geom::Primitive::LINES; }
	uint8_t getAttribDims( ci::geom::Attrib attr ) const override;
	ci::geom::AttribSet getAvailableAttribs() const override;
	void loadInto( ci::geom::Target *target, const ci::geom::AttribSet &requestedAttribs ) const override;
	LSystem* clone() const override { return new LSystem( *this ); }

protected:
	void computeSystem() const;

	std::string mInitialState;
	int mNumIterations;
	bool mHasColors;

	mutable bool mCalculationsCached;

	mutable std::vector<ci::vec3> mPositions, mNormals, mColors;
	mutable std::vector<uint32_t> mIndices;
};