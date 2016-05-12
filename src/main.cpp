#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/Texture2D>
#include <osg/Geometry>
#include <osg/Geode>

const int TEXTURE_WIDTH = 512;
const int TEXTURE_HEIGHT = 512;

int main(int argc, char** argv)
{
	osg::ref_ptr<osg::Camera> texCamera = new osg::Camera();
	texCamera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
	texCamera->setRenderOrder(osg::Camera::PRE_RENDER);
	texCamera->setProjectionMatrixAsPerspective(60.0, 1.0, 0.1, 100.0);
	texCamera->setViewMatrixAsLookAt(osg::Vec3(0.f, 1.f, 10.f), osg::Vec3(0.f, 0.f, 0.f), osg::Vec3(0.f, 1.f, 0.f));
	texCamera->setViewport(0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT);
	texCamera->setClearColor(osg::Vec4(0.5f, 0.5f, 0.5f, 1.f));
	texCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	osg::ref_ptr<osg::Image> image = new osg::Image();
	image->allocateImage(TEXTURE_WIDTH, TEXTURE_HEIGHT, 1, GL_RGBA, GL_UNSIGNED_BYTE);
	image->setInternalTextureFormat(GL_RGBA8);
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(image);
	texCamera->attach(osg::Camera::COLOR_BUFFER, texture.get());

	osg::ref_ptr<osg::Node> teapot = osgDB::readRefNodeFile("teapot.osg");
	texCamera->addChild(teapot.get());

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

	osg::ref_ptr<osg::Group> scene = new osg::Group();
	scene->addChild(texCamera);
	scene->addChild(quad);

	osgViewer::Viewer viewer;
	viewer.setSceneData(scene.get());
	return viewer.run();
}