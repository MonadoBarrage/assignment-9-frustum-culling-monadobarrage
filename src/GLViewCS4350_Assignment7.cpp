#include "GLViewCS4350_Assignment7.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "WOImGui.h" //GUI Demos also need to #include "AftrImGuiIncludes.h"
#include "AftrImGuiIncludes.h"
#include "AftrGLRendererBase.h"
#include "AftrGeometryFrustum.h"
#include "MGLFrustum.h"
#include "CameraLockedGlobal.h"

using namespace Aftr;

static int visibleObjects = 0;
static std::string visibleObjectsDesc = "";
static std::string allModels = "";
GLViewCS4350_Assignment7* GLViewCS4350_Assignment7::New( const std::vector< std::string >& args )
{
   GLViewCS4350_Assignment7* glv = new GLViewCS4350_Assignment7( args );
   glv->init( Aftr::GRAVITY, Vector( 0, 0, -1.0f ), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE );
   glv->onCreate();
   return glv;
}

bool check(float first, float last, float checkTemp)
{
   if( checkTemp >= first && checkTemp <= last)
   {
      return true;
   }
   return false;
   
}

bool checkAll(Vector min, Vector max, Aftr::Camera* cam)
{

   return 
      cam->getAftrGeometryFrustum().isInFrustum(Vector(min[0],min[1],min[2])) &&
      cam->getAftrGeometryFrustum().isInFrustum(Vector(min[0],min[1],max[2])) &&

      cam->getAftrGeometryFrustum().isInFrustum(Vector(min[0],max[1],min[2])) &&
      cam->getAftrGeometryFrustum().isInFrustum(Vector(min[0],max[1],max[2])) &&

      cam->getAftrGeometryFrustum().isInFrustum(Vector(max[0],min[1],min[2])) &&
      cam->getAftrGeometryFrustum().isInFrustum(Vector(max[0],min[1],max[2])) &&

      cam->getAftrGeometryFrustum().isInFrustum(Vector(max[0],max[1],min[2])) &&
      cam->getAftrGeometryFrustum().isInFrustum(Vector(max[0],max[1],max[2]));


}

void GLViewCS4350_Assignment7::fustrumUpdate(Aftr::Camera* camera)
{
   visibleObjects = 0;
   visibleObjectsDesc = "";
   for(unsigned int i = 0; i < trackObjects.size(); ++i)
   {
      WO* affectedWO = this->getWorldContainer()->getWOByID(trackObjects[i]);
      //affectedWO->getModel()->getBoundingBox().getlxlylz();    
      
      // BoundingBox objBox = affectedWO->getModel()->getBoundingBox();
      // Vector minObjBox = objBox.getMin();
      // Vector maxObjBox = objBox.getMax();
      affectedWO->isVisible = true;
      if(affectedWO->isInViewingFrustum(camera->getAftrGeometryFrustum()))
      {
         affectedWO->isVisible = true;
         visibleObjects++;
         visibleObjectsDesc += ("Object " + std::to_string(visibleObjects) + ": " +affectedWO->getLabel() + " " + affectedWO->getPosition().toString() + "\n\t");
      }
      else
      {
         affectedWO->isVisible = false;
         
      }

      // if(checkAll(minObjBox, maxObjBox, cam))
      // {
      //    affectedWO->isInViewingFrustum
      // }
      // else
      // {

      // }

      // WO* wo = WO::New();
      // wo->setModel(MGLFrustum::New(wo,cam->getCameraNearClippingPlaneDistance(),cam->getCameraFarClippingPlaneDistance(),cam->getCameraHorizontalFOVDeg(), cam->getCameraAspectRatioWidthToHeight()));
   
      
   }
}

GLViewCS4350_Assignment7::GLViewCS4350_Assignment7( const std::vector< std::string >& args ) : GLView( args )
{
   //Initialize any member variables that need to be used inside of LoadMap() here.
   //Note: At this point, the Managers are not yet initialized. The Engine initialization
   //occurs immediately after this method returns (see GLViewCS4350_Assignment7::New() for
   //reference). Then the engine invoke's GLView::loadMap() for this module.
   //After loadMap() returns, GLView::onCreate is finally invoked.

   //The order of execution of a module startup:
   //GLView::New() is invoked:
   //    calls GLView::init()
   //       calls GLView::loadMap() (as well as initializing the engine's Managers)
   //    calls GLView::onCreate()

   //GLViewCS4350_Assignment7::onCreate() is invoked after this module's LoadMap() is completed.
}


