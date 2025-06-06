/* -*-c++-*- VulkanSceneGraph - Copyright (C) 1998-2006 Robert Osfield
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the VulkanSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * VulkanSceneGraph Public License for more details.
*/

#ifndef OSGSIM_ScalarBar
#define OSGSIM_ScalarBar 1

#include <vsgGeo/Common.h>
#include <string>


namespace vsgGeo
{
/**
A ScalarBar is an osg::Geode to render a colored bar representing a range
of scalars. The scalar/color ranges are specified by an instance of
ScalarsToColors. There are a number of configurable properties on the
ScalarBar, such as the orientation, the number of labels to be displayed
across the range, the number of distinct colors to use when rendering the
bar, text details etc.

In summary, the main configurables on the ScalarBar are:

 -# The range of scalars represented by the bar, and the colors
    corresponding to this range - these are specified by the
    ScalarsToColors object.
 -# The number of colors used when rendering the bar geometry -
    this may be thought of as the bar 'density'.
 -# The number of text labels to be used when displaying the bar.

The other configurables should be self-explanatory.
*/
class VSGGEO_EXPORT ScalarBar: public osg::Geode
{

public:

    /** ScalarBar orientation specification. */
    enum Orientation{
        HORIZONTAL, ///< a horizontally ascending scalar bar (x-axis)
        VERTICAL    ///< a vertically ascending scalar bar (y-axis)
    };

    /**
    Users may provide their own ScalarPrinter by deriving from this base class and
    overriding the printScalar() method. Users may map the scalar float passed in
    to any string they wish.
    */
    struct VSGGEO_EXPORT ScalarPrinter: public osg::Referenced
    {
        virtual std::string printScalar(float scalar);
    };

    /**
    TextProperties allows users to specify a number of properties for the
    text used to display the labels & title on the ScalarBar. Specifiying a character
    size of 0 will cause the ScalarBar to estimate an appropriate size. Note that
    the attributes are public, and may be set directly.
    */
    struct TextProperties
    {
        std::string		    _fontFile;
        std::pair<int,int>	    _fontResolution;
        float			    _characterSize;
        osg::Vec4		    _color;
	osg::ref_ptr<osgText::Font> _font;

        TextProperties():
            _fontFile("fonts/arial.ttf"),
            _fontResolution(40,40),
            _characterSize(0.0f),
            _color(1.0f,1.0f,1.0f,1.0f),
	    _font(0)
        {
        }
    };

    /** Default constructor. */
    ScalarBar(): osg::Geode(),
        _numColors(256),
        _numLabels(11),
        _stc(new osgSim::ColorRange(0.0f,1.0f)),
        _title("Scalar Bar"),
        _position(0.0f,0.0f,0.0f),
        _width(1.0f),
        _aspectRatio(0.03),
        _orientation(HORIZONTAL),
        _sp(new ScalarPrinter)
    {
        createDrawables();
    }

    /**
    Construct a ScalarBar with the supplied parameters.
    @param numColors    Specify the number of colors in the scalar bar. Color
                        interpolation occurs where necessary.
    @param numLabels    Specify the number of labels in the scalar bar.
    @param stc          The ScalarsToColors defining the range of scalars
                        and the colors they map to.
    @param title        The title to be used when displaying the ScalarBar.
                        Specify "" for no title.
    @param orientation  The orientation of the ScalarBar. @see Orientation.
    @param aspectRatio  The aspect ration (y/x) for the displayed bar. Bear in mind you
                        may want to change this if you change the orientation.
    @param sp           A ScalarPrinter object for the ScalarBar. For every displayed
                        ScalarBar label, the scalar value will be passed to the
                        ScalarPrinter object to turn it into a string. Users may
                        override the default ScalarPrinter object to map scalars to
                        whatever strings they wish. @see ScalarPrinter
    */
    ScalarBar(int numColors, int numLabels, osgSim::ScalarsToColors* stc,
                const std::string& title,
                Orientation orientation = HORIZONTAL,
                float aspectRatio=0.25,
                ScalarPrinter* sp=new ScalarPrinter):
        osg::Geode(),
        _numColors(numColors),
        _numLabels(numLabels),
        _stc(stc),
        _title(title),
        _position(0.0f,0.0f,0.0f),
        _width(1.0f),
        _aspectRatio(aspectRatio),
        _orientation(orientation),
        _sp(sp)
    {
        createDrawables();
    }

