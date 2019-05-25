if(NOT WIN32)
  return()
endif()

set(SCORE_BIN_INSTALL_DIR ".")

 # Compiler Runtime DLLs
if (MSVC)
   # Visual Studio
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP true)
    include(InstallRequiredSystemLibraries)
    install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
            DESTINATION ${SCORE_BIN_INSTALL_DIR})
else()
   # MinGW
    get_filename_component(MINGW_DLL_DIR ${CMAKE_CXX_COMPILER} PATH)
    install(FILES
            "${MINGW_DLL_DIR}/libgcc_s_dw2-1.dll"
            "${MINGW_DLL_DIR}/libstdc++-6.dll"
            "${MINGW_DLL_DIR}/libwinpthread-1.dll"
            DESTINATION ${SCORE_BIN_INSTALL_DIR})
endif()


# Qt Libraries
set(DEBUG_CHAR "$<$<CONFIG:Debug>:d>")

get_target_property(QtCore_LOCATION Qt5::Core LOCATION)
get_filename_component(QT_DLL_DIR ${QtCore_LOCATION} PATH)
file(GLOB ICU_DLLS "${QT_DLL_DIR}/icu*.dll")
# TODO instead register them somewhere like the plug-ins.

if(NOT OSSIA_STATIC)
install(FILES "$<TARGET_FILE:ossia>"
        DESTINATION ${SCORE_BIN_INSTALL_DIR})
endif()

install(FILES
  ${ICU_DLLS}
  "${QT_DLL_DIR}/Qt5Core${DEBUG_CHAR}.dll"
  "${QT_DLL_DIR}/Qt5Gui${DEBUG_CHAR}.dll"
  "${QT_DLL_DIR}/Qt5Widgets${DEBUG_CHAR}.dll"
  "${QT_DLL_DIR}/Qt5Network${DEBUG_CHAR}.dll"
  "${QT_DLL_DIR}/Qt5Sql${DEBUG_CHAR}.dll"
  "${QT_DLL_DIR}/Qt5Svg${DEBUG_CHAR}.dll"
  "${QT_DLL_DIR}/Qt5Help${DEBUG_CHAR}.dll"
  "${QT_DLL_DIR}/Qt5Multimedia${DEBUG_CHAR}.dll"
  DESTINATION "${SCORE_BIN_INSTALL_DIR}")
# install(
#   DIRECTORY
#     "${CMAKE_BINARY_DIR}/Ressources"
#   DESTINATION
#     ${SCORE_BIN_INSTALL_DIR})

# Qt conf file
install(
  FILES
    "${SCORE_ROOT_SOURCE_DIR}/CMake/Deployment/Windows/qt.conf"
    "${SCORE_ROOT_SOURCE_DIR}/base/lib/resources/score.ico"
  DESTINATION
    ${SCORE_BIN_INSTALL_DIR})

# Qt plug-ins
set(QT_PLUGINS_DIR "${QT_DLL_DIR}/../plugins")
set(QT_QML_PLUGINS_DIR "${QT_DLL_DIR}/../qml")
set(plugin_dest_dir "${SCORE_BIN_INSTALL_DIR}/plugins")

install(FILES "${QT_PLUGINS_DIR}/audio/qtaudio_wasapi${DEBUG_CHAR}.dll" DESTINATION "${plugin_dest_dir}/audio")
install(FILES "${QT_PLUGINS_DIR}/audio/qtaudio_windows${DEBUG_CHAR}.dll" DESTINATION "${plugin_dest_dir}/audio")

install(FILES "${QT_PLUGINS_DIR}/platforms/qwindows${DEBUG_CHAR}.dll" DESTINATION "${plugin_dest_dir}/platforms")

