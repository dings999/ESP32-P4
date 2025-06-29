#ifndef AUTH_STORAGE_H
#define AUTH_STORAGE_H

#include <stddef.h>

// 唯一对外接口：获取token（自动处理初始化）
// 返回值为token指针，NULL表示获取失败
const char* auth_get_token();

// 保存token（可选保留）
int auth_save_token(const char* token);

#endif