void GLViewCS4350_Assignment7::onCreate()
{
   //GLViewCS4350_Assignment7::onCreate() is invoked after this module's LoadMap() is completed.
   //At this point, all the managers are initialized. That is, the engine is fully initialized.

   if( this->pe != NULL )
   {
      //optionally, change gravity direction and magnitude here
      //The user could load these values from the module's aftr.conf
      this->pe->setGravityNormalizedVector( Vector( 0,0,-1.0f ) );
      this->pe->setGravityScalar( Aftr::GRAVITY );
   }
   this->setActorChaseType( STANDARDEZNAV ); //Default is STANDARDEZNAV mode
   //this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1
   currentCamera = cam;
   cameraList.push_back(cam);
}


GLViewCS4350_Assignment7::~GLViewCS4350_Assignment7()
{
   //Implicitly calls GLView::~GLView()
}


void GLViewCS4350_Assignment7::updateWorld()
{
   GLView::updateWorld(); //Just call the parent's update world first.
                          //If you want to add additional functionality, do it after
                          //this call.
   this->fustrumUpdate(currentCamera);
}


void GLViewCS4350_Assignment7::onResizeWindow( GLsizei width, GLsizei height )
{
   GLView::onResizeWindow( width, height ); //call parent's resize method.
}


void GLViewCS4350_Assignment7::onMouseDown( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseDown( e );
}


void GLViewCS4350_Assignment7::onMouseUp( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseUp( e );
}


void GLViewCS4350_Assignment7::onMouseMove( const SDL_MouseMotionEvent& e )
{
   GLView::onMouseMove( e );
}


void GLViewCS4350_Assignment7::onKeyDown( const SDL_KeyboardEvent& key )
{
   GLView::onKeyDown( key );
   if( key.keysym.sym == SDLK_0 )
      this->setNumPhysicsStepsPerRender( 1 );

   if( key.keysym.sym == SDLK_1 )
   {

   }
}


void GLViewCS4350_Assignment7::onKeyUp( const SDL_KeyboardEvent& key )
{
   GLView::onKeyUp( key );
}


