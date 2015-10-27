#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Arcball.h"

#include "LSystem.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class lsystemApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;
	void mouseDown(MouseEvent) override;
	void mouseDrag(MouseEvent) override;
	void keyDown(KeyEvent) override;

	void redoSystem();

	// TODO: Use the camera from the Frustum Culling sample - it's really nice
	CameraPersp theCamera;
	Arcball theArcball;
	// float camDistance = 100.0f;
	// vec3 camTarget = vec3(0, 0, 0);

	float camDistance = 10.0f;
	vec3 camTarget = vec3(0, 0, 0);
	int numTreesX = 0;
	int numTreesZ = 0;

	LSystem theSystem;
	std::vector<gl::BatchRef> theBatches;
};

void lsystemApp::setup() {
	theArcball = Arcball(& theCamera, cinder::Sphere(camTarget, camDistance * 2.f / 3.f));

	gl::enableDepthRead();
	gl::enableDepthWrite();

	gl::enableFaceCulling();
	gl::cullFace(GL_BACK);

	redoSystem();
}

void lsystemApp::keyDown(KeyEvent event) {
	if (event.getCode() == ci::app::KeyEvent::KEY_SPACE) {
		redoSystem();
	}
}

void lsystemApp::redoSystem() {
	theSystem = LSystem()
		.iterations(4);

	theSystem.computeSystem();

	auto shaderProgram = gl::GlslProg::create(loadResource("tree_flat_shader.vert"), loadResource("tree_flat_shader.frag"));

	theBatches.clear();
	theBatches = std::vector<gl::BatchRef>();

	for (int xmove = -numTreesX; xmove <= numTreesX; xmove++) {
		for (int zmove = -numTreesZ; zmove <= numTreesZ; zmove++) {
			gl::BatchRef iterBatch = gl::Batch::create(theSystem >> geom::Translate((float) xmove * 6.0f, 0.f, (float) zmove * 6.0f), shaderProgram);
			theBatches.push_back(iterBatch);
		}
	}
}

void lsystemApp::update() {
	quat ballQuat = theArcball.getQuat();
	vec3 camOffset = ballQuat * vec3(0, 0, camDistance);
	vec3 camEye = camTarget + camOffset;
	vec3 camUp = ballQuat * vec3(0, 1, 0);
	theCamera.lookAt(camEye, camTarget, camUp);
}

void lsystemApp::draw() {
	gl::clear( Color( 0, 0, 0 ) );

	gl::setMatrices(theCamera);

	for (auto treeBatch : theBatches) {
		treeBatch->draw();
	}
}

void lsystemApp::mouseDown(MouseEvent evt) {
	// Update the Arcball model
	theArcball.mouseDown(evt);
}

void lsystemApp::mouseDrag(MouseEvent evt) {
	// Update the Arcball model
	theArcball.mouseDrag(evt);
}

CINDER_APP( lsystemApp, RendererGl )
