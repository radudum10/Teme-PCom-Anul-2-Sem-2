#include "subscriber.h"

void get_srv_opcode(std::string &opcode, int &code) {
    const std::unordered_map<std::string, std::function<void()>> srv_codes = {
            {"\\$EXIT", [&]() {code = EXIT_CODE;}},
            {"\\$NO_TOPIC", [&]() {code = NO_TOPIC;}},
            {"\\$SUBSCRIBED", [&]() {code = SUBSCRIBED;}},
            {"\\$UNSUBSCRIBED", [&]() {code = UNSUBSCRIBED;}},
            {"\\$ALREADY_SUB", [&]() {code = ALREADY_SUB;}}
    };

    static const auto end = srv_codes.end();
    auto it = srv_codes.find(opcode);
    if (it != end) {
        it->second();
    } else {
        code = -1;
    }
}

void send_subscription(int sockfd, char *buffer) {
    /* Split the message by ' ' and get the topic and SF. */
    char *topic, *sf;
    strtok(buffer, " ");
    topic = strtok(nullptr, " ");
    if (topic == nullptr) {
        std::cout << "Usage: subscribe <TOPIC> <SF>" << std::endl;
        return;
    }

    /* Topic shouldn't be longer than 50 characters. */
    if (strlen(topic) > 50) {
        std::cout << "Warning! Topic should be less than 50 characters." << std::endl;
        topic = strndup(topic, 50);
    }

    sf = strtok(nullptr, " ");
    if (sf == nullptr) {
        std::cout << "Usage: subscribe <TOPIC> <SF>" << std::endl;
        return;
    }

    if (strtok(nullptr, " ") != nullptr) {
        std::cout << "Usage: subscribe <TOPIC> <SF>" << std::endl;
        return;
    }

    bool sf_booled = strtol(sf, nullptr, 10) == 1;
    /* Create a TCP message and send it to the server. */
    from_tcp new_msg;
    memset(&new_msg, 0, sizeof(new_msg));
    new_msg.type = SUBSCRIBE;
    memcpy(new_msg.topic, topic, strlen(topic));
    new_msg.sf = sf_booled;

    proto_send(sockfd, &new_msg, sizeof(new_msg));
}

void remove_subscription(int sockfd, char *buffer) {
    /* Split the message by ' ' and get the topic. */
    char *topic;
    strtok(buffer, " ");

    topic = strtok(nullptr, " ");
    if (topic == nullptr) {
        std::cout << "Usage: unsubscribe <TOPIC>" << std::endl;
        return;
    }

    if (strtok(nullptr, " ") != nullptr) {
        std::cout << "Usage: unsubscribe <TOPIC>";
        return;
    }

    /* Topic shouldn't be longer than 50 characters. */
    if (strlen(topic) > 50) {
        std::cout << "Warning! Topic should be less than 50 characters." << std::endl;
        topic = strndup(topic, 50);
    }

    /* Create a TCP message and send it to the server. */
    from_tcp new_msg;
    memset(&new_msg, 0, sizeof(new_msg));
    new_msg.type = UNSUBSCRIBE;
    memcpy(new_msg.topic, topic, strlen(topic));
    new_msg.sf = false;

    proto_send(sockfd, &new_msg, sizeof(new_msg));
}

bool recv_from_stdin(int sockfd, fd_set *tmp_fds) {
    if (FD_ISSET(STDIN_FILENO, tmp_fds)) {
        /* Read the input from stdin. */
        char buffer[BUFF_LEN];
        BUFF_CLEAR(buffer);
        size_t status = read(STDIN_FILENO, buffer, BUFF_LEN);
        if (buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
        }
        DIE(status < 0, "read stdin");

        if (strncmp(buffer, "exit", 4) == 0) {
            return true;
        }

        if (strncmp(buffer, "subscribe", 9) == 0) {
            send_subscription(sockfd, buffer);
        } else if (strncmp(buffer, "unsubscribe", 11) == 0) {
            remove_subscription(sockfd, buffer);
        }
    }
    return false;
}

void opcode_received(int code) {
    switch (code) {
        case EXIT_CODE: {
            exit(0);
        }

        case NO_TOPIC: {
            std::cout << "No such topic." << std::endl;
            break;
        }

        case SUBSCRIBED: {
            std::cout << "Subscribed to topic." << std::endl;
            break;
        }

        case UNSUBSCRIBED: {
            std::cout << "Unsubscribed from topic." << std::endl;
            break;
        }

        case ALREADY_SUB: {
            std::cout << "Already subscribed to topic." << std::endl;
            break;
        }

        default: {
            std::cout << "Unknown opcode (this shouldn't be reached)" << std::endl;
            break;
        }
    }
}

void identify_msg(cli_msg new_msg) {
    uint16_t port = ntohs(new_msg.port);

    std::string print_val;
    std::string print_type;

    switch (new_msg.recv_msg.type) {
        case INT_TYPE: {
            uint32_t int_val = ntohl(*(uint32_t *) (new_msg.recv_msg.payload + 1));

            int signed_val;
            if (new_msg.recv_msg.payload[0] == 1) {
                signed_val = (int) (int_val * -1);
            } else {
                signed_val = (int) int_val;
            }

            print_val = std::to_string(signed_val);
            print_type = "INT";
            break;
        }

        case SHORT_TYPE: {
            double multi = ntohs(*(uint16_t *) (new_msg.recv_msg.payload));
            multi /= 100;
            std::string full_val = std::to_string(multi);
            print_val = full_val.substr(0, full_val.find('.') + 3);
            print_type = "SHORT_REAL";
            break;
        }

        case FLOAT_TYPE: {
            double float_val = ntohl(*(uint32_t *) (new_msg.recv_msg.payload + 1));
            float_val /= pow(10, new_msg.recv_msg.payload[5]);
            if (new_msg.recv_msg.payload[0] == 1) {
                float_val *= -1;
            }

            print_val = std::to_string(float_val);
            print_type = "FLOAT";
            break;
        }

        case STRING_TYPE: {
            print_val = std::string(new_msg.recv_msg.payload);
            print_type = "STRING";
            break;
        }

        default: {
            std:: cout << "Unknown type!" << std::endl;
            return;
        }
    }

    std::cout << inet_ntoa(new_msg.ip) << ":" << port << " " <<
              new_msg.recv_msg.topic << " - " << print_type << " - " << print_val << std::endl;
}