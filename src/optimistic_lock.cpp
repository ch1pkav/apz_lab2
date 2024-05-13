#include <hazelcast/client/hazelcast_client.h>
#include <thread>

using namespace std::string_literals;

int main() {
    std::vector<std::thread> threads;
    threads.reserve(3);
    for (int i = 0; i < 3; i++) {
        threads.emplace_back([]() {
            hazelcast::client::client_config config;
            config.set_cluster_name("dev");

            auto client = hazelcast::new_client(std::move(config)).get();

            auto map = client.get_map("my-distributed-map").get();

            map->put_if_absent("key"s, 0);

            for (int i = 0; i < 10000; i++) {
                for (; ; ) {
                    auto value = map->get<std::string, int>("key"s).get().value();
                    auto new_value = value + 1;
                    if (map->replace("key"s, value, new_value).get())
                        break;
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