    /** Copy constructor */
    ScalarBar(const ScalarBar& rhs, const osg::CopyOp& co): osg::Geode(rhs,co),
        _numColors(rhs._numColors),
        _numLabels(rhs._numLabels),
        _stc(rhs._stc),                     // Consider clone for deep copy?
        _title(rhs._title),
        _position(rhs._position),
        _width(rhs._width),
        _aspectRatio(rhs._aspectRatio),
        _orientation(rhs._orientation),
        _sp(rhs._sp),                        // Consider clone for deep copy?
        _textProperties(rhs._textProperties)
    {
    }


    META_Node(osgSim, ScalarBar);

    /** Set the number of distinct colours on the ScalarBar. */
    void setNumColors(int numColors);

    /** Get the number of distinct colours on the ScalarBar. */
    int getNumColors() const;

    /** Set the number of labels to display along the ScalarBar. There
    will be one label at each end point, and evenly distributed labels
    in between. */
    void setNumLabels(int numLabels);

    /** Get the number of labels displayed along the ScalarBar. */
    int getNumLabels() const;

    /** Set the ScalarsToColors mapping object for the ScalarBar. */
    void setScalarsToColors(osgSim::ScalarsToColors* stc);

    /** Get the ScalarsToColors mapping object from the ScalarBar. */
    const osgSim::ScalarsToColors* getScalarsToColors() const;

    /** Set the title for the ScalarBar, set ""  for no title. */
    void setTitle(const std::string& title);

    /** Get the title for the ScalarBar. */
    const std::string& getTitle() const;


    /** Set the position of scalar bar's lower left corner.*/
    void setPosition(const osg::Vec3& pos);

    /** Get the position of scalar bar.*/
    const osg::Vec3& getPosition() const { return _position; }

    /** Set the width of the scalar bar.*/
    void setWidth(float width);

    /** Get the width of the scalar bar.*/
    float getWidth() const { return _width; }

    /** Set the aspect ration (y/x) for the displayed bar. Bear in mind you
    may want to change this if you change the orientation. */
    void setAspectRatio(float aspectRatio);

    /** Get the aspect ration (y/x) for the displayed bar. */
    float getAspectRatio() const;


    /** Set the orientation of the ScalarBar. @see Orientation */
    void setOrientation(ScalarBar::Orientation orientation);

    /** Get the orientation of the ScalarBar.  @see Orientation */
    ScalarBar::Orientation getOrientation() const;


    /** Set a ScalarPrinter object for the ScalarBar. For every displayed
    ScalarBar label, the scalar value will be passed to the ScalarPrinter
    object to turn it into a string. Users may override the default ScalarPrinter
    object to map scalars to whatever strings they wish. @see ScalarPrinter */
    void setScalarPrinter(ScalarPrinter* sp);

    /** Get the ScalarPrinter object  */
    const ScalarPrinter* getScalarPrinter() const;

    /** Set the TextProperties for the labels & title. @see TextProperties */
    void setTextProperties(const TextProperties& tp);

    /** Get the TextProperties for the labels & title. @see TextProperties */
    const TextProperties& getTextProperties() const;

    /** force update the drawables used to render the scalar bar.*/
    void update() { createDrawables(); }

protected:
    virtual ~ScalarBar();

    int _numColors;
    int _numLabels;
    osg::ref_ptr<osgSim::ScalarsToColors> _stc;
    std::string _title;
    osg::Vec3 _position;
    float _width;
    float _aspectRatio;
    Orientation _orientation;
    osg::ref_ptr<ScalarPrinter> _sp;
    TextProperties  _textProperties;

    void createDrawables();

};

}

#endif
