#pragma once

/* vsgGeo - A collection of geoscientific extensions to OpenSceneGraph.
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
#include <vsgGeo/Common>
#include <osg/AutoTransform>

namespace vsgGeo
{

/* Autotransform with the option to restore proportions. */
class OSGGEO_EXPORT AutoTransform : public osg::AutoTransform
{
    public :
	AutoTransform();
	AutoTransform(const AutoTransform& pat,const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	virtual void accept(osg::NodeVisitor& nv);

	void setRestoreProportions(bool restoreProportions) { _restoreProportions = restoreProportions; _matrixDirty=true; dirtyBound(); }
	bool getRestoreProportions() const { return _restoreProportions; }


	virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const;
	virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const;


    protected :
	bool				_restoreProportions;
	bool				_scaledBack;

	mutable bool			_invMatrixDirty;
	mutable osg::Matrixd		_cachedInvMatrix;

	osg::Vec3d			_modelViewScale;
	osg::Matrix			_lastModelViewMatrix;
};

}
