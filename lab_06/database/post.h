#pragma once

#include <string>
#include <vector>
#include <optional>

#include <Poco/JSON/Object.h>

namespace database
{
    class Post{
        private:
            long _id;
            std::string _title;
            std::string _text;
            long _author_id;
            std::string _post_datetime;
            struct Comment {
                std::string comment_text;
                long comment_author_id;
                std::string comment_datetime;
                const std::string to_string() const;
            };
            Comment _comment;


        public:

            static Post fromJSON(const std::string & str);

            long               get_id() const;
            const std::string &get_title() const;
            const std::string &get_text() const;
            long               get_author_id() const;
            const std::string &get_post_datetime() const;
            const std::string  get_comment_as_string() const;

            long&        id();
            std::string& title();
            std::string& text();
            long&        author_id();
            std::string& post_datetime();
            Comment&     comment();

            static std::optional<Post> read_by_id(long id);
            static std::vector<Post> read_by_author_id(long author_id);
            void add();
            void update();
            Poco::JSON::Object::Ptr toJSON() const;

    };
}