MCU=16F15214

CC=xc8-cc
MKDIR=mkdir -p
CFLAGS=-mcpu=$(MCU) -O1


OUT_DIR=out
OUT_BIN_DIR=out/bin
SRC_DIR=src

TARGET_NAME=app
TARGET=$(OUT_BIN_DIR)/app



RPI_APP_PATH=/home/pi/fw-flash/pic-prod


SOURCES=src/main_USBC_PIC.c
OBJECTS=$(patsubst %, $(OUT_DIR)/%.o, $(basename $(SOURCES)))


$(TARGET).hex:
	$(MKDIR) $(dir $@)
	@echo -n "[CC] "
	$(CC) $(CFLAGS) $(SOURCES) -o $@

transfer: $(TARGET).hex
	scp $< pi@raspberrypi.local:$(RPI_APP_PATH)

burn: transfer
	ssh pi@raspberrypi.local '~/.local/bin/n14 program $(RPI_APP_PATH)/$(TARGET_NAME).hex'

clean:
	rm -rf out