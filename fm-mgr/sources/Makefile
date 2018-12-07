SRCS = fm_main.cpp 
OBJS = fm_main.o 

OBJS = $(SRCS:.cpp=.o)
INCLUDES = -I.
CCFLAGS = -g -O2 -Wall -Werror
EXTRACCFLAGS = -fstack-protector-strong -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security
LDFLAGS = -Wl,-z,noexecstack,-z,relro,-z,now

build: fmManager

.cpp.o:
	$(CXX) $(INCLUDES) $(CCFLAGS)  $(EXTRACCFLAGS) -c $< -o $@

fmManager: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS) -lfmcommon -lrt -lpthread -luuid

clean:
	 @rm -f $(OBJS)

install_non_bb:
	mkdir -p $(DEST_DIR)/usr/lib/systemd/system
	install -m 755 -d $(DEST_DIR)/etc
	install -m 755 -d $(DEST_DIR)/etc/init.d
	install -m 755 -d $(DEST_DIR)/etc/logrotate.d
	install -m 755 -d $(DEST_DIR)/usr/local
	install -m 755 -d $(DEST_DIR)/usr/local/bin
	install -m 755 fminit $(DEST_DIR)/etc/init.d/fminit
	install -m 755 fmManager $(DEST_DIR)/usr/local/bin/fmManager
	install -m 644 fm.logrotate $(DEST_DIR)/etc/logrotate.d/fm.logrotate
	install -m 644 fminit.service $(DEST_DIR)/usr/lib/systemd/system/fminit.service
