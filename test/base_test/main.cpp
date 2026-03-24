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
#include "base/base_util.h"
#include "base/blockingqueue.h"
#include "base/consumer.h"
#include "base/file_util.h"
#include "base/logfile.h"
#include "base/logging.h"
#include "base/threadpool.h"
#include "base/ticktime.h"

constexpr size_t thread_num_ = 4;
base::ThreadPool pool_;
// 多线程测试
std::atomic<size_t> total_count_{0};

void TestTimer(base::XTimer* xtimer) {
    constexpr uint64_t period = 1000 * 1000;  // 每秒打印一次
    xtimer->StartTimer(period, []() { LOGW << "this is timer print."; });
}

void TestConsume(base::CVConsumer* consumer) {
    consumer->CallAsync([&]() {
        for (size_t i = 0; i < 10000; i++) total_count_++;
    });
}

void TestFileOP() {
#if defined(_WIN32) || defined(_WIN32_) || defined(WIN32) || defined(_WIN64_) || defined(WIN64) || \
    defined(_WIN64)
    const char* path = "C:\\田先润\\123\\467";
    if (!base::FileUtil::IsDirectoryExist(path)) {
        CTCHECK(base::FileUtil::RecursionCreateDir(path)) << "RecursionCreateDir success.";
    }
    if (!base::FileUtil::IsDirectoryExist(path)) {
        LOGE << "not found path:" << std::string(path);
    } else {
        LOGE << "create path success. path:" << std::string(path);
        CTCHECK(base::FileUtil::RemoveDirectoryEx("C:\\田先润")) << "remove dir failed";
    }
#endif
}

int main(int argc, const char* argv[]) {
    for (size_t i = 0; i < 10; i++) LOGI << "uuid:" << base::UUID::CoCreateGuid();

    TestFileOP();
    pool_.Launch(thread_num_);
    pool_.WaitLaunch();
    // 定时器占用一个
    base::XTimer xtimer;
    constexpr size_t time_thread_num = 1;
    for (size_t i = 0; i < time_thread_num; i++) {
        pool_.CallAsync([](base::XTimer* t) { t->Attch(); }, &xtimer);
    }

    // 消费者占用3个
    base::CVConsumer consumer;
    constexpr size_t con_thread_num = 3;
    for (size_t i = 0; i < con_thread_num; i++) {
        pool_.CallAsync([](base::CVConsumer* t) { t->Attch(); }, &consumer);
    }

    auto start_time = base::GetTickTimeMS();
    // 测试定时器
    TestTimer(&xtimer);

    TestConsume(&consumer);

    LOGW << "use time:" << base::GetTickTimeMS() - start_time << " ms.";

    // 这里稍等一会
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));

    // 打印多线程结果
    LOGW << "total_count:" << total_count_.load();

    LOGE << "begin exit";
    consumer.SetStop();
    xtimer.SetStop();
    pool_.SetStop();

    std::cout << "Hello, World!\n" << std::endl;

    getchar();
    return 0;
}
