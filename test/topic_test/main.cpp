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

#include "base/threadpool.h"
#include "core/topic/message.h"
#include "core/topic/subscription.h"
#include "core/topic/topicmanager.h"

struct TestStruct {
    int a;
    int b;
    int c;
};
struct TestStruct2 {
    int a;
    int b;
    int c;
};
namespace rabbit {
TOPIC_ID_CREATOR(TestStruct);
TOPIC_ID_CREATOR(TestStruct2);
}  // namespace rabbit
using TestStructMessage = rabbit::Message<TestStruct>;
using TestStructMessage2 = rabbit::Message<TestStruct2>;
// 测试用随机topic id转发消息，需要传递topic id
void Test1() {
    // 随机生成一个id
    rabbit::SubscriptionHelper helper;
    // 绑定函数和id
    helper.Subscribe<TestStructMessage, void(TestStructMessage&)>(
        [](TestStructMessage& msg) { msg.Body().a = 7; });

    std::shared_ptr<TestStructMessage> test_msg = std::make_shared<TestStructMessage>();
    test_msg->Body().a = 1;
    test_msg->Body().b = 2;
    test_msg->Body().c = 3;
    // publish的时候代入id
    rabbit::TopicManagerSingleton::Instance().PublishAsync(test_msg);

    // 因为是异步转发消息，所以需要等一会
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    LOGW << "value:" << test_msg->Body().a;
}

// 测试用固定topic id转发消息
// 此种模式虽然不用传递topic id，非常方便，但不建议随便用，容易造成维护困难
void Test2() {
    rabbit::SubscriptionHelper helper;
    // 绑定函数和id
    helper.Subscribe<TestStructMessage, void(TestStructMessage&)>(
        [](TestStructMessage& msg) { msg.Body().a = 7; });

    std::shared_ptr<TestStructMessage> test_msg = std::make_shared<TestStructMessage>();
    test_msg->Body().a = 1;
    test_msg->Body().b = 2;
    test_msg->Body().c = 3;
    // publish的时候代入id
    rabbit::TopicManagerSingleton::Instance().PublishAsync(test_msg);

    // 因为是异步转发消息，所以需要等一会
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    LOGW << "value:" << test_msg->Body().a;
}

void Test3() {
    rabbit::SubscriptionHelper helper;
    // 绑定函数和id
    helper.Subscribe<TestStructMessage, void(TestStructMessage&)>(
        [](TestStructMessage& msg) { msg.Body().a = 7; });

    helper.Subscribe<TestStructMessage, void(TestStructMessage&)>(
        [](TestStructMessage& msg) { msg.Body().b = 9; });

    std::shared_ptr<TestStructMessage> test_msg = std::make_shared<TestStructMessage>();
    test_msg->Body().a = 1;
    test_msg->Body().b = 2;
    test_msg->Body().c = 3;
    // publish的时候代入id
    rabbit::TopicManagerSingleton::Instance().PublishAsync(test_msg);

    // 因为是异步转发消息，所以需要等一会
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    LOGW << "value:" << test_msg->Body().a << ".b:" << test_msg->Body().b;
}

int main(int argc, const char* argv[]) {
    // 消息转发模块需要1个线程
    const size_t thread_num = 1;
    base::ThreadPool pool_;
    pool_.Launch(thread_num);
    pool_.WaitLaunch();

    base::CVConsumer consumer;
    constexpr size_t con_thread_num = 1;
    for (size_t i = 0; i < con_thread_num; i++) {
        pool_.CallAsync([](base::CVConsumer* t) { t->Attch(); }, &consumer);
    }

    rabbit::TopicManagerSingleton::Instance().Init(&consumer);

    Test3();

    std::cout << "Hello, World!\n" << std::endl;

    getchar();
    return 0;
}
