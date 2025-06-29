#include "auth_storage.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include <stdlib.h>
#include <string.h>

static const char* TAG = "AUTH_STORAGE";
static nvs_handle_t s_nvs_handle = 0;
static char* token_cache = NULL;
static bool initialized = false;

#define MAX_TOKEN_LEN 256
#define NVS_NAMESPACE "homeass_auth"
#define TOKEN_KEY "bearer_token"

// 内部初始化函数
static int lazy_initialize() {
    if (initialized) return 0;

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "NVS init failed: %s", esp_err_to_name(ret));
        return -1;
    }

    ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &s_nvs_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "NVS open failed: %s", esp_err_to_name(ret));
        return -2;
    }

    initialized = true;
    return 0;
}

// 内部加载token函数
static int load_token_to_cache() {
    if (token_cache) {
        free(token_cache);
        token_cache = NULL;
    }

    size_t required_size = 0;
    esp_err_t ret = nvs_get_str(s_nvs_handle, TOKEN_KEY, NULL, &required_size);
    if (ret != ESP_OK || required_size == 0) {
        return -1;
    }

    token_cache = malloc(required_size);
    if (!token_cache) {
        return -2;
    }

    ret = nvs_get_str(s_nvs_handle, TOKEN_KEY, token_cache, &required_size);
    if (ret != ESP_OK) {
        free(token_cache);
        token_cache = NULL;
        return -3;
    }

    return 0;
}

// 主接口实现
const char* auth_get_token() {
    // 懒加载初始化
    if (!initialized && lazy_initialize() != 0) {
        return NULL;
    }

    // 如果缓存为空则尝试加载
    if (!token_cache && load_token_to_cache() != 0) {
        return NULL;
    }

    return token_cache;
}

// 可选保留的保存接口
int auth_save_token(const char* token) {
    if (!token) return -1;

    // 确保已初始化
    if (!initialized && lazy_initialize() != 0) {
        return -2;
    }

    // 保存到NVS
    esp_err_t ret = nvs_set_str(s_nvs_handle, TOKEN_KEY, token);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Save failed: %s", esp_err_to_name(ret));
        return -3;
    }

    ret = nvs_commit(s_nvs_handle);
    if (ret != ESP_OK) return -4;

    // 更新缓存
    if (token_cache) {
        free(token_cache);
    }
    token_cache = strdup(token);
    if (!token_cache) {
        return -5;
    }

    return 0;
}