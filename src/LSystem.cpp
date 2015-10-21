#include "LSystem.h"

using namespace cinder;

std::string iterateSystem(std::string description) {
	return "";
}

std::string getNext(char systemPoint) {
	switch (systemPoint) {
		case 'A' : return "LFABRRFABL";
		case 'F' : return "F";
		case 'B' : return "B";
		case 'L' : return "L";
		case 'R' : return "R";
		default: return "";
	}
}

LSystem::LSystem() :
	mInitialState(std::string()), mNumIterations(3), mHasColors(false), mCalculationsCached(false)
{
}

LSystem::LSystem(std::string const & trunk) :
	mInitialState(trunk), mNumIterations(3), mHasColors(false), mCalculationsCached(false)
{
}

void LSystem::computeSystem() const
{
	if (mCalculationsCached) {
		return;
	}



	mCalculationsCached = true;
}

uint8_t LSystem::getAttribDims( geom::Attrib attr ) const {
	switch (attr) {
		case geom::Attrib::POSITION: return 3;
		case geom::Attrib::NORMAL: return 3;
		case geom::Attrib::TANGENT: return 3;
		case geom::Attrib::COLOR: return mHasColors ? 3 : 0;
		default:
			return 0;
	}
}

geom::AttribSet LSystem::getAvailableAttribs() const {
	return { geom::Attrib::POSITION, geom::Attrib::NORMAL, geom::Attrib::COLOR, geom::Attrib::TANGENT };
}

void LSystem::loadInto( geom::Target *target, const geom::AttribSet &requestedAttribs ) const {
	computeSystem();

	// Copy attributes over to the target
	target->copyAttrib(geom::Attrib::POSITION, 3, 0, value_ptr(*(mPositions.data())), mPositions.size());
	target->copyAttrib(geom::Attrib::NORMAL, 3, 0, value_ptr(*(mNormals.data())), mNormals.size());
	target->copyAttrib(geom::Attrib::COLOR, 3, 0, value_ptr(*(mColors.data())), mColors.size());

	// std::vector<vec3> tangents;
	// target->copyAttrib(geom::Attrib::TANGENT, 3, 0, value_ptr(*(tangents.data())), tangents.size());

	// 4 required bytes per index
	target->copyIndices(getPrimitive(), mIndices.data(), mIndices.size(), 4);
}

