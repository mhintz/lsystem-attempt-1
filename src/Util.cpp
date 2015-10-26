#include "Util.h"

using namespace cinder;
using std::vector;

namespace util {

vector<vec3> calculateNormals(vector<vec3> const & positions, vector<uint32_t> const & indices) {
	vector<vec3> normals = vector<vec3>(positions.size());

	for (auto iptr = indices.cbegin(); iptr != indices.cend(); iptr += 3) {
		uint32_t i1 = *(iptr);
		uint32_t i2 = *(iptr + 1);
		uint32_t i3 = *(iptr + 2);
		vec3 side1 = positions.at(i1) - positions.at(i2);
		vec3 side2 = positions.at(i1) - positions.at(i3);
		vec3 norm = normalize(cross(side1, side2));
		normals.at(i1) += norm;
		normals.at(i2) += norm;
		normals.at(i3) += norm;
	}

	for (auto & norm : normals) {
		norm = normalize(norm);
	}

	return normals;
}

} // namespace util
