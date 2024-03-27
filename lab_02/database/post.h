#pragma once

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>

namespace database
{
    class Post{
        private:
            long _id;
            long _user_id;
            std::string _post_text;
            std::string _date_time;
            std::string _media;

        public:

            static Post fromJSON(const std::string & str);

            long             get_id() const;
            long             get_user_id() const;
            const std::string &get_post_text() const;
            const std::string &get_date_time() const;
            const std::string &get_media() const;

            long&        id();
            long&        user_id();
            std::string &post_text();
            std::string &date_time();
            std::string &media();

            static void init();
            static std::optional<Post> read_by_id(long id);
            static std::vector<Post> read_by_user_id(long user_id);
            void add();
            void update();

            Poco::JSON::Object::Ptr toJSON() const;

    };
}