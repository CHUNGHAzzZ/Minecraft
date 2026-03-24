#include <string.h>

#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "auto_save/auto_save_module.h"
#include "cmd/commandline_opt.h"
#include "config/local_config.h"
#include "config/software_config.h"
#include "config/system_config.h"
#include "context.h"
#include "font/font_manager.h"
#include "path/path_manager.h"
#include "permission/permissionmanager.h"
#include "util/core_util.h"
#include "permission/unpay.h"
int main88(int argc, const char* argv[]) {
    bool file_bad = rabbit::JsonFileBad("D:\\abc\\CHITUBOX Pro\\machinecfg\\list.cfgx");
    // ��¼����
    std::string login_record_value = "131";
    // ʹ��ʱ��/����
    std::string used_time_value = "2147483659";
    // ��Ȩ����ʱ��
    std::string auth_time_value = "1724747745";
    // ��������ʱ��
    std::string init_time_value = "1724747738";
    if (!login_record_value.empty()) {
        int used_time = used_time_value.empty() ? 0 : std::atoi(used_time_value.c_str());
        int login_count = std::atoi(login_record_value.c_str());
        size_t auth_time = auth_time_value.empty() ? 0 : std::stoul(auth_time_value.c_str());
        size_t init_time = init_time_value.empty() ? 0 : std::stoul(init_time_value.c_str());
        // ��¼��������100�Σ�ʹ��ʱ�䳬��10��
        if (login_count > 100 && used_time > 10 * 24 * 60) {
            if (init_time > auth_time) {
                // ���û����ȨУ��ʱ��
                while (1) { std::this_thread::sleep_for(std::chrono::milliseconds(500)); }
            } else {
                LOGW << "end time engine1";
            }
        } else {
            LOGW << "end time engine1";
        }
    }

    rabbit::PathManager path_mgr("D:\\abc\\CHITUBOX Dental", "CHITUBOX Dental");
    path_mgr.SetAppDataDir("C:\\Users\\admin\\AppData\\Local");
    path_mgr.SetTempDir("C:\\Users\\admin\\AppData\\Local\\Temp");
    path_mgr.Init("12345", rabbit::UpgradeLevel::NormalUpgrade);

    return 0;
    rabbit::SoftwareConfig config;
    rabbit::RWSoftwareConfig<rabbit::SoftwareConfig>().ReadConfig(
        "C:\\Program Files\\CHITUBOX Pro\\resources\\conf\\SoftwareConfig.json", config);
    rabbit::RWSoftwareConfig<rabbit::SoftwareConfig>().WriteConfig(
        "C:\\Users\\admin\\Desktop\\22\\SoftwareConfig.json", config);
    return 0;
    rabbit::ClientConfig cli_config;
    rabbit::LocalConfig().ReadConfig("D:\\redrabbit_pro\\redrabbitproject\\bin\\config1.json", cli_config);
    cli_config.setting_config.file_other_setting.character_replace_setting.replace_items = "a,b,c,d,e,f,g";
    rabbit::LocalConfig().WriteConfig("D:\\redrabbit_pro\\redrabbitproject\\bin\\config1.json", cli_config);
    rabbit::ClientConfig cli_config1;
    rabbit::LocalConfig().ReadConfig("D:\\redrabbit_pro\\redrabbitproject\\bin\\config1.json", cli_config1);
    getchar();
    return 0;
}
int main1(int argc, const char* argv[]) {
    rabbit::ShortcutConfig config;
    rabbit::RWSoftwareConfig<rabbit::ShortcutConfig>().ReadConfig("/Volumes/DISK_IMG/list(1).json", config);
    rabbit::RWSoftwareConfig<rabbit::ShortcutConfig>().WriteConfig(
        "C:\\Users\\admin\\Desktop\\22\\SoftwareConfig.json", config);
    return 0;
}
class CommandOption : public rabbit::CommandOptionBase {
public:
    virtual void LoadModels(const rabbit::CommandList& command, const rabbit::CommandInfo* self) {
        int i = 0;
    };
    virtual void LoadModelsFromFolders(const rabbit::CommandList& command, const rabbit::CommandInfo* self) {
        int i = 0;
    };
    virtual void LoadSliceFile(const rabbit::CommandList& command, const rabbit::CommandInfo* self) {
        int i = 0;
    };
    virtual void Info(const rabbit::CommandList& command, const rabbit::CommandInfo* self) { int i = 0; };
    virtual void Switch(const rabbit::CommandList& command, const rabbit::CommandInfo* self) { int i = 0; };
    virtual void Exec(const rabbit::CommandList& command, const rabbit::CommandInfo* self, bool& is_interrupt) { int i = 0; };
};
int main000(int argc, const char* argv[]) {
    CommandOption option;
    rabbit::CommandLineOptSingleton::Instance().SetInstance(&option);

    // rabbit::RabbitContext context("D:\\abc\\CHITUBOX Pro", "CHITUBOX Pro");
    // context.GetPathManager().SetAppDataDir("C:\\Users\\admin\\AppData\\Local");
    // context.GetPathManager().SetTempDir("C:\\Users\\admin\\AppData\\Local\\Temp");
    // context.Init(algo::UISupportType_Default);
    // rabbit::CommandLineOptSingleton::Instance().ExecCommandLine(argc, (char**)argv, {"-r"});
    // rabbit::CommandLineOptSingleton::Instance().ExecFromJson("{ \"-loadmodels\": {\"path\":\"c:\"},
    // \"-loadfolders\" : {\"path\":\"d:\"}, \"-exec\" : { \"priority\": 2,\"export\": { \"export2\": \"e2:\",
    // \"export_mode3\" : 33 }, \"export_mode\" : 2 }, \"-exec\" : { \"export0\": \"e4:\", \"export_mode0\" :
    // 4 }}", {"-exec"});
    rabbit::CommandLineOptSingleton::Instance().ExecFromJson(
        //"{\"-exec\": [{\"name\": \"loadmodels\", \"priority\":3,\"path\": [\"d:\",\"d:\"]},{\"name\": \"loadmodels1\",\"priority\":4,\"path\": [\"d1:\",\"d2:\"]}]}");
        "{\"-exec\" : { \"name\": \"auto_layout\", \"path\":[\"d:\",\"d:\"]}}");
    // context.Destory();
    return 0;
}
int main(int argc, const char* argv[]) {
    // rabbit::PermissionMgrSingleton::Instance().GetPayModule(PayModuleType::PayModuleType_MultiPlatform)->SetVisibled();
   // auto out = rabbit::PermissionMgrSingleton::Instance().SerialPayInfos();
   // bool ret = rabbit::PermissionMgrSingleton::Instance().DeserialPayInfos(out);
   // rabbit::PermissionMgrSingleton::Instance().VerifyPayModules();
    
    rabbit::RabbitContext context("C:\\Program Files\\CHITUBOX Pro", "CHITUBOX Pro");
    context.GetPathManager().SetAppDataDir("C:\\Users\\admin\\AppData\\Local");
    context.GetPathManager().SetTempDir("C:\\Users\\admin\\AppData\\Local\\Temp");
    context.Init(algo::UISupportType_Default);

    std::this_thread::sleep_for(std::chrono::seconds(10000));
    context.Destory();
    
    return 0;
}