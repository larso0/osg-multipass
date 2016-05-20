#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/Texture2D>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>
#include <osgGA/GUIEventHandler>
#include <osg/Program>
#include <stdexcept>

const int WINDOW_WIDTH = 512;
const int WINDOW_HEIGHT = 512;

void addQuad(osg::Group* scene, osg::Vec3 position, osg::Vec4 color, osg::Program* program)
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
	colorArray = new osg::Vec4Array();
	colorArray->push_back(color);
	
	quadGeometry->setVertexArray(vertexArray);
	quadGeometry->setColorArray(colorArray);
	quadGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::Geode* geode = new osg::Geode();
	if (program)
	{
		osg::StateSet* stateSet = geode->getOrCreateStateSet();
		stateSet->setAttributeAndModes(program, osg::StateAttribute::ON);
		quadGeometry->setUseDisplayList(false);
		quadGeometry->setUseVertexBufferObjects(true);
		quadGeometry->setVertexAttribArray(3, colorArray);
		quadGeometry->setVertexAttribBinding(3, osg::Geometry::BIND_OVERALL);
	}

	quadGeometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLE_FAN, 0, 4));
	geode->addDrawable(quadGeometry);

	osg::PositionAttitudeTransform* transform = new osg::PositionAttitudeTransform();
	transform->addChild(geode);
	transform->setPosition(position);

	scene->addChild(transform);
}

osg::ref_ptr<osg::Group> createScene(osg::Program* program)
{
	osg::ref_ptr<osg::Group> group = new osg::Group();
	addQuad(group.get(), osg::Vec3(-0.25f, 0.f, 0.f), osg::Vec4(1.f, 0.f, 0.f, 0.5f), program);
	addQuad(group.get(), osg::Vec3(0.25f, 0.f, 0.25f), osg::Vec4(0.f, 1.f, 0.f, 0.3f), program);
	addQuad(group.get(), osg::Vec3(0.f, 0.25f, -0.25f), osg::Vec4(0.f, 0.f, 1.f, 0.36f), program);
	return group;
}

class TextureResizeHandler : public osgGA::GUIEventHandler
{
private:
	osg::Texture2D* texture;
	osg::Camera* texCamera;
	osg::Camera* mainCamera;
public:
	TextureResizeHandler(osg::Texture2D* texture,
						 osg::Camera* texCamera,
						 osg::Camera* mainCamera) :
	texture(texture), texCamera(texCamera), mainCamera(mainCamera)
	{
		if(!(texture && texCamera && mainCamera))
			throw std::invalid_argument("Recieved nullptr.");
	}

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&) override
	{
		if(ea.getEventType() == osgGA::GUIEventAdapter::RESIZE)
		{
			int w = ea.getWindowWidth(), h = ea.getWindowHeight();
			texture->setTextureSize(w, h);
			texture->dirtyTextureObject();
			texCamera->setRenderingCache(0);
			texCamera->setViewport(0, 0, w, h);
			texCamera->setProjectionMatrix(mainCamera->getProjectionMatrix());
		}

		return false;
	}
};

osg::ref_ptr<osg::Program> loadBlendProgram()
{
	osg::ref_ptr<osg::Program> program = new osg::Program();
	osg::ref_ptr<osg::Shader> vShader = osg::Shader::readShaderFile(osg::Shader::VERTEX, "shaders/blendVertexShader.glsl");
	osg::ref_ptr<osg::Shader> fShader = osg::Shader::readShaderFile(osg::Shader::FRAGMENT, "shaders/blendFragmentShader.glsl");
	program->addShader(vShader);
	program->addShader(fShader);
	program->addBindAttribLocation("pos", 0);
	program->addBindAttribLocation("color", 3);
	return program;
}

int main(int argc, char** argv)
{
	osg::ref_ptr<osg::Camera> texCamera = new osg::Camera();
	texCamera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
	texCamera->setRenderOrder(osg::Camera::POST_RENDER);
	texCamera->setProjectionMatrixAsPerspective(60.0, 1.0, 0.1, 100.0);
	texCamera->setViewMatrixAsLookAt(osg::Vec3(0.f, 1.f, 4.f), osg::Vec3(0.f, 0.f, 0.f), osg::Vec3(0.f, 1.f, 0.f));
	texCamera->setViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	texCamera->setClearColor(osg::Vec4(0.5f, 0.5f, 0.5f, 0.f));
	texCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
	texture->setTextureSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	texture->setSourceFormat(GL_RGBA);
	texture->setSourceType(GL_UNSIGNED_BYTE);
	texture->setInternalFormat(GL_RGBA8);
	texCamera->attach(osg::Camera::COLOR_BUFFER, texture.get());

	osg::ref_ptr<osg::Program> blendProgram = loadBlendProgram();

	osg::ref_ptr<osg::Group> scene = new osg::Group();
	osg::ref_ptr<osg::Group> transparentScene = createScene(blendProgram.get());
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

	viewer.realize();
	osg::Camera* mainCamera = viewer.getCamera();
	osg::State* state = mainCamera->getGraphicsContext()->getState();
	state->setUseModelViewAndProjectionUniforms(true);
	state->setUseVertexAttributeAliasing(true);
	viewer.addEventHandler(new TextureResizeHandler(texture, texCamera, mainCamera));
	texCamera->setViewMatrix(mainCamera->getViewMatrix());
	texCamera->setProjectionMatrix(mainCamera->getProjectionMatrix());
	while (!viewer.done())
	{		
		texCamera->setViewMatrix(mainCamera->getViewMatrix());
		viewer.frame();
	}
	return 0;
}