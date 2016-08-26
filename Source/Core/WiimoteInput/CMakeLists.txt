set(SRCS
  InputSource.cpp
  RealWiimotes/RealDevice.cpp
  SourceMapper.cpp
  WiimoteInput.cpp
  )

set(LIBS 
	common
	inputcommon
	)

add_dolphin_library(wiimoteinput "${SRCS}" "${LIBS}")
