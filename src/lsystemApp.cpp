#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"

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

	CameraPersp theCamera;
	CameraUi theCamUi;

	int numTreesX = 0;
	int numTreesZ = 0;

	LSystem theSystem;
	std::vector<gl::BatchRef> theBatches;
};

void lsystemApp::setup() {
	setWindowSize(1000, 700);

	theCamUi = CameraUi(& theCamera, getWindow());

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
		.iterations(3);

	theSystem.computeSystem();

	auto shaderProgram = gl::GlslProg::create(loadResource("tree_flat_shader.vert"), loadResource("tree_flat_shader.frag"));

	theBatches.clear();

	for (int xmove = -numTreesX; xmove <= numTreesX; xmove++) {
		for (int zmove = -numTreesZ; zmove <= numTreesZ; zmove++) {
			gl::BatchRef iterBatch = gl::Batch::create(theSystem >> geom::Translate((float) xmove * 6.0f, 0.f, (float) zmove * 6.0f), shaderProgram);
			theBatches.push_back(iterBatch);
		}
	}
}

void lsystemApp::update() {

}

void lsystemApp::draw() {
	gl::clear( Color( 0, 0, 0 ) );

	gl::setMatrices(theCamera);

	for (auto treeBatch : theBatches) {
		treeBatch->draw();
	}
}

void lsystemApp::mouseDown(MouseEvent evt) {

}

void lsystemApp::mouseDrag(MouseEvent evt) {

}

CINDER_APP( lsystemApp, RendererGl )
