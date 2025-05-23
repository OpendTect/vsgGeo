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

#include <vector>


namespace vsgGeo
{

// Object class with extra functionality to perform custom (non-osg) callbacks

class VSGGEO_EXPORT CallbackObject : public osg::Object
{
public:
		CallbackObject()				{};
		CallbackObject(const CallbackObject&,const osg::CopyOp&);
		META_Object(vsgGeo,CallbackObject);
    virtual	~CallbackObject();

    struct Callback : public virtual osg::Object
    {
		Callback()					{}
		Callback(const Callback&,const osg::CopyOp&)	{}
		META_Object(vsgGeo,Callback);

		// Add here your own virtual callback function
		virtual void requestRedraw() const		{}
		virtual void startWorkInProgress() const	{}
		virtual void stopWorkInProgress() const		{}
    };

    void	addCallback(Callback* cb);
    void	removeCallback(Callback* cb);

		// Add here your own callback trigger function
    void	triggerRedrawRequest();
    void	triggerStartWorkInProgress();
    void	triggerStopWorkInProgress();

protected:
    std::vector<Callback*>	_callbacks;
};


}; // namespace vsgGeo


