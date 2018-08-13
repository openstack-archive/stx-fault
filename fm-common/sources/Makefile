SRCS = fmAPI.cpp fmFile.cpp fmLog.cpp fmMsgServer.cpp fmMutex.cpp fmSocket.cpp fmThread.cpp fmTime.cpp \
       fmAlarmUtils.cpp fmDb.cpp fmDbUtils.cpp fmDbAlarm.cpp fmSnmpUtils.cpp \
       fmDbEventLog.cpp fmEventSuppression.cpp fmConfig.cpp
CLI_SRCS = fm_cli.cpp
OBJS = $(SRCS:.cpp=.o)
CLI_OBJS = fm_cli.o
LDLIBS = -lstdc++ -lrt -luuid -lpq -lpthread -lpython2.7
INCLUDES = -I./
CCFLAGS = -g -O2 -Wall -Werror -fPIC

LIBFMCOMMON_SO := libfmcommon.so
build: lib fmClientCli  

.cpp.o:
	$(CXX) $(CCFLAGS) $(INCLUDES) $(EXTRACCFLAGS) -c $< -o $@

lib: $(OBJS)
	$(CXX) -Wl,-soname,$(LIBFMCOMMON_SO).$(MAJOR) -o $(LIBFMCOMMON_SO).$(MAJOR).$(MINOR) -shared $(OBJS) $(EXTRAARFLAGS) ${LDLIBS}
	ln -sf $(LIBFMCOMMON_SO).$(MAJOR).$(MINOR) $(LIBFMCOMMON_SO).$(MAJOR)
	ln -sf $(LIBFMCOMMON_SO).$(MAJOR).$(MINOR) $(LIBFMCOMMON_SO)

fmClientCli: $(CLI_OBJS) lib
	$(CXX) -o $@ $(CLI_OBJS) -L./ -lfmcommon 
	
clean:
	@rm -f $(OBJ) *.o *.so fmClientCli  

install_non_bb:
	install -m 755 -d $(DEST_DIR)$(BIN_DIR)
	install -m 755 -d $(DEST_DIR)$(LIB_DIR)
	install -m 755 -d $(DEST_DIR)$(INC_DIR)
	install -m 755 fmClientCli $(DEST_DIR)$(BIN_DIR)
	install -m 644 fmDbAPI.h $(DEST_DIR)$(INC_DIR)
	install -m 644 fmAPI.h $(DEST_DIR)$(INC_DIR)
	install -m 644 fmThread.h $(DEST_DIR)$(INC_DIR)
	install -m 644 fmAlarm.h $(DEST_DIR)$(INC_DIR)
	install -m 755 $(LIBFMCOMMON_SO).$(MAJOR).$(MINOR) $(DEST_DIR)$(LIB_DIR)
	ln -s $(LIBFMCOMMON_SO).$(MAJOR).$(MINOR) $(DEST_DIR)$(LIB_DIR)/$(LIBFMCOMMON_SO).$(MAJOR)
	ln -s $(LIBFMCOMMON_SO).$(MAJOR).$(MINOR) $(DEST_DIR)$(LIB_DIR)/$(LIBFMCOMMON_SO)

