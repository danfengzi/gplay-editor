#include "GplayDevice.h"

#include <spark/SPARK.h>


#include "node-editor/common/BaseNode.h"

#include <imgui/imgui.h>


#include "gp3d/helpers/Events.h"

#include "gp3d/QtImGui.h"

#include "node-editor/spark-nodes/SpkRenderer.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------
// GPDevice
//-----------------------------------------------------------------------------------------------------------------------------------------------

#include "gp3d/GPRenderer.h"
ISubRenderer* _curentSubRenderer;


GplayDeviceGame::GplayDeviceGame() :
    _platform(nullptr)
    //_scene(nullptr)
   // _isShowDebug(true)
{

}

GplayDeviceGame::~GplayDeviceGame()
{

}

void GplayDeviceGame::createRenderWindow(void* hwnd)
{
    _platform = gplay::Platform::create(this, hwnd);
    GP_ASSERT(_platform);
    _platform->start();

    // create default view
    View::create(0, Rectangle(200, 200), View::ClearFlags::COLOR_DEPTH, 0x556677ff, 1.0f, 0);


    _curentSubRenderer = new SpkRenderer();//OpClassNode_Renderer();
}

void GplayDeviceGame::runFrame()
{
    // begin frame
    Renderer::getInstance().beginFrame();
    QtImGui::newFrame();

    // call gplay frame that will invoke update and render methods.
    frame();

    // end frame
    ImGui::Render();
    Renderer::getInstance().endFrame();
}

void GplayDeviceGame::stop()
{
    _platform->stop();
}

void GplayDeviceGame::keyEvent(Keyboard::KeyEvent evt, int key)
{
    // send key event
    std::shared_ptr<KeyEvent> keyEvent = KeyEvent::create();
    keyEvent.get()->event = evt;
    keyEvent.get()->key = key;
    EventManager::getInstance()->queueEvent(keyEvent);
}

bool GplayDeviceGame::mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta)
{
    // when right button is pressed set on mouse captured to interact with fps camera
    if(evt == Mouse::MOUSE_PRESS_RIGHT_BUTTON)
        setMouseCaptured(true);
    else if(evt == Mouse::MOUSE_RELEASE_RIGHT_BUTTON)
        setMouseCaptured(false);

    // send mouse event
    std::shared_ptr<MouseEvent> mouseEvent = MouseEvent::create();
    mouseEvent.get()->event = evt;
    mouseEvent.get()->mousePos = Vector2(x, y);
    EventManager::getInstance()->queueEvent(mouseEvent);

    return true;
}

void GplayDeviceGame::resizeRenderView(int width, int height)
{
    // resize game engine window
    _platform->setWindowSize(width, height);

    // resize default view
    View::getView(0)->setViewRect(Rectangle(width, height));

    // resize current renderer
    _curentSubRenderer->resize(width, height);
}

void GplayDeviceGame::initialize()
{
   /* // Create a new empty scene.
    _scene = Scene::create();

    // create camera
    Camera* camera = Camera::createPerspective(45.0f, 1.77f, 0.1f, 100.0f);
    Node* cameraNode = _scene->addNode("camera");
    cameraNode->setCamera(camera);
    cameraNode->setTranslation(Vector3(0,0,10));
    _scene->setActiveCamera(camera);*/
}

void GplayDeviceGame::finalize()
{

}

void GplayDeviceGame::update(float elapsedTime)
{
    _curentSubRenderer->update(elapsedTime);
}

void GplayDeviceGame::render(float elapsedTime)
{
    bgfx::touch(0);

    _curentSubRenderer->render(elapsedTime);

    //View::getView(0)->bind();
   // _scene->visit(this, &GplayDeviceGame::drawScene);
}

/*bool GplayDeviceGame::drawScene(Node* node)
{
    Drawable* drawable = node->getDrawable();
    if (drawable)
        drawable->draw();
    return true;
}*/

