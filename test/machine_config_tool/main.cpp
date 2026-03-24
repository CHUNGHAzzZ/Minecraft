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

#include "context.h"
#include "config/system_config.h"
#include "font/font_manager.h"
#include "auto_save/auto_save_module.h"
#include "config/local_config.h"
#include "config/software_config.h"
#include "path/path_manager.h"
#include "machine_config/merge_machine_config.h"
int main(int argc, const char* argv[]) {
    rabbit::PathManager path_mgr("D:\\abc\\CHITUBOX Dental", "CHITUBOX Dental");
    path_mgr.SetAppDataDir("C:\\Users\\admin\\AppData\\Local");
    path_mgr.SetTempDir("C:\\Users\\admin\\AppData\\Local\\Temp");
    path_mgr.Init("12345", rabbit::UpgradeLevel::NormalUpgrade);
   // rabbit::MergeMachineConfig merge(&path_mgr, (int)rabbit::UpgradeLevel::ReplaceUpgrad);
   // merge.Merge("D:\\rabbit_machine_config\\bin\\machinecfg", rabbit::UpgradeLevel::NormalUpgrade, );

    
    return 0;
}
