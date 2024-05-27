#pragma once

#include <string>
#include <vector>
#include <optional>

#include <Poco/JSON/Object.h>

namespace database
{
    class Message{
        private:
            long _message_id;
            long _sender_id;
            long _recipient_id;
            std::string _text;
            std::string _timestamp;
            bool _is_read;
            struct Attachment {
                std::string type;
                std::string url;
                const std::string to_string() const;
            };
            Attachment _attachment;


        public:

            static Message fromJSON(const std::string & str);

            long               get_message_id() const;
            long               get_sender_id() const;
            long               get_recipient_id() const;
            const std::string &get_text() const;
            const std::string &get_timestamp() const;
            bool               get_is_read() const;
            const std::string  get_attachment_as_string() const;

            long&        message_id();
            long&        sender_id();
            long&        recipient_id();
            std::string& text();
            std::string& timestamp();
            bool&        is_read();
            Attachment&  attachment();

            static std::optional<Message> read_by_id(long id);
            static std::vector<Message> read_by_recipient_id(long recipient_id);
            void add();
            void update();
            Poco::JSON::Object::Ptr toJSON() const;

    };
}