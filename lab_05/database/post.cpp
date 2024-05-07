#include "post.h"
#include "database.h"

#include <sstream>

#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

namespace database
{
    Post Post::fromJSON(const std::string &str)
    {
        int start = str.find("_id");
        int end = str.find(",",start);

        std::string s1 = str.substr(0,start-1);
        std::string s2 = str.substr(end+1);

        Post post;

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(s1+s2);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        post.id() = object->getValue<long>("id");
        post.title() = object->getValue<std::string>("title");
        post.text() = object->getValue<std::string>("text");
        post.author_id() = object->getValue<long>("author_id");
        post.post_datetime() = object->getValue<std::string>("post_datetime");
        Poco::JSON::Object::Ptr object_array = object->getObject("comment");
        post._comment.comment_text = object_array->getValue<std::string>("comment_text");
        post._comment.comment_author_id = object_array->getValue<long>("comment_author_id");
        post._comment.comment_datetime = object_array->getValue<std::string>("comment_datetime");

        return post;
    }

    long Post::get_id() const
    {
        return _id;
    }

    const std::string &Post::get_title() const
    {
        return _title;
    }

    const std::string &Post::get_text() const
    {
        return _text;
    }

    long Post::get_author_id() const
    {
        return _author_id;
    }

    const std::string &Post::get_post_datetime() const
    {
        return _post_datetime;
    }

    const std::string  Post::get_comment_as_string() const
    {
        return _comment.to_string();
    }

    const std::string Post::Comment::to_string() const
    {
        std::stringstream ss;
        ss << comment_text << ", " << comment_author_id << ", " << comment_datetime;
        return ss.str();
    }

    long &Post::id()
    {
        return _id;
    }

    std::string &Post::title()
    {
        return _title;
    }

    std::string &Post::text()
    {
        return _text;
    }

    long &Post::author_id()
    {
        return _author_id;
    }

    std::string &Post::post_datetime()
    {
        return _post_datetime;
    }

    Post::Comment &Post::comment()
    {
        return _comment;
    }

    std::optional<Post> Post::read_by_id(long id)
    {
        std::optional<Post> result;
        std::map<std::string,long> params;
        params["id"] = id;
        std::vector<std::string> results = database::Database::get().get_from_mongo("arch.posts",params);

        if(!results.empty())
            result = fromJSON(results[0]);
        
        return result;
    }

    std::vector<Post> Post::read_by_author_id(long author_id)
    {
        std::vector<Post> result;
        std::map<std::string,long> params;
        params["author_id"] = author_id;

        std::vector<std::string> results = database::Database::get().get_from_mongo("arch.posts",params);

        for(std::string& s : results) 
            result.push_back(fromJSON(s));
        

        return result;
    }

    void Post::add()
    {
        database::Database::get().send_to_mongo("posts",toJSON());
    }

    void Post::update()
    {
        std::map<std::string,long> params;
        params["id"] = _id;       
        database::Database::get().update_mongo("posts",params,toJSON());
    }

    Poco::JSON::Object::Ptr Post::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("title", _title);
        root->set("text", _text);
        root->set("author_id", _author_id);
        root->set("post_datetime", _post_datetime);

        Poco::JSON::Object::Ptr comment = new Poco::JSON::Object();

        comment->set("comment_text",_comment.comment_text);
        comment->set("comment_author_id",_comment.comment_author_id);
        comment->set("comment_datetime",_comment.comment_datetime);

        root->set("comment", comment);


        return root;
    }
}