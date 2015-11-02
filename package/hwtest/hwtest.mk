################################################################################
#
# hwtest
#
################################################################################

HWTEST_VERSION = master
HWTEST_REPO_NAME=CHIP-hwtest
HWTEST_SITE = https://github.com/NextThingCo/$(HWTEST_REPO_NAME)
HWTEST_SITE_METHOD = git
HWTEST_DEPENDENCIES = lshw stress i2c-tools\

define HWTEST_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(HWTEST_DIR)/hwtest.sh $(TARGET_DIR)/usr/bin/hwtest
	$(INSTALL) -D -m 0755 $(HWTEST_DIR)/battery.sh $(TARGET_DIR)/usr/bin/battery.sh
	$(INSTALL) -D -m 0755 $(HWTEST_DIR)/power.sh $(TARGET_DIR)/usr/bin/power.sh

	$(INSTALL) -D -m 0644 $(HWTEST_DIR)/axp_ref.txt $(TARGET_DIR)/usr/lib/hwtest/axp_ref.txt
	$(INSTALL) -D -m 0644 $(HWTEST_DIR)/dd_ref0.txt $(TARGET_DIR)/usr/lib/hwtest/dd_ref0.txt
	$(INSTALL) -D -m 0644 $(HWTEST_DIR)/dd_ref1.txt $(TARGET_DIR)/usr/lib/hwtest/dd_ref1.txt
	$(INSTALL) -D -m 0644 $(HWTEST_DIR)/i2cdetect_ref0.txt $(TARGET_DIR)/usr/lib/hwtest/i2cdetect_ref0.txt
	$(INSTALL) -D -m 0644 $(HWTEST_DIR)/i2cdetect_ref1.txt $(TARGET_DIR)/usr/lib/hwtest/i2cdetect_ref1.txt
	$(INSTALL) -D -m 0644 $(HWTEST_DIR)/i2cdetect_ref2.txt $(TARGET_DIR)/usr/lib/hwtest/i2cdetect_ref2.txt
	$(INSTALL) -D -m 0644 $(HWTEST_DIR)/iw_ref.txt $(TARGET_DIR)/usr/lib/hwtest/iw_ref.txt
	$(INSTALL) -D -m 0644 $(HWTEST_DIR)/lshw_ref.txt $(TARGET_DIR)/usr/lib/hwtest/lshw_ref.txt
	$(INSTALL) -D -m 0644 $(HWTEST_DIR)/wifi_ref0.txt $(TARGET_DIR)/usr/lib/hwtest/wifi_ref0.txt
endef

$(eval $(generic-package))
