#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/Texture2D>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>

const int WINDOW_WIDTH = 512;
const int WINDOW_HEIGHT = 512;

void addQuad(osg::Group* scene, osg::Vec3 position, osg::Vec4 color)
{
	if (!scene) return;

	static osg::ref_ptr<osg::Vec3Array> vertexArray;
	if (!vertexArray)
	{
		vertexArray = new osg::Vec3Array();
		vertexArray->push_back(osg::Vec3(-1.f, -1.f, 0.f));
		vertexArray->push_back(osg::Vec3(1.f, -1.f, 0.f));
		vertexArray->push_back(osg::Vec3(1.f, 1.f, 0.f));
		vertexArray->push_back(osg::Vec3(-1.f, 1.f, 0.f));
	}

	osg::ref_ptr<osg::Vec4Array> colorArray;
	osg::ref_ptr<osg::Geometry> quadGeometry;
	quadGeometry = new osg::Geometry();
	quadGeometry->setVertexArray(vertexArray);
	colorArray = new osg::Vec4Array();
	colorArray->push_back(color);
	quadGeometry->setColorArray(colorArray);
	quadGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	quadGeometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLE_FAN, 0, 4));
	
	osg::Geode* geode = new osg::Geode();
	geode->addDrawable(quadGeometry);

	osg::PositionAttitudeTransform* transform = new osg::PositionAttitudeTransform();
	transform->addChild(geode);
	transform->setPosition(position);

	scene->addChild(transform);
}

osg::ref_ptr<osg::Group> createScene()
{
	osg::ref_ptr<osg::Group> group = new osg::Group();
	addQuad(group.get(), osg::Vec3(-0.25f, 0.f, 0.f), osg::Vec4(1.f, 0.f, 0.f, 0.5f));
	addQuad(group.get(), osg::Vec3(0.25f, 0.f, 0.25f), osg::Vec4(0.f, 1.f, 0.f, 0.3f));
	addQuad(group.get(), osg::Vec3(0.f, 0.25f, -0.25f), osg::Vec4(0.f, 0.f, 1.f, 0.36f));
	return group;
}

int main(int argc, char** argv)
{
	osg::ref_ptr<osg::Camera> texCamera = new osg::Camera();
	texCamera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
	texCamera->setRenderOrder(osg::Camera::POST_RENDER);
	texCamera->setProjectionMatrixAsPerspective(60.0, 1.0, 0.1, 100.0);
	texCamera->setViewMatrixAsLookAt(osg::Vec3(0.f, 1.f, 4.f), osg::Vec3(0.f, 0.f, 0.f), osg::Vec3(0.f, 1.f, 0.f));
	texCamera->setViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	texCamera->setClearColor(osg::Vec4(0.5f, 0.5f, 0.5f, 1.f));
	texCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	osg::ref_ptr<osg::Image> image = new osg::Image();
	image->allocateImage(WINDOW_WIDTH, WINDOW_HEIGHT, 1, GL_RGBA, GL_UNSIGNED_BYTE);
	image->setInternalTextureFormat(GL_RGBA8);
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(image);
	texCamera->attach(osg::Camera::COLOR_BUFFER, texture.get());

	osg::ref_ptr<osg::Group> scene = new osg::Group();
	osg::ref_ptr<osg::Group> transparentScene = createScene();
	texCamera->addChild(transparentScene);

	osg::ref_ptr<osg::Geode> quad = new osg::Geode();
	osg::ref_ptr<osg::Geometry> quadGeometry = new osg::Geometry();
	quad->addDrawable(quadGeometry);

	osg::ref_ptr<osg::Vec3Array> vertexArray = new osg::Vec3Array();
	vertexArray->push_back(osg::Vec3(-1.f, -1.f, 0.f));
	vertexArray->push_back(osg::Vec3(1.f, -1.f, 0.f));
	vertexArray->push_back(osg::Vec3(1.f, 1.f, 0.f));
	vertexArray->push_back(osg::Vec3(-1.f, 1.f, 0.f));
	quadGeometry->setVertexArray(vertexArray);

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array();
	colorArray->push_back(osg::Vec4(1.f, 1.f, 1.f, 1.f));
	quadGeometry->setColorArray(colorArray);
	quadGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::ref_ptr<osg::Vec2Array> texCoordArray = new osg::Vec2Array();
	texCoordArray->push_back(osg::Vec2(0.f, 0.f));
	texCoordArray->push_back(osg::Vec2(1.f, 0.f));
	texCoordArray->push_back(osg::Vec2(1.f, 1.f));
	texCoordArray->push_back(osg::Vec2(0.f, 1.f));
	quadGeometry->setTexCoordArray(0, texCoordArray);

	quadGeometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLE_FAN, 0, 4));
	quad->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

	osg::ref_ptr<osg::Group> quadGroup = new osg::Group();
	quadGroup->addChild(texCamera);
	quadGroup->addChild(quad);

	osgViewer::Viewer viewer;
	viewer.setUpViewInWindow(100, 100, WINDOW_WIDTH, WINDOW_HEIGHT);
	viewer.setSceneData(scene);
	viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	viewer.addSlave(texCamera, false);

	osg::ref_ptr<osg::Camera> quadCamera = new osg::Camera();
	quadCamera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
	quadCamera->setRenderOrder(osg::Camera::POST_RENDER);
	quadCamera->setViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	quadCamera->setViewMatrix(osg::Matrixd());
	quadCamera->setProjectionMatrix(osg::Matrixd());
	quadCamera->setClearMask(0);
	quadCamera->addChild(quadGroup);
	scene->addChild(quadCamera);

	//viewer.addSlave(quadCamera, false);
	viewer.realize();
	while (!viewer.done())
	{
		osg::Camera* mainCamera = viewer.getCamera();
		texCamera->setViewMatrix(mainCamera->getViewMatrix());
		texCamera->setProjectionMatrix(mainCamera->getProjectionMatrix());
		viewer.frame();
	}
	return 0;
}