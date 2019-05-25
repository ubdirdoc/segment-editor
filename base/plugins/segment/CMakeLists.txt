cmake_minimum_required(VERSION 3.1)
project(iscore_addon_SEGMent LANGUAGES CXX)

# General initialization
score_common_setup()
set(CMAKE_CXX_STANDARD 17)
find_package(Qt5 5.9 REQUIRED COMPONENTS Multimedia OpenGL LinguistTools Help)

# Source files
set(HDRS
    SEGMent/Panel/Library.hpp
    SEGMent/Panel/Minimap.hpp

    SEGMent/Commands/CommandFactory.hpp
    SEGMent/Commands/Properties.hpp
    SEGMent/Commands/Creation.hpp
    SEGMent/Commands/Deletion.hpp

    SEGMent/Model/Layer/ProcessPresenter.hpp
    SEGMent/Model/Layer/ProcessView.hpp

    SEGMent/Model/BackClickArea.hpp
    SEGMent/Model/ClickArea.hpp
    SEGMent/Model/GifObject.hpp
    SEGMent/Model/Model.hpp
    SEGMent/Model/ProcessMetadata.hpp
    SEGMent/Model/ProcessModel.hpp
    SEGMent/Model/Scene.hpp
    SEGMent/Model/SimpleObject.hpp
    SEGMent/Model/TextArea.hpp
    SEGMent/Model/Transition.hpp
    SEGMent/Model/Traits.hpp

    SEGMent/Model/Sound.hpp
    SEGMent/Model/Riddle.hpp
    SEGMent/Model/Image.hpp

    SEGMent/Inspector/InspectorSetup.hpp
    SEGMent/Inspector/Inspectors.hpp

    SEGMent/Items/SizeGripItem.hpp
    SEGMent/Items/Window.hpp
    SEGMent/Items/SceneWindow.hpp
    SEGMent/Items/ObjectWindow.hpp
    SEGMent/Items/ClickWindow.hpp
    SEGMent/Items/RectItem.hpp
    SEGMent/Items/QLineEditAutoResizeAutoCenter.hpp
    SEGMent/Items/GlobalVariables.hpp
    SEGMent/Items/Arrow.hpp
    SEGMent/Items/Anchor.hpp
    SEGMent/Items/AnchorSetter.hpp

    SEGMent/ImageList.hpp
    SEGMent/Document.hpp
    SEGMent/FilePath.hpp
    SEGMent/ApplicationPlugin.hpp
    iscore_addon_SEGMent.hpp
)
set(SRCS
    SEGMent/Panel/Library.cpp
    SEGMent/Panel/Minimap.cpp

    SEGMent/Model/Layer/ProcessPresenter.cpp
    SEGMent/Model/Layer/ProcessView.cpp
    SEGMent/Model/ProcessModel.cpp

    SEGMent/Model/BackClickArea.cpp
    SEGMent/Model/ClickArea.cpp
    SEGMent/Model/GifObject.cpp
    SEGMent/Model/Scene.cpp
    SEGMent/Model/SimpleObject.cpp
    SEGMent/Model/TextArea.cpp
    SEGMent/Model/Transition.cpp

    SEGMent/Model/Riddle.cpp
    SEGMent/Model/Sound.cpp
    SEGMent/Model/Image.cpp

    SEGMent/Items/SizeGripItem.cpp
    SEGMent/Items/Window.cpp
    SEGMent/Items/SceneWindow.cpp
    SEGMent/Items/ObjectWindow.cpp
    SEGMent/Items/ClickWindow.cpp
    SEGMent/Items/RectItem.cpp
    SEGMent/Items/QLineEditAutoResizeAutoCenter.cpp
    SEGMent/Items/Arrow.cpp
    SEGMent/Items/Anchor.cpp
    SEGMent/Items/AnchorSetter.cpp

    SEGMent/Document.cpp
    SEGMent/ImageList.cpp

    iscore_addon_SEGMent.cpp
)
function(add_translation _qm_files)
    foreach(_basename ${ARGN})
    set(qm "${CMAKE_CURRENT_SOURCE_DIR}/${_basename}.qm")
    add_custom_command(
        OUTPUT "${qm}"
        COMMAND "${Qt5_LRELEASE_EXECUTABLE}"
        ARGS -markuntranslated "Not translated!" -nounfinished -removeidentical -compress "${CMAKE_CURRENT_SOURCE_DIR}/${_basename}.ts" -qm "${qm}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${_basename}.ts" VERBATIM
    )
    list(APPEND ${_qm_files} "${qm}")
    endforeach()
    set(${_qm_files} ${${_qm_files}} PARENT_SCOPE)
endfunction()

add_translation(QM_SRCS
   SEGMent/resources/translations/fr
  )

qt5_add_resources(QRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/SEGMent/resources/segment.qrc"
  )

# Creation of the library
add_library(${PROJECT_NAME} ${SRCS} ${HDRS} ${QRCS} ${QM_SRCS})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

if("${Qt5_VERSION}" VERSION_LESS 5.12)
    set(QT_HELPGEN Qt5::qcollectiongenerator)
else()
    set(QT_HELPGEN Qt5::qhelpgenerator)
endif()

add_custom_command(
    OUTPUT
        build/qthelp/SEGMentEditor.qch
    DEPENDS
        Documentation/docs/segment/index.rst
        Documentation/docs/segment/ui/inspector.rst
        Documentation/docs/segment/ui/library.rst
        Documentation/docs/segment/ui/scene.rst
        Documentation/docs/segment/ui/riddles.rst
        Documentation/docs/segment/save/saveformat.rst
        Documentation/docs/segment/tutorial/tutorial.rst
        Documentation/docs/conf.py

    COMMAND ${CMAKE_COMMAND} -E env SCORE_VERSION="${SCORE_VERSION_MAJOR}.${SCORE_VERSION_MINOR}" sphinx-build -M qthelp "${CMAKE_CURRENT_SOURCE_DIR}/Documentation/docs" "${CMAKE_CURRENT_BINARY_DIR}/build"
    COMMAND ${QT_HELPGEN} "${CMAKE_CURRENT_BINARY_DIR}/build/qthelp/SEGMentEditor.qhcp"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/build/qthelp/SEGMentEditor.qhc" "${CMAKE_BINARY_DIR}/Documentation/SEGMentEditor.qhc"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/build/qthelp/SEGMentEditor.qch" "${CMAKE_BINARY_DIR}/Documentation/SEGMentEditor.qch"
    COMMENT
        "Documentation"
    VERBATIM
    )

add_custom_target(
      docs ALL
      DEPENDS build/qthelp/SEGMentEditor.qch
      )
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/build/qthelp/SEGMentEditor.qhc" DESTINATION Documentation)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/build/qthelp/SEGMentEditor.qch" DESTINATION Documentation)

# Code generation
score_generate_command_list_file(${PROJECT_NAME} "${HDRS}")

# Link
target_link_libraries(${PROJECT_NAME} PUBLIC score_lib_inspector score_lib_base Qt5::Multimedia Qt5::OpenGL Qt5::Help)

# Target-specific options
setup_score_plugin(${PROJECT_NAME})
