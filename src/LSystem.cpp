#include "LSystem.h"

#include "cinder/Rand.h"

#include <iostream>

using namespace cinder;

std::string getNext(char systemPoint) {
	switch (systemPoint) {
		// case 'A' : return Rand::randFloat() < 0.5f ? "LFABRRFABL" : "PFABNNFABP";
		case 'A' : return "LFABRRFABLPFABNNFABP";
		case 'F' : return "F";
		case 'B' : return "B";
		case 'L' : return "L";
		case 'R' : return "R";
		case 'N' : return "N";
		case 'P' : return "P";
		default: return "";
	}
}

std::string iterateSystem(std::string description) {
	std::string nextIteration;
	for (char & systemNode : description) {
		nextIteration += getNext(systemNode);
	}
	return nextIteration;
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

	// Compute the description by iterating the L-System
	std::string treeDescription = mInitialState;

	for (int i = 0; i < mNumIterations; i++) {
		treeDescription = iterateSystem(treeDescription);
	}

	std::cout << "Computed the system:" << std::endl;
	std::cout << treeDescription << std::endl;

	// Generate the mesh from the description
	// positions
	// normals
	// colors
	// indices

	vec3 position(0, 0, 0);
	vec3 heading(0, 1, 0);
	vec3 xaxis(1, 0, 0);
	vec3 zaxis(0, 0, 1);
	float rotationAngle = glm::radians(30.0f);
	float branchLength = 1.0f;

	Color brown = Color("brown");
	int vertexCount = 0;

	for (char & systemNode : treeDescription) {
		if (systemNode == 'A') {
		// Nothing, this is an end branch
		} else if (systemNode == 'F') {
		// Add a forward branch
			mPositions.push_back(position);
			mColors.push_back(brown.get(CM_RGB));
			mNormals.push_back(vec3());
			mIndices.push_back(vertexCount++);

			position = position + branchLength * heading;

			mPositions.push_back(position);
			mColors.push_back(brown.get(CM_RGB));
			mNormals.push_back(vec3());
			mIndices.push_back(vertexCount++);
		} else if (systemNode == 'B') {
		// Move backwards along heading
			position = position - branchLength * heading;
		} else if (systemNode == 'L') {
			heading = angleAxis(rotationAngle, zaxis) * heading;
		} else if (systemNode == 'R') {
			heading = angleAxis(-rotationAngle, zaxis) * heading;
		} else if (systemNode == 'N') {
			heading = angleAxis(rotationAngle, xaxis) * heading;
		} else if (systemNode == 'P') {
			heading = angleAxis(-rotationAngle, xaxis) * heading;
		}
	}

	mCalculationsCached = true;
}

uint8_t LSystem::getAttribDims( geom::Attrib attr ) const {
	switch (attr) {
		case geom::Attrib::POSITION: return 3;
		case geom::Attrib::NORMAL: return 3;
		case geom::Attrib::COLOR: return mHasColors ? 3 : 0;
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
	// std::vector<vec3> tangents;
	// need to calculate tangents
	// target->copyAttrib(geom::Attrib::TANGENT, 3, 0, value_ptr(*(tangents.data())), tangents.size());

	// Assume 4 bytes per vertex index (highest index can be pretty big)
	target->copyIndices(getPrimitive(), mIndices.data(), mIndices.size(), 4);
}

