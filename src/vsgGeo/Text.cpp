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

#include "vsgGeo/Text.h"


vsg::ref_ptr<vsg::Text> vsgGeo::createText()
{
    auto font = vsg::read_cast<vsg::Font>( "fonts/arial.vsgt" );
    if ( !font )
    {
	//std::cerr << "Failed to load font.\n";
	return nullptr;
    }

    auto text = vsg::Text::create();
    /*text->font = font;
    text->position.set(0.0f, 0.0f, 0.0f);
    text->text = {"Hello VulkanSceneGraph"};
    text->layout = vsg::Text::LEFT_TO_RIGHT; */

    return text;
}
