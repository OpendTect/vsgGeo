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


#include <vsgGeo/OneSideRender.h>
#include <vsgGeo/ComputeBoundsVisitor.h>


vsgGeo::OneSideRender::OneSideRender()
{}


vsgGeo::OneSideRender::OneSideRender(const OneSideRender& b,
					      const osg::CopyOp& op)
    : osg::Node( b, op )
{
    for ( DrawableList::const_iterator itr=b._drawables.begin();
	  itr!=b._drawables.end(); ++itr)
    {
	osg::Drawable* drawable = op( itr->get() );
	if ( drawable ) addDrawable(drawable);
    }

    _lines = b._lines;
    _isdrawn = b._isdrawn;
}


bool vsgGeo::OneSideRender::addDrawable(osg::Drawable* gset)
{
    return addDrawable(gset, Line3(osg::Vec3(0,0,0), osg::Vec3(0,0,1)));
}


bool vsgGeo::OneSideRender::addDrawable( osg::Drawable* gset,
					     const Line3& line )
{
    _drawables.push_back( gset );
    // Precalculate bounding sphere for (multi-threaded) cull traversal
    gset->getBound();

    _lines.push_back( line );
    _isdrawn.push_back( false );
    return true;
}


bool vsgGeo::OneSideRender::removeDrawable(osg::Drawable* gset)
{
    DrawableList::iterator drawable = std::find(
	_drawables.begin(), _drawables.end(), gset);

    const int idx = drawable - _drawables.begin();
    if ( idx<0 )
	return false;

    _drawables.erase( drawable );
    _lines.erase( _lines.begin()+idx );
    _isdrawn.erase( _isdrawn.begin()+idx );

    return true;
}

void vsgGeo::OneSideRender::traverse(osg::NodeVisitor& nv)
{
    if ( nv.getVisitorType()==osg::NodeVisitor::CULL_VISITOR )
    {
	osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(&nv);

	if ( getStateSet() )
	    cv->pushStateSet( getStateSet() );
	osg::BoundingBox bbox;
	bbox.init();
	const osg::Vec3 eye = cv->getEyePoint();
	for ( unsigned int idx=0; idx<_drawables.size(); idx++ )
	{
	    _isdrawn[idx] = false;
	    const osg::Vec3 viewline = eye-_lines[idx]._pos;
	    if ( viewline*_lines[idx]._dir>=0 )
	    {
#if OSG_MIN_VERSION_REQUIRED(3,3,2)
		const osg::BoundingBox bb = _drawables[idx]->getBoundingBox();
#else
		const osg::BoundingBox bb = _drawables[idx]->getBound();
#endif
		const float depth = cv->getDistanceFromEyePoint(
		    bb.center(), false );

		if ( depth < 0 )
		    continue;

		cv->addDrawableAndDepth(
		    _drawables[idx], cv->getModelViewMatrix(), depth );
		_isdrawn[idx] = true;
		bbox.expandBy( bb );
	    }
	}

	if ( _bbox._min != bbox._min || _bbox._max != bbox._max )
	{
	       dirtyBound();
	       _bbox = bbox;
	}

	if ( getStateSet() )
	    cv->popStateSet();
    }
    else
    {
	vsgGeo::ComputeBoundsVisitor* cbv =
	    dynamic_cast<vsgGeo::ComputeBoundsVisitor*>( &nv );
	if ( cbv )
	    cbv->applyBoundingBox(_bbox);
	else
	    osg::Node::traverse( nv );
    }
}


void vsgGeo::OneSideRender::setLine(unsigned int idx,const Line3& line)
{
    if ( idx<_lines.size() )
	_lines[idx] = line;
}


osg::BoundingSphere vsgGeo::OneSideRender::computeBound() const
{
   if ( _bbox.valid() )
    return _bbox;

   osg::BoundingBox bbox;

   for ( unsigned int idx=0; idx< _lines.size(); idx++ )
       bbox.expandBy(_lines[idx]._pos);

   return bbox;
}


bool vsgGeo::OneSideRender::isDrawn( int dim, bool first ) const
{
    int idx = dim*2;
    if ( !first )
	idx++;
    return _isdrawn[idx];
}


#include <osgDB/ObjectWrapper>
#include <osgDB/Registry>
#include <osgDB/Serializer>

static bool checkDrawables( const vsgGeo::OneSideRender& node )
{
    return node.getNumDrawables()>0;
}


static bool writeDrawables( osgDB::OutputStream& os, const vsgGeo::OneSideRender& node )
{
    unsigned int size = node.getNumDrawables();
    os << size << os.BEGIN_BRACKET << std::endl;
    for ( unsigned int i=0; i<size; ++i )
    {
        os << os.PROPERTY("Line") << node.getLine(i) << std::endl;
        os.writeObject( node.getDrawable(i) );
    }

    os << os.END_BRACKET << std::endl;
    return true;
}


static bool readDrawables( osgDB::InputStream& is, vsgGeo::OneSideRender& node )
{
    unsigned int size = 0; is >> size >> is.BEGIN_BRACKET;
    for ( unsigned int i=0; i<size; ++i )
    {
        vsgGeo::Line3 line;
        is >> is.PROPERTY("Line") >> line;

	osg::ref_ptr<osg::Drawable> drawable =
	    dynamic_cast<osg::Drawable*>( (osg::Object*) is.readObject() );
        if ( drawable )
        {
            node.addDrawable( drawable, line );
        }
    }

    is >> is.END_BRACKET;
    
    return true;
}


REGISTER_OBJECT_WRAPPER( OneSideRender_Wrapper,
                        new vsgGeo::OneSideRender,
                        vsgGeo::OneSideRender,
                        "osg::Object osg::Node vsgGeo::OneSideRender")
{
    ADD_USER_SERIALIZER( Drawables );  // _drawables & _lines
}




