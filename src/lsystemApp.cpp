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

	CameraPersp theCamera;
	Arcball theArcball;
	float camDistance = 5.0f;

	LSystem theSystem;
	gl::GlslProgRef theProgram;
	gl::BatchRef theBatch;
};

void lsystemApp::setup() {
	theArcball = Arcball(& theCamera, cinder::Sphere(vec3(0, 0, 0), camDistance));

	gl::enableDepthRead();
	gl::enableDepthWrite();

	theSystem = LSystem("FA")
		.colors()
		.iterations(4);

	auto shader = gl::ShaderDef().color();

	theProgram = gl::getStockShader(shader);

	theBatch = gl::Batch::create(theSystem, theProgram);
}

void lsystemApp::update() {
	quat ballQuat = theArcball.getQuat();
	// ballQuat.w *= -1.0f;
	vec3 camTarget = vec3(0, 0, 0);
	vec3 camOffset = ballQuat * vec3(0.0f, 0.0f, camDistance);
	vec3 camEye = camOffset - camTarget;
	// vec3 camEye = camTarget - camOffset;
	vec3 camUp = ballQuat * vec3(0, 1, 0);
	theCamera.lookAt(camEye, camTarget, camUp);
}

void lsystemApp::draw() {
	gl::clear( Color( 0, 0, 0 ) );

	gl::setMatrices(theCamera);

	theBatch->draw();
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