install(FILES "${QT_PLUGINS_DIR}/imageformats/qsvg${DEBUG_CHAR}.dll" DESTINATION "${plugin_dest_dir}/imageformats")
install(FILES "${QT_PLUGINS_DIR}/imageformats/qgif${DEBUG_CHAR}.dll" DESTINATION "${plugin_dest_dir}/imageformats")
install(FILES "${QT_PLUGINS_DIR}/imageformats/qicns${DEBUG_CHAR}.dll" DESTINATION "${plugin_dest_dir}/imageformats")
install(FILES "${QT_PLUGINS_DIR}/imageformats/qico${DEBUG_CHAR}.dll" DESTINATION "${plugin_dest_dir}/imageformats")
install(FILES "${QT_PLUGINS_DIR}/imageformats/qjpeg${DEBUG_CHAR}.dll" DESTINATION "${plugin_dest_dir}/imageformats")
install(FILES "${QT_PLUGINS_DIR}/imageformats/qtga${DEBUG_CHAR}.dll" DESTINATION "${plugin_dest_dir}/imageformats")
install(FILES "${QT_PLUGINS_DIR}/imageformats/qtiff${DEBUG_CHAR}.dll" DESTINATION "${plugin_dest_dir}/imageformats")
install(FILES "${QT_PLUGINS_DIR}/imageformats/qwbmp${DEBUG_CHAR}.dll" DESTINATION "${plugin_dest_dir}/imageformats")
install(FILES "${QT_PLUGINS_DIR}/imageformats/qwebp${DEBUG_CHAR}.dll" DESTINATION "${plugin_dest_dir}/imageformats")

install(FILES "${QT_PLUGINS_DIR}/mediaservice/dsengine${DEBUG_CHAR}.dll" DESTINATION "${plugin_dest_dir}/mediaservice")
install(FILES "${QT_PLUGINS_DIR}/mediaservice/wmfengine${DEBUG_CHAR}.dll" DESTINATION "${plugin_dest_dir}/mediaservice")

install(FILES "${QT_PLUGINS_DIR}/iconengines/qsvgicon${DEBUG_CHAR}.dll" DESTINATION "${plugin_dest_dir}/iconengines")

install(FILES "${QT_PLUGINS_DIR}/styles/qwindowsvistastyle${DEBUG_CHAR}.dll" DESTINATION "${plugin_dest_dir}/styles")

install(FILES "${QT_PLUGINS_DIR}/sqldrivers/qsqlite${DEBUG_CHAR}.dll" DESTINATION "${plugin_dest_dir}/sqldrivers")

# NSIS metadata
set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_EXECUTABLES "segment-editor.exe;score")

set(CPACK_COMPONENTS_ALL score)

set(CPACK_MONOLITHIC_INSTALL TRUE)
set(CPACK_NSIS_PACKAGE_NAME "SEGMent Editor")
set(CPACK_PACKAGE_ICON "${SCORE_ROOT_SOURCE_DIR}\\\\base\\\\lib\\\\resources\\\\score.ico")
set(CPACK_NSIS_MUI_ICON "${CPACK_PACKAGE_ICON}")
set(CPACK_NSIS_MUI_UNIICON "${CPACK_PACKAGE_ICON}")

set(CPACK_NSIS_HELP_LINK "https:\\\\\\\\example.com")
set(CPACK_NSIS_URL_INFO_ABOUT "https:\\\\\\\\example.com")
set(CPACK_NSIS_CONTACT "https:\\\\\\\\example.com")

set(CPACK_NSIS_COMPRESSOR "/SOLID lzma")

set(CPACK_NSIS_MENU_LINKS
    "segment-editor.exe" "SEGMent Editor"
    )


set(CPACK_NSIS_DEFINES "!include ${CMAKE_CURRENT_LIST_DIR}\\\\Deployment\\\\Windows\\\\FileAssociation.nsh")
set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
\\\${registerExtension} '\\\$INSTDIR\\\\segment-editor.exe' '.segment' 'SEGMent Game'

SetOutPath '\\\$INSTDIR'
CreateShortcut '\\\$DESKTOP\\\\segment.lnk' '\\\$INSTDIR\\\\segment-editor.exe' '' '\\\$INSTDIR\\\\score.ico'
SetRegView 64
WriteRegStr HKEY_LOCAL_MACHINE 'SOFTWARE\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\App Paths\\\\segment-editor.exe' '' '$INSTDIR\\\\segment-editor.exe'

AccessControl::GrantOnFile '\\\$INSTDIR' '(S-1-5-32-545)' 'FullAccess'
")
set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
Delete '$DESKTOP\\\\score.lnk'
DeleteRegKey HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\App Paths\\\\segment-editor.exe'
\\\${unregisterExtension} '.segment' 'SEGMent Game'
")

