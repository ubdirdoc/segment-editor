set(SCORE_COTIRE_DISABLE_UNITY True)
set(SCORE_SPLIT_DEBUG True)
if(SEGMENT_RELEASE)
  include(travis/static-release)
else()
  include(travis/static-debug)
endif()

set(SCORE_PLUGINS_TO_BUILD
score-lib-inspector
score-plugin-inspector
segment
)

