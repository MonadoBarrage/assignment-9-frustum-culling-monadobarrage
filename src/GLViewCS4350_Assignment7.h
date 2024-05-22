#pragma once

#include "GLView.h"
#include "MGLFrustum.h"

namespace Aftr
{
   class Camera;

/**
   \class GLViewCS4350_Assignment7
   \author Scott Nykl 
   \brief A child of an abstract GLView. This class is the top-most manager of the module.

   Read \see GLView for important constructor and init information.

   \see GLView

    \{
*/

class GLViewCS4350_Assignment7 : public GLView
{
public:
   static GLViewCS4350_Assignment7* New( const std::vector< std::string >& outArgs );
   virtual ~GLViewCS4350_Assignment7();
   virtual void updateWorld(); ///< Called once per frame
   virtual void loadMap(); ///< Called once at startup to build this module's scene
   virtual void createCS4350_Assignment7WayPoints();
   virtual void onResizeWindow( GLsizei width, GLsizei height );
   virtual void onMouseDown( const SDL_MouseButtonEvent& e );
   virtual void onMouseUp( const SDL_MouseButtonEvent& e );
   virtual void onMouseMove( const SDL_MouseMotionEvent& e );
   virtual void onKeyDown( const SDL_KeyboardEvent& key );
   virtual void onKeyUp( const SDL_KeyboardEvent& key );
   void fustrumUpdate(Camera* camera);

   void fustrumUpdate(MGLFrustum* camera);

   //Helps user to select an object and modify it in the game engine
   std::vector<unsigned int> trackObjects;

   //Keeps all of the models available to use in a vector for accessing
   std::vector<std::string> modelList;

   std::vector<Mat4> modelOGPose;


   std::vector<Camera*> cameraList;

   Aftr::Camera* currentCamera;
protected:
   GLViewCS4350_Assignment7( const std::vector< std::string >& args );
   virtual void onCreate();   
};

/** \} */

} //namespace Aftr
