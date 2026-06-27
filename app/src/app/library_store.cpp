#include "library_store.hpp"

#include "app_config.hpp"
#include "roah/distb/errors.hpp"
#include "roah/distb/logger.hpp"
#include "roah/distb/utils/string.hpp"
#include "roah/distb/utils/subprocess.hpp"

#include <nlohmann/json.hpp>

#include <fstream>

roah::distb::app::LibraryStore::LibraryStore(const AppConfig & app_config)
    : app_config_{ app_config }
    , commit_hash_{}
{}

roah::distb::app::LibraryStore::~LibraryStore() noexcept = default;

void
roah::distb::app::LibraryStore::fetch()
{
    std::filesystem::create_directories(this->app_config_.getAppStoragePath());

    const auto store_json_file_path = this->app_config_.getAppStoragePath() / u8"store.json";

    nlohmann::json j_root;
    std::ifstream  ifst{ store_json_file_path };
    if (ifst)
    {
        try
        {
            j_root = nlohmann::json::parse(ifst);
        }
        catch (const nlohmann::json::parse_error & e)
        {
            logger.log("Failed to open library store file. {}", e.what());
            logger.log("Regenerating.");
        }
    }

    // 最終 fetch 時間を読み込む.
    std::int64_t epoch = 0;
    if (const auto iter = j_root.find("last_fetched"); iter != j_root.end())
    {
        iter->get_to(epoch);
    }
    this->last_fetched_at_ = std::chrono::system_clock::time_point{ std::chrono::seconds{ epoch } };

    // commit hash を読み込む.
    if (const auto iter = j_root.find("commit_hash"); iter != j_root.end())
    {
        iter->get_to(this->commit_hash_);
    }

    logger.log("LibraryStore: Last fetched at: {:%FT%TZ}, Commit Hash: {}", this->last_fetched_at_, this->commit_hash_);

    const auto                     now = std::chrono::system_clock::now();
    constexpr std::chrono::minutes fetch_interval{ 10 };

    if ((now - this->last_fetched_at_) > fetch_interval)
    {
        // Fetch
        logger.log("Fetching library store from GitHub...");
        std::string new_commit_hash;
        {
            const auto f_commit = utils::run(
                {
                    u8"curl",
                    u8"https://api.github.com/repos/WhiteAtelier/distbuilder3-libraries/branches/main",
                    u8"-H",
                    u8"Accept: application/vnd.github+json"  //
                    u8"--silent",
                },
                {
                    .print_stdout   = false,
                    .print_stderr   = logger.isVerbose(),
                    .capture_stdout = true,
                    .capture_stderr = false,
                });

            if (f_commit.exit_code == 0)
            {
                // Parse JSON
                try
                {
                    const auto j_commits = nlohmann::json::parse(f_commit.stdout_output);
                    j_commits.at("commit").at("sha").get_to(new_commit_hash);
                }
                catch (const nlohmann::json::exception & e)
                {
                    logger.log("Failed to parse commit hash. Skip to update. {}", e.what());
                    return;
                }
            }
            else
            {
                logger.log("Failed to fetch commit hash. Skip to update.");
                return;
            }
        }

        logger.log("Local commit hash..........: {}", this->commit_hash_);
        logger.log("Current master commit hash : {}", new_commit_hash);

        if (this->commit_hash_ != new_commit_hash)
        {
            logger.log("Updating libraryies...");

            // 更新があったので, zip を落とす
            const auto working_dir   = this->app_config_.getBuildDirectory() / u8"_libs";
            const auto src_file_path = working_dir / u8"src.zip";

            if (std::filesystem::exists(working_dir))
            {
                std::filesystem::remove_all(working_dir);
            }
            std::filesystem::create_directories(working_dir);

            const auto f_download = utils::run(
                {
                    u8"curl",
                    u8"https://github.com/WhiteAtelier/distbuilder3-libraries/archive/"  //
                        + utils::toU8String(new_commit_hash) + u8".zip",
                    u8"--fail",
                    u8"--location",
                    u8"--output",
                    src_file_path.u8string(),
                },
                {
                    .print_stdout   = false,
                    .print_stderr   = logger.isVerbose(),
                    .capture_stdout = false,
                    .capture_stderr = true,
                });

            if (f_download.exit_code != 0)
            {
                logger.log("LibraryStore: Failed to download library store. Exit code: {}, {}",
                           f_download.exit_code,
                           f_download.stderr_output);
                logger.log("Skipping update.");
                return;
            }

            // Extract
            const auto f_extract = utils::run({
                u8"tar",
                u8"-xf",
                src_file_path.u8string(),
                u8"-C",
                working_dir.u8string(),
            });
            if (f_extract.exit_code != 0)
            {
                logger.log("LibraryStore: Failed to extract library store. Exit code: {}", f_extract.exit_code);
                logger.log("Skipping update.");
                return;
            }

            // Copy
            const auto src_dir
                = working_dir / (u8"distbuilder3-libraries-" + utils::toU8String(new_commit_hash)) / "libs";
            const auto dst_dir = this->app_config_.getAppStoragePath() / u8"libs";

            std::filesystem::create_directories(dst_dir);
            std::filesystem::copy(src_dir, dst_dir, std::filesystem::copy_options::overwrite_existing);

            logger.log("LibraryStore: Updated successfully.");
        }
        else
        {
            logger.log("LibraryStore: up-to-date.");
        }

        // json を更新する
        j_root["last_fetched"] = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        j_root["commit_hash"]  = new_commit_hash;

        std::ofstream ofst{ store_json_file_path };
        if (!ofst)
        {
            logger.log("Failed to open library store file for writing: {}", store_json_file_path.u8string());
            return;
        }
        ofst << j_root.dump(4);
        logger.trace("LibraryStore: Updated store.json.");
    }
}
