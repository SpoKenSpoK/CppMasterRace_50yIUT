#ifndef RENDERTOTEXTURE_H
#define RENDERTOTEXTURE_H


#include <osgViewer/Viewer> 


osg::Camera* createHUDCamera();
osg::Camera* createRTTCamera(osg::Camera::BufferComponent buffer, osg::Texture* tex, bool isAbsolute = false);
osg::Geode* createScreenQuad();



#endif
