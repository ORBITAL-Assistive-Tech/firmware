/* Code to test helper/setup functions for the SD Card

*/

#include <sys/unistd.h> // lets you pass to stdout
#include "esp_log.h" // print to esp monitor
#include "esp_err.h" // run esp code in error catcher macro
#include "esp_vfs_fat.h" // mounting and unmounting fat filesystem
#include "sd_card_pins.h" // edit this file to configure pins

static const char *LOGTAG = "test_sd";

void app_main(void)
{
  ESP_LOGI(LOGTAG, "Hello world!");

}





// Things needed to mount and unmount
// basically we need everything that is passed into this function to successfully mount.
// 
// esp_err_t mount_successful = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);
// ESP_LOGE(LOGTAG, "Attempting to mount sd card. Showing: %s", esp_err_to_name(mount_successful))
// sdmmc_card_print_info(stdout, card); // make sure card is mounted
// 
//ESP_ERROR_CHECK(
// esp_vfs_fat_sdcard_unmount(mount_point, card)
// ); // all-in-one unmount
