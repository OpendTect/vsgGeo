#pragma once

/* vsgGeo - A collection of geoscientific extensions to VulkanSceneGraph.
Copyright 2025 dGB Beheer B.V.

vsgGeo is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>

*/

#include <vsgGeo/Common.h>


namespace vsgGeo
{

class VSGGEO_EXPORT TiledOffScreenRenderer
{
public:
			TiledOffScreenRenderer(osgViewer::View*,
					       osgViewer::CompositeViewer*);
			~TiledOffScreenRenderer();

    void		setOutputSize(int width, int height);
    void		setOutputBackgroundTransparency(unsigned char transparency);
    bool		createOutput();

    const osg::Image*	getOutput() const;
    void		writeOutputFile(const std::string& fileName);
    static bool		isOutputFormatSupported(const std::string& ext);
    void		setForegroundTransparency(unsigned char transparency);
    unsigned char	getForegroundTransparency() const;

protected:
	class OffscreenTileImageCollector : public osg::Referenced
	{
	public:
	    typedef std::pair<unsigned int, unsigned int> TilePosition;
	    typedef std::map< TilePosition, osg::ref_ptr<osg::Image> > TileImages;

	    OffscreenTileImageCollector();
	    void setTileSize(int w, int h) { _tileSize.set(w, h); }
	    void setFinalImageSize(int w, int h) { _finalImageSize.set(w, h); }
	    void setCaptureCamera(osg::Camera* camera) { _camera = camera; }
	    void setFinalImage(osg::Image* image) { _finalImage = image; }
	    bool done() const { return !_isRunning && !_isFinishing; }
	    bool finished() const { return _isFinishing; }
	    void setCameraOrientation(const osg::Matrixd& view, const osg::Matrixd& proj);
	    void frame(const osg::FrameStamp* fs);
	    osg::ref_ptr<osg::Image> getFinalImage(){ return _finalImage; }
	    void setBackgroundTransparency(unsigned char transparency){_transparency = transparency;}
	    void setForegroundTransparency(unsigned char transparency){_foregroundTransparency = transparency;}

	protected:
	    void bindCameraToImage( osg::Camera* camera, int row, int col );
	    void recordImages();
	    void setImageRowBgTransparency(int row,osg::Image* image, const osg::Image* depthImage);

	    bool _isRunning;
	    bool _isFinishing;
	    int _currentRow;
	    int _currentColumn;
	    osg::Vec2 _tileSize;
	    osg::Vec2 _finalImageSize;
	    unsigned int _lastBindingFrame;
	    unsigned char   _transparency;
	    unsigned char   _foregroundTransparency;

	    osg::Matrixd _currentViewMatrix;
	    osg::Matrixd _currentProjectionMatrix;
	    osg::ref_ptr<osg::Camera> _camera;
	    osg::ref_ptr<osg::Image> _finalImage;
	    TileImages _images;
	    TileImages _depthImages;
	};


    const osg::ref_ptr<osgViewer::View>		    _view;
    const osg::ref_ptr<osgViewer::CompositeViewer>  _viewer;
    int						    _width;
    int						    _height;
    int						    _tileWidth;
    int						    _tileHeight;

    osg::ref_ptr<osg::Camera>			    _collectorCamera;
    osg::ref_ptr<osg::Camera>			    _orientationCamera;
    osg::ref_ptr<osg::Switch>			    _cameraSwitch;
    osg::ref_ptr<OffscreenTileImageCollector>	    _imageCollector;
    unsigned char				    _transparency;
    unsigned char				    _foregroundTransparency;

    void			    createImageCollectorCamera();
    void			    setupImageCollector();
    bool			    doRender();


};

} // namespace vsgGeo
