set(SRCS
  WiimoteInput.cpp
  SourceMapper.cpp
  )

set(LIBS 
	common
	inputcommon
	)

add_dolphin_library(wiimoteinput "${SRCS}" "${LIBS}")
