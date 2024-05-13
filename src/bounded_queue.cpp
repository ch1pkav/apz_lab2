#include <hazelcast/client/hazelcast_client.h>

int main() {

    auto provider = std::jthread([]() {
        hazelcast::client::client_config config;
        config.set_cluster_name("dev");

        auto client = hazelcast::new_client(std::move(config)).get();

        auto queue = client.get_queue("default").get();

        queue->clear().get();

        for (int i = 0; i < 100; i++) {
            std::cout << "Provider: " << i << std::endl;
            queue->put(i).get();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        queue->put(-1).get();

        std::cout << "Provider finished" << std::endl;

        return 0;
    });

    std::vector<std::thread> consumers;
    consumers.reserve(2);
    consumers.emplace_back([]() {
        hazelcast::client::client_config config;
        config.set_cluster_name("dev");

        auto client = hazelcast::new_client(std::move(config)).get();

        auto queue = client.get_queue("default").get();

        while (true) {
            auto value = queue->take<int>().get();
            if (value) {
                if (value.value() == -1) {
                    queue->put(-1).get();
                    break;
                }
                std::cout << "consumer " << 0 << ": " << value << std::endl;
            } else
                std::cerr << "Failed to take value from queue" << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        return 0;
    });

    consumers.emplace_back([]() {
        hazelcast::client::client_config config;
        config.set_cluster_name("dev");

        auto client = hazelcast::new_client(std::move(config)).get();

        auto queue = client.get_queue("default").get();

        while (true) {
            auto value = queue->take<int>().get();
            if (value) {
                if (value.value() == -1) {
                    queue->put(-1).get();
                    break;
                }
                std::cout << "consumer " << 1 << ": " << value << std::endl;
            } else
                std::cerr << "Failed to take value from queue" << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        return 0;
    });

    for (auto& consumer : consumers) {
        consumer.join();
    }
    provider.join();

}