#ifndef DMI_UTILS_FILESYSTEM_HPP_
#define DMI_UTILS_FILESYSTEM_HPP_
#include <string>



namespace dmi_utils {


/**
 * @brief ログディレクトリを作る
 *
 * もしすでに存在する場合は何もしない
 *
 * @param dir_path ログディレクトリへのパス
 *
 * @return True(作成完了orすでに存在する場合) False(パーミッションの都合などで失敗した場合)
 */
bool CreateLogDirIfNotExists(const std::string& dir_path);

}
#endif
