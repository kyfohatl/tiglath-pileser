if (WIN32)
  set(ARMA_TOOLS_CMD "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Arma 3 Tools\\AddonBuilder\\AddonBuilder.exe")
  execute_process(COMMAND ${ARMA_TOOLS_CMD} ${CMAKE_SOURCE_DIR}/addons/main/ ${CMAKE_INSTALL_PREFIX}/../addons)
endif()