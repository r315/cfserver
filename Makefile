#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := cfserver

include $(IDF_PATH)/make/project.mk

PARTITION_DATA =build/partition_data.bin

MKSPIFFS =mkspiffs/mkspiffs

$(MKSPIFFS):
	$(MAKE) -C mkspiffs

$(PARTITION_DATA):
#	python scripts/spiffsgen.py 0x70000 public build/spiffs.bin
#	mkspiffs/mkspiffs -c public -b 4096 -p 256 -s 0x70000 $@

partition_data-flash: $(PARTITION_DATA) $(MKSPIFFS)
#	python scripts/spiffsgen.py 0x70000 public $<
	$(MKSPIFFS) -c public -b 4096 -p 256 -s 0x70000 $(PARTITION_DATA)
	$(ESPTOOLPY) --port $(ESPPORT) --baud $(ESPBAUD) write_flash -z 0x90000 $(PARTITION_DATA)

test:
	@echo $(ESPTOOLPY_WRITE_FLASH) $(ESPTOOL_ALL_FLASH_ARGS)