/*
void createDebugGeomteriesFromZone(const SPK::Ref<SPK::Zone> zone)
{
    // every zone has a position
    const SPK::Vector3D pos = zone->getPosition();

    if(zone->getClassName() == "Point")
    {
        DebugDraw::getInstance()->drawSphere(Vector3(pos.x, pos.y, pos.z), 0.1f, Vector3(1,1,1));
    }
    else if(zone->getClassName() == "Sphere")
    {
        const SPK::Sphere* sphere = dynamic_cast<SPK::Sphere*>(zone.get());
        GP_ASSERT(sphere);
        float radius = sphere->getRadius();
        DebugDraw::getInstance()->drawSphere(Vector3(pos.x, pos.y, pos.z), radius, Vector3(1,1,1));
    }
    else if(zone->getClassName() == "Plane")
    {
        const SPK::Plane* plane = dynamic_cast<SPK::Plane*>(zone.get());
        GP_ASSERT(plane);
        const SPK::Vector3D normal = plane->getNormal();
        DebugDraw::getInstance()->drawPlane(Vector3(normal.x, normal.y, normal.z), 0.0f, Matrix::identity(), Vector3(1,1,1));
    }
    else if(zone->getClassName() == "Box")
    {
        const SPK::Box* box = dynamic_cast<SPK::Box*>(zone.get());
        GP_ASSERT(box);
        Vector3 scale = ToGplayVector3(box->getDimensions());
        Matrix matrix;
        // todo: fix rotation
        Matrix::createTranslation(ToGplayVector3(pos), &matrix);
        BoundingBox bbox(-scale/2.0f, scale/2.0f);
        bbox *= matrix;
        DebugDraw::getInstance()->drawBox(bbox.min, bbox.max, Vector3(1,1,1));
    }
    else if(zone->getClassName() == "Cylinder")
    {
        const SPK::Cylinder* cylinder = dynamic_cast<SPK::Cylinder*>(zone.get());
        GP_ASSERT(cylinder);

        const SPK::Vector3D axis = cylinder->getAxis();
        float height = cylinder->getHeight();
        float radius = cylinder->getRadius();

        // todo: fix rotation
        Matrix matrix;
        Matrix::createTranslation(ToGplayVector3(pos), &matrix);
        DebugDraw::getInstance()->drawCylinder(radius, height/2.0f, 1, matrix, Vector3(1,1,1));
    }
    else if(zone->getClassName() == "Ring")
    {
        const SPK::Ring* ring = dynamic_cast<SPK::Ring*>(zone.get());
        GP_ASSERT(ring);

        float minRadius = ring->getMinRadius();
        float maxRadius = ring->getMaxRadius();

        // todo: fix rotation

        DebugDraw::getInstance()->drawArc(ToGplayVector3(pos), Vector3(0,1,0), Vector3(1,0,0), minRadius, minRadius, 0.0f, MATH_DEG_TO_RAD(360.0f), Vector3(1,1,1), false);
        DebugDraw::getInstance()->drawArc(ToGplayVector3(pos), Vector3(0,1,0), Vector3(1,0,0), maxRadius, maxRadius, 0.0f, MATH_DEG_TO_RAD(360.0f), Vector3(1,1,1), false);
    }
}

void drawDebugShapes(SparkParticleEmitter* spkEffect, Scene* scene)
{
    DebugDraw::getInstance()->begin(scene->getActiveCamera()->getViewProjectionMatrix());

    SPK::Ref<SPK::System> spkSystem = spkEffect->getSparkSystem();
    for(size_t nGroup = 0; nGroup < spkSystem->getNbGroups(); nGroup++)
    {
        // show emitters zones
        for(size_t nEmitter = 0; nEmitter < spkSystem->getGroup(nGroup)->getNbEmitters(); nEmitter++)
        {
            const SPK::Ref<SPK::Zone> zone = spkSystem->getGroup(nGroup)->getEmitter(nEmitter)->getZone();
            createDebugGeomteriesFromZone(zone);
        }

        // show modifiers zones
        for(size_t nModifiers = 0; nModifiers < spkSystem->getGroup(nGroup)->getNbModifiers(); nModifiers++)
        {
            const SPK::Ref<SPK::Modifier> modifier = spkSystem->getGroup(nGroup)->getModifier(nModifiers);

            if(modifier->getClassName() == "PointMass")
            {
                const SPK::PointMass* pointMass = dynamic_cast<SPK::PointMass*>(modifier.get());
                GP_ASSERT(pointMass);
                const SPK::Vector3D pos = pointMass->getPosition();
                DebugDraw::getInstance()->drawSphere(Vector3(pos.x, pos.y, pos.z), 0.25f, Vector3(0,1,0));
            }
            else if(modifier->getClassName() == "Destroyer")
            {
                const SPK::Destroyer* destroyer = dynamic_cast<SPK::Destroyer*>(modifier.get());
                GP_ASSERT(destroyer);
                createDebugGeomteriesFromZone(destroyer->getZone());
            }
            else if(modifier->getClassName() == "Obstacle")
            {
                const SPK::Obstacle* obstacle = dynamic_cast<SPK::Obstacle*>(modifier.get());
                GP_ASSERT(obstacle);
                createDebugGeomteriesFromZone(obstacle->getZone());
            }
            else if(modifier->getClassName() == "LinearForce")
            {
                const SPK::LinearForce* linearForce = dynamic_cast<SPK::LinearForce*>(modifier.get());
                GP_ASSERT(linearForce);
                createDebugGeomteriesFromZone(linearForce->getZone());
            }
        }
    }

    DebugDraw::getInstance()->end();
}


bool GplayDeviceGame::updateEmitters(Node* node, float elapsedTime)
{
    SparkParticleEmitter* spkEffect = dynamic_cast<SparkParticleEmitter*>(node->getDrawable());
    if (spkEffect)
        spkEffect->update(elapsedTime);

    // show debug shapes for all zones in spark system
    if(spkEffect && _isShowDebug)
    {
        drawDebugShapes(spkEffect, _scene);
    }

    return true;
}*/

void GplayDeviceGame::setCurentParticleSystem(SPK::Ref<SPK::System> sparkSystem)
{
    SpkRenderer* _spkRenderer = dynamic_cast<SpkRenderer*>(_curentSubRenderer);
    if(_spkRenderer)
        _spkRenderer->setCurentParticleSystem(sparkSystem);

    /*Node* node = _scene->findNode("sparkSystem");
    if(node)
        _scene->removeNode(node);

    // Create a node in scene and attach spark foutain effect
    SparkParticleEmitter* foutainEmitter = SparkParticleEmitter::createRef(sparkSystem, true);
    Node* particleNode = Node::create("sparkSystem");
    particleNode->setDrawable(foutainEmitter);
    particleNode->setTranslation(0.0f, 0.0f, 0.0f);

    _scene->addNode(particleNode);*/
}
