/* Code to test helper/setup functions for the SD Card

*/

#include <sys/unistd.h> // lets you pass to stdout
#include "esp_log.h" // print to esp monitor
#include "esp_err.h" // run esp code in error catcher macro
#include "esp_vfs_fat.h" // mounting and unmounting fat filesystem
#include "driver/sdmmc_host.h" // set frequency etc. for the esp
#include "sd_card_config.h" // edit this file to configure pins and things
#include "sd_protocol_types.h"

static const char *LOGTAG = "test_sd";

void app_main(void)
{
  ESP_LOGI(LOGTAG, "Hello world!");

}





// Things needed to mount and unmount
sdmmc_host_t host = SDMMC_HOST_DEFAULT();
// we can also change the host max frequency or set it to high speed with extra pins but I don't think we're doing that
esp_vfs_fat_sdmmc_mount_config_t mount_config = {
  .format_if_mount_failed = false, // will delete files 
  .allocation_unit_size = 16*1024, // only matters when format_if_mount_fails is true
  .use_one_fat = false, // adds space but decreases reliability after reformatting
  .max_files = 5, // max number of open files; may affect IO speed
  .disk_status_check_enable = false, // Set to handle removal without ejection or other issues
};

sdmmc_card_t *card; // store info about card

// 
// basically we need everything that is passed into this function to successfully mount.
// 
 esp_err_t mount_successful = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);
ESP_LOGE(LOGTAG, "Attempting to mount sd card. Showing: %s", esp_err_to_name(mount_successful))
sdmmc_card_print_info(stdout, card); // make sure card is mounted
// 
ESP_ERROR_CHECK(
esp_vfs_fat_sdcard_unmount(mount_point, card)
); // all-in-one unmount
