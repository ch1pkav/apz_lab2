#include <hazelcast/client/hazelcast_client.h>

int main() {
    hazelcast::client::client_config config;
    config.set_cluster_name("dev");

    auto client = hazelcast::new_client(std::move(config)).get();

    auto map = client.get_map("my-distributed-map").get();

    for (int i = 0; i < 1000; i++) {
        map->put(std::to_string(i), std::to_string(i)).get();
    }

    return 0;
}