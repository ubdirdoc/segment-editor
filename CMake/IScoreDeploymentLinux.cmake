if(UNIX)

configure_file (
  "${CMAKE_CURRENT_LIST_DIR}/Deployment/Linux/SEGMent.desktop.in"
  "${PROJECT_BINARY_DIR}/SEGMent.desktop"
  )

install(FILES "${PROJECT_BINARY_DIR}/SEGMent.desktop"
        DESTINATION share/applications)

endif()
