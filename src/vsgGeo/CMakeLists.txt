cmake_path( GET CMAKE_CURRENT_SOURCE_DIR FILENAME LIB_NAME )

set( CONFIG_FILE "${CMAKE_BINARY_DIR}/include/${LIB_NAME}/Config.h" )
configure_file(
    "${CMAKE_SOURCE_DIR}/include/${LIB_NAME}/Config.h.in"
    "${CONFIG_FILE}" )

set( LIB_PUBLIC_HEADERS 
    AxesNode.h
    Export.h
    Callback.h
    Common.h
    ComputeBoundsVisitor.h
    Draggers.h
    GLInfo.h
    LayeredTexture.h
    LayerProcess.h
    Line3.h
    MarkerSet.h
    MarkerShape.h
    OneSideRender.h
    PlaneWellLog.h
    PolygonSelection.h
    PolyLine.h
    ScalarBar.h
    TabBoxDragger.h
    TabPlaneDragger.h
    Text.h
    TexturePlane.h
    TexturePanelStrip.h
    ThreadGroup.h
    ThumbWheel.h
    TiledOffScreenRenderer.h
    TrackballManipulator.h
    TubeWellLog.h
    Vec2i.h
    VolumeTechniques.h
    WellLog.h )

set( SOURCES
    AxesNode.cpp
    Callback.cpp
    Draggers.cpp
    GLInfo.cpp
    Palette.cpp
    PlaneWellLog
    ShaderUtility.cpp
    PolygonSelection.cpp
    PolyLine.cpp
    LayeredTexture.cpp
    LayerProcess.cpp
    Line3.cpp
    MarkerSet.cpp
    MarkerShape.cpp
    OneSideRender.cpp
    PlaneWellLog.cpp
    ScalarBar.cpp
    TabPlaneDragger.cpp
    TabBoxDragger.cpp
    Text.cpp
    TexturePlane.cpp
    TexturePanelStrip.cpp
    ThumbWheel.cpp
    TiledOffScreenRenderer.cpp
    TrackballManipulator.cpp 
    TubeWellLog.cpp
    VolumeTechniques.cpp
    WellLog.cpp)

foreach( HEADER ${LIB_PUBLIC_HEADERS} )
    list( APPEND PUBLIC_HEADERS "${CMAKE_SOURCE_DIR}/include/${LIB_NAME}/${HEADER}" )
endforeach()

add_library( ${LIB_NAME} SHARED
    ${SOURCES}
    ${PUBLIC_HEADERS}
    "${CONFIG_FILE}"
)

set_target_properties( ${LIB_NAME} PROPERTIES
    PUBLIC_HEADER "${LIB_PUBLIC_HEADERS} ${CONFIG_FILE}"
)

target_link_libraries( ${LIB_NAME} vsg::vsg )
target_include_directories( ${LIB_NAME} PUBLIC
		"${CMAKE_SOURCE_DIR}/include"
		"${CMAKE_BINARY_DIR}/include" )

install(TARGETS ${LIB_NAME} EXPORT vsgGeoTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${LIB_NAME}"
)
