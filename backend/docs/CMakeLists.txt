find_package(Doxygen)

if(NOT DOXYGEN_FOUND)
  message(STATUS "Doxygen not found, not creating docs target")
  return()
endif()

include(FetchContent)
FetchContent_Declare(
  doxygen-awesome-css
  URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(doxygen-awesome-css)
FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

set(DOXYGEN_HTML_EXTRA_STYLESHEET
  "${AWESOME_CSS_DIR}/doxygen-awesome.css"
  "${AWESOME_CSS_DIR}/doxygen-awesome-sidebar-only.css"
)
set(DOXYGEN_GENERATE_TREEVIEW YES)
set(DOXYGEN_DISABLE_INDEX NO)
set(DOXYGEN_FULL_SIDEBAR NO)

set(DOXYGEN_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/docs)
set(DOXYGEN_RECURSIVE YES)
set(DOXYGEN_QUIET YES)

doxygen_add_docs(${PROJECT_NAME}_docs
  src/
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMENT "Generate HTML documentation"
)