void Aftr::GLViewCS4350_Assignment7::loadMap()
{
   this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
   this->actorLst = new WorldList();
   this->netLst = new WorldList();

   ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
   ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
   ManagerOpenGLState::enableFrustumCulling = false;
   Axes::isVisible = true;
   this->glRenderer->isUsingShadowMapping( false ); //set to TRUE to enable shadow mapping, must be using GL 3.2+

   this->cam->setPosition( 15,15,10 );

   std::string shinyRedPlasticCube( ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl" );
   std::string wheeledCar( ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl" );
   std::string grass( ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl" );
   std::string human( ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl" );
   
   modelList.push_back(shinyRedPlasticCube);
   modelList.push_back(wheeledCar);
   modelList.push_back(grass);
   modelList.push_back(human);
   //SkyBox Textures readily available
   std::vector< std::string > skyBoxImageNames; //vector to store texture paths
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_water+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_dust+6.jpg" );
   skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_winter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/early_morning+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_afternoon+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy3+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day2+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_deepsun+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_evening+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning2+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_noon+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_warp+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_Hubble_Nebula+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_gray_matter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_easter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_hot_nebula+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_ice_field+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_lemon_lime+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_milk_chocolate+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_solar_bloom+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_thick_rb+6.jpg" );

   {
      //Create a light
      float ga = 0.1f; //Global Ambient Light level for this module
      ManagerLight::setGlobalAmbientLight( aftrColor4f( ga, ga, ga, 1.0f ) );
      WOLight* light = WOLight::New();
      light->isDirectionalLight( true );
      light->setPosition( Vector( 0, 0, 100 ) );
      //Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
      //for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
      light->getModel()->setDisplayMatrix( Mat4::rotateIdentityMat( { 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD ) );
      light->setLabel( "Light" );
      worldLst->push_back( light );
   }

   {
      //Create the SkyBox
      WO* wo = WOSkyBox::New( skyBoxImageNames.at( 0 ), this->getCameraPtrPtr() );
      wo->setPosition( Vector( 0, 0, 0 ) );
      wo->setLabel( "Sky Box" );
      wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
      worldLst->push_back( wo );
   }

   { 
      ////Create the infinite grass plane (the floor)
      WO* wo = WO::New( grass, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
      wo->setPosition( Vector( 0, 0, 0 ) );
      wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
      wo->upon_async_model_loaded( [wo]()
         {
            ModelMeshSkin& grassSkin = wo->getModel()->getModelDataShared()->getModelMeshes().at( 0 )->getSkins().at( 0 );
            grassSkin.getMultiTextureSet().at( 0 ).setTexRepeats( 5.0f );
            grassSkin.setAmbient( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Color of object when it is not in any light
            grassSkin.setDiffuse( aftrColor4f( 1.0f, 1.0f, 1.0f, 1.0f ) ); //Diffuse color components (ie, matte shading color of this object)
            grassSkin.setSpecular( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Specular color component (ie, how "shiney" it is)
            grassSkin.setSpecularCoefficient( 10 ); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
         } );
      wo->setLabel( "Grass" );
      worldLst->push_back( wo );
   }


   { 
      ////Create the infinite grass plane (the floor)
      WO* wo = WO::New( shinyRedPlasticCube, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
      wo->setPosition( Vector( 10, 10, 20 ) );
      wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
      wo->upon_async_model_loaded( [wo]()
         {
            ModelMeshSkin& grassSkin = wo->getModel()->getModelDataShared()->getModelMeshes().at( 0 )->getSkins().at( 0 );
            grassSkin.getMultiTextureSet().at( 0 ).setTexRepeats( 5.0f );
            grassSkin.setAmbient( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Color of object when it is not in any light
            grassSkin.setDiffuse( aftrColor4f( 1.0f, 1.0f, 1.0f, 1.0f ) ); //Diffuse color components (ie, matte shading color of this object)
            grassSkin.setSpecular( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Specular color component (ie, how "shiney" it is)
            grassSkin.setSpecularCoefficient( 10 ); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
         } );
      wo->setLabel( "cube" );
      
      trackObjects.push_back(wo->getID());
      modelOGPose.push_back(wo->getPose());
      worldLst->push_back( wo );
   }


   //{
   //   //Create the infinite grass plane that uses the Open Dynamics Engine (ODE)
   //   WO* wo = WOStatic::New( grass, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
   //   ((WOStatic*)wo)->setODEPrimType( ODE_PRIM_TYPE::PLANE );
   //   wo->setPosition( Vector(0,0,0) );
   //   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //   wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).getMultiTextureSet().at(0)->setTextureRepeats( 5.0f );
   //   wo->setLabel( "Grass" );
   //   worldLst->push_back( wo );
   //}

   //{
   //   //Create the infinite grass plane that uses NVIDIAPhysX(the floor)
   //   WO* wo = WONVStaticPlane::New( grass, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
   //   wo->setPosition( Vector( 0, 0, 0 ) );
   //   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //   wo->getModel()->getModelDataShared()->getModelMeshes().at( 0 )->getSkins().at( 0 ).getMultiTextureSet().at( 0 )->setTextureRepeats( 5.0f );
   //   wo->setLabel( "Grass" );
   //   worldLst->push_back( wo );
   //}

   //{
   //   //Create the infinite grass plane (the floor)
   //   WO* wo = WONVPhysX::New( shinyRedPlasticCube, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
   //   wo->setPosition( Vector( 0, 0, 50.0f ) );
   //   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //   wo->setLabel( "Grass" );
   //   worldLst->push_back( wo );
   //}

   //{
   //   WO* wo = WONVPhysX::New( shinyRedPlasticCube, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
   //   wo->setPosition( Vector( 0, 0.5f, 75.0f ) );
   //   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //   wo->setLabel( "Grass" );
   //   worldLst->push_back( wo );
   //}

   //{
   //   WO* wo = WONVDynSphere::New( ManagerEnvironmentConfiguration::getVariableValue( "sharedmultimediapath" ) + "/models/sphereRp5.wrl", Vector( 1.0f, 1.0f, 1.0f ), mstSMOOTH );
   //   wo->setPosition( 0, 0, 100.0f );
   //   wo->setLabel( "Sphere" );
   //   this->worldLst->push_back( wo );
   //}

   //{
   //   WO* wo = WOHumanCal3DPaladin::New( Vector( .5, 1, 1 ), 100 );
   //   ((WOHumanCal3DPaladin*)wo)->rayIsDrawn = false; //hide the "leg ray"
   //   ((WOHumanCal3DPaladin*)wo)->isVisible = false; //hide the Bounding Shell
   //   wo->setPosition( Vector( 20, 20, 20 ) );
   //   wo->setLabel( "Paladin" );
   //   worldLst->push_back( wo );
   //   actorLst->push_back( wo );
   //   netLst->push_back( wo );
   //   this->setActor( wo );
   //}
   //
   //{
   //   WO* wo = WOHumanCyborg::New( Vector( .5, 1.25, 1 ), 100 );
   //   wo->setPosition( Vector( 20, 10, 20 ) );
   //   wo->isVisible = false; //hide the WOHuman's bounding box
   //   ((WOHuman*)wo)->rayIsDrawn = false; //show the 'leg' ray
   //   wo->setLabel( "Human Cyborg" );
   //   worldLst->push_back( wo );
   //   actorLst->push_back( wo ); //Push the WOHuman as an actor
   //   netLst->push_back( wo );
   //   this->setActor( wo ); //Start module where human is the actor
   //}

   //{
   //   //Create and insert the WOWheeledVehicle
   //   std::vector< std::string > wheels;
   //   std::string wheelStr( "../../../shared/mm/models/WOCar1970sBeaterTire.wrl" );
   //   wheels.push_back( wheelStr );
   //   wheels.push_back( wheelStr );
   //   wheels.push_back( wheelStr );
   //   wheels.push_back( wheelStr );
   //   WO* wo = WOCar1970sBeater::New( "../../../shared/mm/models/WOCar1970sBeater.wrl", wheels );
   //   wo->setPosition( Vector( 5, -15, 20 ) );
   //   wo->setLabel( "Car 1970s Beater" );
   //   ((WOODE*)wo)->mass = 200;
   //   worldLst->push_back( wo );
   //   actorLst->push_back( wo );
   //   this->setActor( wo );
   //   netLst->push_back( wo );
   //}
   
   //Make a Dear Im Gui instance via the WOImGui in the engine... This calls
   //the default Dear ImGui demo that shows all the features... To create your own,
   //inherit from WOImGui and override WOImGui::drawImGui_for_this_frame(...) (among any others you need).
   for(int i = 0; i < trackObjects.size(); ++i)
         allModels += ("Object " + std::to_string(i+1) + ": " + worldLst->getWOByID(trackObjects[i])->getLabel() + " " + worldLst->getWOByID(trackObjects[i])->getPosition().toString() + "\n\t");

   WOImGui* gui = WOImGui::New( nullptr );
   gui->setLabel( "My Gui" );
   gui->subscribe_drawImGuiWidget(
      [this, gui]() //this is a lambda, the capture clause is in [], the input argument list is in (), and the body is in {}
      {

         static std::string updateText = "";
         

         


         //used to access and modify a specified object
         static unsigned int trackId = trackObjects[0];
         static int tracker = 0;
         static std::string angleChangeLabel = "Global";
         
         WO *currentObject = worldLst->getWOByID(trackId);
         static bool globalAngleBool = true;
         //variables used for creating new object in the world
         static char  newLabel[128] = "NewObject";
   
         //Constructor is protected and only invoked by descendants of the GLView class
         //tracks position and angle of currentobect 
         static float xP = currentObject->getPosition()[0];
         static float yP = currentObject->getPosition()[1];
         static float zP = currentObject->getPosition()[2];
         static float xPAngle = 0.0f;
         static float yPAngle = 0.0f;
         static float zPAngle = 0.0f;
         static float addedX = 0.0f;
         static float addedY = 0.0f;
         static float addedZ = 0.0f;
         
         static float camX = 0.0f;
         static float camY = 0.0f;
         static float camZ = 0.0f;

         static std::string modelSelected = "";
         static std::string cameraSelected = "";
         static int cameraPos = 0;
         // static int clientNumber = std::stoi(ManagerEnvironmentConfiguration::getVariableValue("NetServerListenPort"));
         // static int serverNumber = std::stoi(ManagerEnvironmentConfiguration::getVariableValue("NetServerTransmitPort"));
         if(cameraPos == 0)
         {
            cameraSelected = "Camera Selected: Default";
         }
         else
         {
            cameraSelected = "Camera Selected: " + currentCamera->getCameraLookAtPoint().toString();
         }

         
         (ImGui::Begin("Rotate and Add Models"));
            
            // ImGui::Text(("Client: " + ManagerEnvironmentConfiguration::getVariableValue("NetServerListenPort")).c_str());
            // ImGui::Text(("Server: " + ManagerEnvironmentConfiguration::getVariableValue("NetServerTransmitPort")).c_str());
            // ImGui::Separator();
            ImGui::Text(("Current Object: " + currentObject->getLabel()).c_str());
            ImGui::Text(cameraSelected.c_str());  
            
            ImGui::Separator();
            ImGui::Text((updateText).c_str());
            ImGui::Separator();
            
            if(ImGui::CollapsingHeader("Objects in World"))
            {
               ImGui::Text(("Visible Objects: " + std::to_string(visibleObjects)  + '\n' + '\t'  + visibleObjectsDesc).c_str());
               ImGui::Separator();
               ImGui::Text(("All Objects: " + std::to_string(trackObjects.size())  + '\n' + '\t'  + allModels).c_str());


            }
            // if (ImGui::CollapsingHeader("Networking"))
            // {
            //      ImGui::InputInt("New Client Number", &clientNumber);
            //      ImGui::InputInt("New Server Number", &serverNumber);
            //      if(ImGui::Button("Change Network"))
            //       {
                     
            //          ManagerEnvironmentConfiguration::registerVariable("NetServerListenPort", std::to_string(clientNumber));
            //          ManagerEnvironmentConfiguration::registerVariable("NetServerTransmitPort", std::to_string(serverNumber));
            //          delete client;
            //          client = NetMessengerClient::New("127.0.0.1", std::to_string(serverNumber));
            //       }
               
            //    ImGui::Text( (client->port).c_str());   
            //    ImGui::Text( (client->getLocalIpAddressesStrings()).c_str());
               
                 
            // }

            if (ImGui::CollapsingHeader("Rotate Model"))
            { 


               if (ImGui::Button("Print my position"))
               {
                  Vector pos = currentObject->getPosition();
                  std::cout << "X Position: " << pos[0] << std::endl;
                  std::cout << "Y Position: " << pos[1] << std::endl;
                  std::cout << "Z Position: " << pos[2] << std::endl;
                  std::cout << "Object ID: " << currentObject->getID() << std::endl;
                  std::cout << currentObject->getPose() << std::endl;
               }
               // redCube->getPosition()[0];
               
               if(ImGui::SliderFloat("Set X Position", &xP, -100, 100))
               {
                  
                  currentObject->setPosition(xP,yP,zP);
                  
                  // msgF->size = 5;
                  
                  // this->client->sendNetMsgSynchronousTCP(this->msgF);
                  // this->client->sendNetMsgBlockForReplyTCP(this->msgF);
                  // if(client->isTCPSocketOpen())
                  // {
                     // this->client->sendNetMsgSynchronousTCP(this->msgF);
                     
                  // if(client->isTCPSocketOpen())
                  // { 
                  //    //Functions
                  //    NetMsgMoveWO msg;
                     
                  //    msg.pose = currentObject->getPose();
                     
                  //    msg.trackPos = tracker;

                  //    this->client->sendNetMsgSynchronousTCP(msg);
                  //    updateText = "Moving " + currentObject->getClassName() + " in instance " + client->host + ":" +client->port;
                     
                  // }
                  // else
                  // {
                  //    delete client;
                  //    client = NetMessengerClient::New("127.0.0.1", ManagerEnvironmentConfiguration::getVariableValue("NetServerTransmitPort"));
                  //    updateText = "Failed to move " + currentObject->getClassName() + " in instance " + client->host + ":" + client->port;
                  // }
               }
               
               if(ImGui::SliderFloat("Set Y Position", &yP, -100, 100))
               {
                  
                  currentObject->setPosition(xP,yP,zP);
                  // if(client->isTCPSocketOpen())
                  // { 
                  //    //Functions
                  //    NetMsgMoveWO msg;
                     
                  //    msg.pose = currentObject->getPose();
                     
                  //    msg.trackPos = tracker;

                  //    this->client->sendNetMsgSynchronousTCP(msg);
                  //    updateText = "Moving " + currentObject->getClassName() + " in instance " + client->host + ":" +client->port;
                     
                  // }
                  // else
                  // {
                  //    updateText = "Failed to move " + currentObject->getClassName() + " in instance " + client->host + ":" +client->port;
                  // }
                  
               }

               if(ImGui::SliderFloat("Set Z Position", &zP, -100, 100))
               {
                  
                  currentObject->setPosition(xP,yP,zP);
                  // if(client->isTCPSocketOpen())
                  // { 
                  //    //Functions
                  //    NetMsgMoveWO msg;
                     
                  //    msg.pose = currentObject->getPose();
                     
                  //    msg.trackPos = tracker;

                  //    this->client->sendNetMsgSynchronousTCP(msg);
                  //    updateText = "Moving " + currentObject->getClassName() + " in instance " + client->host + ":" +client->port;
                     
                  // }
                  // else
                  // {
                  //    updateText = "Failed to move " + currentObject->getClassName() + " in instance " + client->host + ":" +client->port;
                  // }
               }
               
               if (ImGui::SliderAngle("Rotate X Position", &xPAngle))
               {
                  
                  if(globalAngleBool)
                     currentObject->rotateAboutGlobalX(DEGtoRAD * xPAngle);
                  else
                  {
                     currentObject->rotateAboutRelX(DEGtoRAD * xPAngle);
                  }
                  
                  // if(client->isTCPSocketOpen())
                  // { 
                  //    //Functions
                  //    NetMsgMoveWO msg;
                     
                  //    msg.pose = currentObject->getPose();
                     
                  //    msg.trackPos = tracker;

                  //    this->client->sendNetMsgSynchronousTCP(msg);
                  //    updateText = "Moving " + currentObject->getClassName() + " in instance " + client->host + ":" +client->port;
                     
                  // }
                  // else
                  // {
                     
                  //    updateText = "Failed to move " + currentObject->getClassName() + " in instance " + client->host + ":" +client->port;
                  // }
               }

               if (ImGui::SliderAngle("Rotate Y Position", &yPAngle))
               {
                  if(globalAngleBool)
                     currentObject->rotateAboutGlobalY(DEGtoRAD * yPAngle);
                  else
                  {
                     currentObject->rotateAboutRelY(DEGtoRAD * yPAngle);
                  }
                  // if(client->isTCPSocketOpen())
                  // { 
                  //    //Functions
                  //    NetMsgMoveWO msg;
                     
                  //    msg.pose = currentObject->getPose();
                     
                  //    msg.trackPos = tracker;

                  //    this->client->sendNetMsgSynchronousTCP(msg);
                  //    updateText = "Moving " + currentObject->getClassName() + " in instance " + client->host + ":" +client->port;
                     
                  // }
                  // else
                  // {
                  //    updateText = "Failed to move " + currentObject->getClassName() + " in instance " + client->host + ":" +client->port;
                  // }
               }  

               if (ImGui::SliderAngle("Rotate Z Position", &zPAngle))
               {
                  if(globalAngleBool)
                     currentObject->rotateAboutGlobalZ(DEGtoRAD * zPAngle);
                  else
                     currentObject->rotateAboutRelZ(DEGtoRAD * zPAngle);
                  // if(client->isTCPSocketOpen())
                  // { 
                  //    //Functions
                  //    NetMsgMoveWO msg;
                     
                  //    msg.pose = currentObject->getPose();
                     
                  //    msg.trackPos = tracker;

                  //    this->client->sendNetMsgSynchronousTCP(msg);
                  //    updateText = "Moving " + currentObject->getClassName() + " in instance " + client->host +":" + client->port;
                     
                  // }
                  // else
                  // {
                  //    updateText = "Failed to move " + currentObject->getClassName() + " in instance " + client->host + ":" +client->port;
                  // }   
               }

               if (ImGui::Button(("Current Angle: "+ angleChangeLabel).c_str()))
               {
                  if(globalAngleBool)
                  {  
                     
                     globalAngleBool = false;
                     angleChangeLabel = "Relative";
                     xPAngle = 0.0f;
                     yPAngle = 0.0f;
                     zPAngle = 0.0f;
                     updateText = "Switched to Relative Rotation";
            
                  }
                  else
                  {
                     globalAngleBool = true;
                     angleChangeLabel = "Global";
                     xPAngle = 0.0f;
                     yPAngle = 0.0f;
                     zPAngle = 0.0f;
                     updateText = "Switched to Global Rotation";
                     
                  } 
               }
            
               if(ImGui::Button("Switch Object"))
               {
                  if(trackObjects.size() > 1)
                  {
                  
                     if(tracker >= (int) trackObjects.size()-1)
                        tracker = 0;
                     else 
                        ++tracker;

                     
                     trackId = trackObjects[tracker];
                     currentObject = worldLst->getWOByID(trackId);
                     xP = currentObject->getPosition()[0];
                     yP = currentObject->getPosition()[1];
                     zP = currentObject->getPosition()[2];
                     xPAngle = 0.0f;
                     yPAngle = 0.0f;
                     zPAngle = 0.0f;
                     updateText = std::string("Switched to ") + currentObject->getLabel();
                  }
                  else
                  {
                     updateText = std::string("Cannot switch. ") + currentObject->getLabel() + std::string(" is the only object");
                  }
               }
               if(ImGui::Button("Reset Position"))
               {
                  currentObject->setPose(modelOGPose[tracker]);
                  currentObject->rotateToIdentity();
                  xP = currentObject->getPosition()[0];
                  yP = currentObject->getPosition()[1];;
                  zP = currentObject->getPosition()[2];;

                  updateText = std::string("Resetted position of ") + currentObject->getLabel();
                  //  NetMsgMoveWO msg;
                     
                  //    msg.pose = currentObject->getPose();
                     
                  //    msg.trackPos = tracker;

                  //    this->client->sendNetMsgSynchronousTCP(msg);
               }
            }

            if (ImGui::CollapsingHeader("Add Object to World"))
            {
               ImGui::InputText("Label", newLabel, IM_ARRAYSIZE(newLabel));

               ImGui::DragFloat("X-Position", &addedX, 0.0f, -1000, 1000);
               ImGui::DragFloat("Y-Position", &addedY, 0.0f, -1000, 1000);
               ImGui::DragFloat("Z-Position", &addedZ, 0.0f, -1000, 1000);

               static int modelList_current_idx = 0;
               if(ImGui::BeginListBox("Choose Model"))
               {

                  for (int n = 0; n < (int) modelList.size(); n++)
                  {
                     const bool is_selected = (modelList_current_idx == n);
                     if (ImGui::Selectable(modelList[n].c_str(), is_selected))
                        modelList_current_idx = n;

                     // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                     if (is_selected)
                        ImGui::SetItemDefaultFocus();
                        modelSelected = modelList[modelList_current_idx];
                     }
                  ImGui::EndListBox();
               }

               if (ImGui::Button("Add Object"))
               {
                 


                  WO* newObject = WO::New( modelSelected, Vector( 1 , 1, 1 ), MESH_SHADING_TYPE::mstFLAT );

                  newObject->setPosition( Vector(addedX,addedY,addedZ) );
                  newObject->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
                  newObject->setLabel(newLabel);
                  std::cout << newObject->getID() << std::endl;
                  
                  worldLst->push_back( newObject );
                  trackObjects.push_back(newObject->getID());
                  modelOGPose.push_back(Mat4(addedX,addedY,addedZ));
                  allModels +=  ("Object " + std::to_string(trackObjects.size()) + ": " + newObject->getLabel() + " " + newObject->getPosition().toString() + "\n\t");
                  updateText = std::string("Added ") + newLabel + " to world";

                  // NetMsgCreateWO msg;
                  // std::string modTemp = modelSelected;
                  // modTemp.erase(modTemp.find(ManagerEnvironmentConfiguration::getSMM()), ManagerEnvironmentConfiguration::getSMM().length());
                  // msg.modelSelected = modTemp;
                  // msg.pose = newObject->getPose();
                  // msg.label = newLabel;
                  // this->client->sendNetMsgSynchronousTCP(msg);
                 
               }
            }

            if (ImGui::CollapsingHeader("Add Camera to World"))
            {

               ImGui::DragFloat("X-Position", &camX, 0.0f, -1000, 1000);
               ImGui::DragFloat("Y-Position", &camY, 0.0f, -1000, 1000);
               ImGui::DragFloat("Z-Position", &camZ, 0.0f, -1000, 1000);
   

               if (ImGui::Button("Add Camera "))
               {
                 
                  Camera* camera = new CameraLockedGlobal(this,nullptr,Vector(camX,camY,camZ));
                  camera->setPosition(Vector(camX,camY,camZ));
                  camera->updateFrustumNormals();
                  // camera->setPosition(Vector(camX,camY,camZ));
                  WO* wo = WO::New();
               
                  wo->setModel(MGLFrustum::New(wo, camera->getCameraNearClippingPlaneDistance(),camera->getCameraFarClippingPlaneDistance(), camera->getCameraHorizontalFOVDeg(),camera->getCameraAspectRatioWidthToHeight()));
                  //wo->setModel(MGLFrustum::New(wo,fru.getNearPlaneDistance(),fru.getFarPlaneDistance(), ));
                  
                  wo->getModel()->setPosition(Vector(camX,camY,camZ));
                  wo->isVisible=false;
                  camera->setActorToWatch(wo);
                  wo->setLabel("camera");
                  worldLst->push_back(wo);
                  
                  cameraList.push_back(camera);
                  
                  updateText = std::string("Added new camera to world");

                  // NetMsgCreateWO msg;
                  // std::string modTemp = modelSelected;
                  // modTemp.erase(modTemp.find(ManagerEnvironmentConfiguration::getSMM()), ManagerEnvironmentConfiguration::getSMM().length());
                  // msg.modelSelected = modTemp;
                  // msg.pose = newObject->getPose();
                  // msg.label = newLabel;
                  // this->client->sendNetMsgSynchronousTCP(msg); 
                  
               }

               if(ImGui::Button("Change Camera"))
               {
                  if(cameraList.size() <= 1)
                  {
                     updateText = ("Cannot change cameras. Only one camera in use.");
                  }

                  else{
                     if(cameraPos != 0)
                     {
                        currentCamera->getActorToWatch()->isVisible = false;
                        
                     }

                     ++cameraPos;
                     if(cameraPos >= cameraList.size())
                     {
                        cameraPos = 0;
                     }
                     
                     
                     currentCamera = cameraList[cameraPos];
                     if(cameraPos != 0)
                     {
                        currentCamera->getActorToWatch()->isVisible = true;
                        
                     }
                     updateText = ("Successfully changed camera");
                  }
               }


            }

            if(ImGui::CollapsingHeader("Detailed Model Statistics"))
            {
               ImGui::Separator();
               ImGui::Text("Object Pose");
               ImGui::Separator();
               ImGui::Text((currentObject->getPose()).toString().c_str());
               ImGui::Spacing();

               ImGui::Separator();
               ImGui::Text("Object Box");
               ImGui::Separator();
               ImGui::Text(currentObject->getModel()->getBoundingBox().toString().c_str());
            }

            if(ImGui::CollapsingHeader("Detailed Camera Statistics"))
            {  
               char temp[256]; 
               ImGui::Text("Pose of Camera");
               ImGui::Separator();
               ImGui::Text((currentCamera->getPose()).toString().c_str());
               ImGui::Spacing();

               ImGui::Separator();
               ImGui::Text("Camera Look");
               ImGui::Separator();
               ImGui::Text(currentCamera->getCameraLookAtPoint().toString().c_str());
               ImGui::Spacing();

         


               ImGui::Separator();
               ImGui::Text("Camera Planes\n");
               ImGui::Separator();
               ImGui::BulletText(("Near Plane: " + std::to_string(currentCamera->getCameraNearClippingPlaneDistance())).c_str());      
               ImGui::BulletText(("Far Plane: " + std::to_string(currentCamera->getCameraFarClippingPlaneDistance())).c_str()); 
               ImGui::BulletText(("Horizontal FOV Deg: " + std::to_string(currentCamera->getCameraHorizontalFOVDeg())).c_str());     
               ImGui::BulletText(("Camera Aspect Ratio: " + std::to_string(currentCamera->getCameraAspectRatioWidthToHeight())).c_str());                
               ImGui::Spacing();

               ImGui::Separator();
               ImGui::Text("Camera Box");
               ImGui::Separator();
               ImGui::Text(currentCamera->getModel()->getBoundingBox().toString().c_str());

            }
            xPAngle = 0.0f;
            yPAngle = 0.0f;
            zPAngle = 0.0f;

   
            
            ImGui::End();
         // ImGui::ShowDemoWindow(); //Displays the default ImGui demo from C:/repos/aburn/engine/src/imgui_implot/implot_demo.cpp
         // WOImGui::draw_AftrImGui_Demo( gui ); //Displays a small Aftr Demo from C:/repos/aburn/engine/src/aftr/WOImGui.cpp
         // ImPlot::ShowDemoWindow(); //Displays the ImPlot demo using ImGui from C:/repos/aburn/engine/src/imgui_implot/implot_demo.cpp
      } );
   this->worldLst->push_back( gui );

   createCS4350_Assignment7WayPoints();
}


void GLViewCS4350_Assignment7::createCS4350_Assignment7WayPoints()
{
   // Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
   WayPointParametersBase params(this);
   params.frequency = 5000;
   params.useCamera = true;
   params.visible = true;
   WOWayPointSpherical* wayPt = WOWayPointSpherical::New( params, 3 );
   wayPt->setPosition( Vector( 50, 0, 3 ) );
   worldLst->push_back( wayPt );
}
